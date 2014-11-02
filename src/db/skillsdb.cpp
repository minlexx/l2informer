#include <QVariant>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QSqlError>
#include <QSplashScreen>
#include <QApplication>

#include "skillsdb.h"
#include "skillgrp_loader.h"
#include "utils_db.h"


static Skillgrp_loader *_s_internal_skillgrp = NULL;

SkillsDB *SkillsDB::_s_instance = NULL;



SkillsDB *SkillsDB::getInstance() {
    if( !_s_instance ) {
        _s_instance = new SkillsDB();
    }
    return _s_instance;
}


SkillsDB::SkillsDB() {
    m_splashScreenPointer = NULL;
    _initDb();
}


bool SkillsDB::_initDb() {
    _db = QSqlDatabase::addDatabase( "QSQLITE", "skilltree_db" );
    _db.setDatabaseName( ":memory:" );
    if( !_db.open() ) {
        qWarning( "SkillsDB: failed to open SQlite memory database!" );
        return false;
    } else {
        QSqlQuery query( _db );
        query.exec( "CREATE TABLE skills ( id int primary key, name text )" );
        _detectSqlError( query );
        qDebug( "SkillsDB: memory SQLite DB init OK" );
        return true;
    }
}


void SkillsDB::_detectSqlError( const QSqlQuery& query ) {
    QSqlError err = query.lastError();
    if( err.isValid() ) {
        qWarning( "SkillsDB: SQLite ERROR: [%s]", err.text().toUtf8().data() );
    }
}


void SkillsDB::_addSkillToDb( const L2Skill& skill ) {
    QSqlQuery query( _db );
    query.prepare( "INSERT INTO skills VALUES( ?, ? )" );
    query.addBindValue( skill.skillId() );
    query.addBindValue( skill.skillName() );
    if( !query.exec() ) {
        this->_detectSqlError( query );
    }
}


int SkillsDB::_db_count_skills() {
    int ret = 0;
    QSqlQuery query( _db );
    if( query.exec("SELECT COUNT(*) AS cnt FROM skills") ) {
        query.next();
        ret = query.value( 0 ).toInt();
    } else {
        _detectSqlError( query );
    }
    return ret;
}


static void _internal_find_skill_icon( L2Skill& skill ) {
    if( _s_internal_skillgrp ) {
        QString iconName = _s_internal_skillgrp->getSkillIconName( skill.skillId() );
        skill.setIconName( iconName );
    } else {
        qWarning( "SkillsDB: _internal_find_skill_icon: _s_internal_skillgrp == NULL!!!" );
    }
}


L2Skill SkillsDB::getSkill( int skillId ) {
    L2Skill ret;
    if( _skills.contains( skillId ) ) {
        ret = _skills[ skillId ];
    } else {
        qWarning( "ERROR: SkillsDB: cannot get skill %d (not in DB!)", skillId );
    }
    return ret;
}


QList<L2Skill> SkillsDB::findSkill( const QString& skillNamePart ) {
    QList<L2Skill> ret;
    QSqlQuery query( _db );
    QString LIKE_ARG = QString("%%1%").arg(skillNamePart); // surround with '%' sign: "%value%"
    QString LIKE_ARG_esc = db_escape_value( &_db, LIKE_ARG ); // escape all this
    //qDebug( "LIKE_ARG, escaped = [%s] [%s]\n", LIKE_ARG.toUtf8().data(), LIKE_ARG_esc.toUtf8().data() );
    if( query.exec( QString("SELECT id, name FROM skills WHERE name LIKE %1").arg(LIKE_ARG_esc) ) ) {
        while( query.next() ) {
            int skillId = query.value( 0 ).toInt();
            if( skillId > 0 ) {
                L2Skill sk = getSkill( skillId );
                ret.append( sk );
            }
        }
    } else {
        _detectSqlError( query );
    }
    return ret;
}


bool SkillsDB::load( void *splashScreenPointer ) {
    if( splashScreenPointer )
        m_splashScreenPointer = splashScreenPointer;
    // load skills icons loader
    _s_internal_skillgrp = new Skillgrp_loader();
    _s_internal_skillgrp->load();
    //
    QDir skillsDir = QDir::current();
    skillsDir.cd( "data/skills" );
    // list all *.xml files in subdirectory data/skills
    QStringList nameFilters;
    nameFilters << "*.xml";
    QList<QFileInfo> fileList = skillsDir.entryInfoList(
        nameFilters, QDir::Files | QDir::Readable, QDir::Name );
    int num_xmls_done = 0;
    int num_fs_skills = 0;
    QListIterator<QFileInfo> iter( fileList );
    try {
        while( iter.hasNext() ) {
            const QFileInfo& fileInfo = iter.next();
            this->loadSkillXML( fileInfo.absoluteFilePath() );
            num_xmls_done++;
        }
        // load forgotten skill tree
        num_fs_skills = this->loadForgottenSkillTree();
    } catch( UnparsedSkillParameterException e ) {
        qWarning( "PARSE ERROR: %s", e.toString().toUtf8().data() );
    } catch( UnparsedSkillForParameterException e ) {
        qWarning( "PARSE ERROR: %s", e.toString().toUtf8().data() );
    } catch( UnparsedSkillAttributeException e ) {
        qWarning( "PARSE ERROR: %s", e.toString().toUtf8().data() );
    }

    // remove skills icons loader from memory - all skills are loaded now
    delete _s_internal_skillgrp;
    _s_internal_skillgrp = NULL;
    //
    qDebug( "SkillsDB: loaded %d XML files", num_xmls_done );
    qDebug( "SkillsDB: Loaded %d skills (in db: %d); forgottenTreeSkills: %d",
            _skills.size(), _db_count_skills(), num_fs_skills );
    return true;
}


int SkillsDB::loadSkillXML( const QString& fileName ) {
    QFile f( fileName );
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        qDebug( "SkillsDB: Failed to read skill XML: %s", fileName.toUtf8().data() );
        return 0;
    }
    qDebug( "SkillsDB: reading %s...", fileName.toUtf8().data() );
    if( m_splashScreenPointer ) {
        QSplashScreen *spl = (QSplashScreen *)m_splashScreenPointer;
        spl->showMessage( QString("Loading file: %1").arg(fileName), Qt::AlignLeft|Qt::AlignBottom );
        qApp->processEvents();
    }
    int num_skills_read = 0;
    QXmlStreamReader xml( &f );
    while( !xml.atEnd() && !xml.hasError() ) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if( token == QXmlStreamReader::StartDocument )
            continue;
        if( token == QXmlStreamReader::StartElement ) {
            if( xml.name() == "skill" ) {
                L2Skill skill = this->parseSkillDefinition( xml );
                if( (skill.skillId()) > 0 && (!skill.skillName().isEmpty()) ) {
                    _internal_find_skill_icon( skill );
                    this->_addSkillToDb( skill );
                    this->_skills.insert( skill.skillId(), skill );
                    num_skills_read++;
                }
            }
        }
        //if( num_skills_read >= 1 )
        //    break;
    }
    if( xml.hasError() ) {
        qCritical( "SkillsDB: XML read error: (%s) %s",
            fileName.toUtf8().data(),
            xml.errorString().toUtf8().data() );
    }
    xml.clear();
    f.close();
    return num_skills_read;
}


L2Skill SkillsDB::parseSkillDefinition( QXmlStreamReader& xml ) {
    // read current skill attributes (id, levels, name)
    QVector<QXmlStreamAttribute> attrs = xml.attributes();
    int skillId = 0;
    int num_levels = 0;
    QString skillName;
    int num_enchants = 0;
    QVectorIterator<QXmlStreamAttribute> sa_iter(attrs);
    while( sa_iter.hasNext() ) {
        QXmlStreamAttribute attr = sa_iter.next();
        if( attr.name() == "id" ) {
            skillId = attr.value().toInt();
        } else if( attr.name() == "levels" ) {
            num_levels = attr.value().toInt();
        } else if( attr.name() == "name" ) {
            skillName = attr.value().toString();
        } else if( (attr.name() == "enchantGroup1") ||
                   (attr.name() == "enchantGroup2") ||
                   (attr.name() == "enchantGroup3") ||
                   (attr.name() == "enchantGroup4") ||
                   (attr.name() == "enchantGroup5") ||
                   (attr.name() == "enchantGroup6") ||
                   (attr.name() == "enchantGroup7") ||
                   (attr.name() == "enchantGroup8") ) {
            QString anames = attr.name().toString();
            QString snum_enchants = anames.mid( 12 ); // only last numbers after word "enchantGroup"
            int max_num_enchants = snum_enchants.toInt();
            if( max_num_enchants > num_enchants )
                num_enchants = max_num_enchants;
        } else {
            throw UnparsedSkillAttributeException( skillId, attr.name().toString(), xml.lineNumber() );
        }
    }
    xml.readNext();
    //
    L2Skill skill;
    skill.setSkillId( skillId );
    skill.setSkillName( skillName );
    skill.setNumLevels( num_levels );
    skill.setNumEnchants( num_enchants );
    // read until the end of element named "skill"
    while( !((xml.tokenType() == QXmlStreamReader::EndElement) && (xml.name() == "skill")) ) {
        if( xml.tokenType() == QXmlStreamReader::StartElement ) {
            if( xml.name() == "table" ) {
                this->parseSkillTableDefinition( skill, xml );
            } else if( xml.name() == "set" ) {
                this->parseSkillSetDefinition( skill, xml );
            } else if( xml.name() == "cond" ) {
                this->parseSkillCondDefinition( skill, xml );
            } else if( (xml.name() == "enchant1") ||
                       (xml.name() == "enchant2") ||
                       (xml.name() == "enchant3") ||
                       (xml.name() == "enchant4") ||
                       (xml.name() == "enchant5") ||
                       (xml.name() == "enchant6") ||
                       (xml.name() == "enchant7") ||
                       (xml.name() == "enchant8") ) {
                this->parseSkillEnchantDefinition( skill, xml );
            } else if( (xml.name() == "for") ||
                       (xml.name() == "enchant1for") ||
                       (xml.name() == "enchant2for") ||
                       (xml.name() == "enchant3for") ||
                       (xml.name() == "enchant4for") ||
                       (xml.name() == "enchant5for") ||
                       (xml.name() == "enchant6for") ||
                       (xml.name() == "enchant7for") ||
                       (xml.name() == "enchant8for") ) {
                this->parseSkillForDefinition( skill, xml );
            } else { // unknown element?
                throw UnparsedSkillParameterException( skillId, xml );
            }
        }
        // anyways, read next
        xml.readNext();
    }
    return skill;
}


void SkillsDB::parseSkillTableDefinition( L2Skill& skill, QXmlStreamReader& xml ) {
    QXmlStreamAttributes attrs = xml.attributes();
    QString tableName = attrs.value( "name" ).toString();
    QString tableText;
    while( !((xml.name() == "table") && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
        //qDebug( "    token name=[%s], type=[%s]",
        //        xml.name().toUtf8().data(),
        //        xml.tokenString().toUtf8().data() );
        if( xml.tokenType() == QXmlStreamReader::Characters ) {
            QString text = xml.text().toString();
            tableText = text.trimmed();
        }
        xml.readNext();
    }
    skill.addTable( tableName, tableText );
}


void SkillsDB::parseSkillSetDefinition( L2Skill& skill, QXmlStreamReader& xml ) {
    QXmlStreamAttributes attrs = xml.attributes();
    QString setName = attrs.value( "name" ).toString();
    QString setValue = attrs.value( "val" ).toString();
    //qDebug( "  SkillsDB: parsing skill set [%s]=[%s]",
    //        setName.toUtf8().data(), setValue.toUtf8().data() );
    while( !((xml.name() == "set") && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
        //qDebug( "    token name=[%s], type=[%s]",
        //        xml.name().toUtf8().data(),
        //        xml.tokenString().toUtf8().data() );
        xml.readNext();
    }
    skill.addSet( setName, setValue );
}


void SkillsDB::parseSkillForDefinition( L2Skill& skill, QXmlStreamReader& xml ) {
    QString kind = xml.name().toString(); // save current XML block token
    //qDebug( "parsing 'for' kind = %s", kind.toUtf8().data() );
    // goto next
    xml.readNext();
    while( !((xml.name() == kind) && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
        //qDebug( "    token name=[%s], type=[%s]", xml.name().toUtf8().data(), xml.tokenString().toUtf8().data() );
        if( xml.tokenType() == QXmlStreamReader::StartElement ) {
            if( xml.name() == "effect" ) {
                //qDebug( "        entered parsing effect" );
                // <effect name="Buff" abnormalTime="300" val="0" abnormalLvl="1" abnormalType="possession">
                // parse effect attributes
                // known attributes:
                QString effectName;
                QString abnormalType;
                int abnormalTime = 0;
                int abnormalLvl = 0;
                int effectVal = 0;
                int self = 0;
                int noicon = 0;
                QString effectCount;
                QString effectPower;
                int abnormalVisualEffect = 0;
                QString chanceType;
                int triggeredId = 0;
                L2SkillEffect eff;
                //
                QXmlStreamAttributes attrs = xml.attributes();
                QVectorIterator<QXmlStreamAttribute> iter( attrs );
                while( iter.hasNext() ) {
                    QXmlStreamAttribute attr = iter.next();
                    if( attr.name() == "name" ) {
                        effectName = attr.value().toString();
                        eff.setEffectName( effectName );
                    } else if ( attr.name() == "abnormalType" ) {
                        abnormalType = attr.value().toString();
                        eff.setAbnormalType( abnormalType );
                    } else if( attr.name() == "val" ) {
                        effectVal = attr.value().toInt();
                        eff.setEffectVal( effectVal );
                    } else if( attr.name() == "abnormalTime" ) {
                        abnormalTime = attr.value().toInt();
                        eff.setAbnormalTime( abnormalTime );
                    } else if( attr.name() == "abnormalLvl" ) {
                        abnormalLvl = attr.value().toInt();
                        eff.setAbnormalLvl( abnormalLvl );
                    } else if( attr.name() == "self" ) {
                        self = attr.value().toInt();
                        eff.setSelf( self ? true : false );
                    } else if( attr.name() == "noicon" ) {
                        noicon = attr.value().toInt();
                        eff.setNoicon( noicon ? true : false );
                    } else if( attr.name() == "count" ) {
                        effectCount = attr.value().toString();
                        eff.setEffectCount( effectCount );
                    } else if( attr.name() == "effectPower" ) {
                        effectPower = attr.value().toString();
                        eff.setEffectPower( effectPower );
                    } else if( attr.name() == "abnormalVisualEffect" ) {
                        abnormalVisualEffect = attr.value().toInt();
                        eff.setAbnormalVisualEffect( abnormalVisualEffect );
                    } else if( attr.name() == "triggeredId" ) {
                        triggeredId = attr.value().toInt();
                        eff.setTriggeredId( triggeredId );
                    } else if( attr.name() == "triggeredLevel" ) {
                        eff.setTriggeredLevel( attr.value().toString() );
                    } else if( attr.name() == "chanceType" ) {
                        chanceType = attr.value().toString();
                        eff.setChanceType( chanceType );
                    } else if( attr.name() == "special" ) {
                        eff.setSpecial( attr.value().toString() );
                    } else if( attr.name() == "activationChance" ) {
                        eff.setActivationChance( attr.value().toString() );
                    } else if( attr.name() == "activationMinDamage" ) {
                        eff.setActivationMinDamage( attr.value().toString() );
                    } else if( attr.name() == "activationSkills" ) {
                        eff.setActivationSkills( attr.value().toString() );
                    } else if( attr.name() == "event" ) {
                        // rarely-used stuff, <skill id="22029" levels="1" name="Baguette Herb">
                    } else { // unknown attribute ?
                        throw UnparsedSkillAttributeException( skill.skillId(), attr.name().toString(), xml.lineNumber() );
                    }
                }
                // only after attributes, go further
                xml.readNext();
                // parse effect stat modifiers
                while( !((xml.name() == "effect") && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
                    if( xml.tokenType() == QXmlStreamReader::StartElement ) {
                        if( (xml.name() == "add") ||
                            (xml.name() == "sub") ||
                            (xml.name() == "mul") ||
                            (xml.name() == "div") ||
                            (xml.name() == "basemul") ||
                            (xml.name() == "set") ||
                            (xml.name() == "share") ) {
                            this->parseSkillEffectStatMod( skill, eff, xml );
                        } else {
                            throw UnparsedSkillForParameterException( skill.skillId(), xml );
                        }
                    }
                    xml.readNext();
                }
                // ended parsing effect
                //qDebug( "parsed effect: %s", eff.toString().toUtf8().data() );
                skill.addEffect( eff );
            } else if( (xml.name() == "add") ||
                       (xml.name() == "sub") ||
                       (xml.name() == "mul") ||
                       (xml.name() == "div") ||
                       (xml.name() == "basemul") ||
                       (xml.name() == "set") ||
                       (xml.name() == "share") ) {
                L2SkillEffect *pEffect = skill.defaultEffect();
                this->parseSkillEffectStatMod( skill, *pEffect, xml );
                //qDebug( "!!! DEFAULT effect: %s", pEffect->toString().toUtf8().data() );
            } else {
                xml.lineNumber();
                throw UnparsedSkillForParameterException( skill.skillId(), xml );
            }
        }
        xml.readNext();
    }
}


void SkillsDB::parseSkillEffectStatMod( L2Skill& skill, L2SkillEffect& eff, QXmlStreamReader& xml ) {
    L2SkillStatModifier mod;
    QString op_name = xml.name().toString(); // add / sub / mul / div ?
    //qDebug( "--parsing effect stat [%s]", op_name.toUtf8().data() );
    mod.setOpS( op_name );
    //
    // <add order="0x40" stat="pAtk" val="129.3" /> // direct value
    // <add order="0x40" stat="pAtk" val="#pAtk">   // table reference
    //     <using kind="Sword,Big Sword,Blunt,Big Blunt" /> // sub-tag
    // </add>
    //
    // parse attributes
    QVector<QXmlStreamAttribute> attrs = xml.attributes();
    QVectorIterator<QXmlStreamAttribute> iter( attrs );
    while( iter.hasNext() ) {
        QXmlStreamAttribute attr = iter.next();
        if( attr.name() == "order" ) {
            mod.setOrder( attr.value().toString() );
        } else if( attr.name() == "stat" ) {
            mod.setStat( attr.value().toString() );
        } else if( attr.name() == "val" ) {
            mod.setVal( attr.value().toString() );
        } else {
            throw UnparsedSkillAttributeException( skill.skillId(), attr.name().toString(), xml.lineNumber() );
        }
    }
    eff.addStatMod( mod );
    xml.readNext(); // goto next
    // parse any sub-tags
    bool in_not = false;
    while( !((xml.name() == op_name) && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
        if( xml.tokenType() == QXmlStreamReader::StartElement ) {
            if( xml.name() == "using" ) {
                QXmlStreamAttributes attrs = xml.attributes();
                QString ukind;
                if( attrs.hasAttribute("kind") ) {
                    ukind = attrs.value("kind").toString();
                    if( in_not )
                        ukind = QString( "NOT %1" ).arg(ukind);
                    eff.setUsingKind( ukind );
                }
            } else if( xml.name() == "not" ) {
                in_not = true;
            } else if( xml.name() == "player" ) {
                // ignore ? player status condition, like:
                // <skill id="148" levels="8" name="Vital Force"> ...
                //  ... <player resting="true" />
                // <player hp="60" /> (Frenzy)
            } else if( (xml.name() == "and") || (xml.name() == "or") ) {
                // ignore :( Frenzy
                // <and>
                //   <player hp="60" />
                //   <using kind="Big Sword,Big Blunt" />
                // </and>
            } else if( xml.name() == "game" ) {
                // game condition: Shadow Sense:
                //    <game night="true" />
            } else {
                throw UnparsedSkillForParameterException( skill.skillId(), xml );
            }
        }
        if( xml.tokenType() == QXmlStreamReader::EndElement ) {
            if( xml.name() == "not" ) {
                in_not = false;
            }
        }
        xml.readNext();
    }
}


void SkillsDB::parseSkillEnchantDefinition( L2Skill& skill, QXmlStreamReader& xml ) {
    Q_UNUSED(skill);
    QString kind = xml.name().toString();
    xml.readNext();
    while( !((xml.name() == kind) && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
        //qDebug( "    token name=[%s], type=[%s]",
        //        xml.name().toUtf8().data(),
        //        xml.tokenString().toUtf8().data() );
        xml.readNext();
    }
}


void SkillsDB::parseSkillCondDefinition( L2Skill& skill, QXmlStreamReader& xml ) {
    Q_UNUSED(skill);
    xml.readNext();
    while( !((xml.name() == "cond") && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
        //qDebug( "    token name=[%s], type=[%s]",
        //        xml.name().toUtf8().data(),
        //        xml.tokenString().toUtf8().data() );
        xml.readNext();
    }
}


// problems:
// 1) not float values (<skill id="21" levels="3" name="Poison Recovery">)
//   <table name="#negateAbnormals"> poison,3 poison,7 poison,9 </table>
// 2) not space-delimited, has line-breaks: (<skill id="323" levels="1" name="Quiver of Arrow: Grade A">)
//   <table name="#extractableItems">
//        1344,700,30;1344,1400,50;1344,2800,20
//   </table>

/*
<skill id="205" levels="45" name="Sword/Blunt Weapon Mastery" enchantGroup1="1" enchantGroup2="1" enchantGroup3="1">
    <!-- Confirmed CT2.5 and Updated to H5 -->
    <table name="#magicLvl"> 20 24 26 28 30 32 34 36 38 39 40 41 42 43 44 54 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 </table>
    <table name="#pAtk"> 4.5 7.3 8.9 10.7 12.8 15.1 17.7 20.5 23.7 25.4 27.1 29 30.9 32.9 35 37.1 39.4 41.7 44.1 46.6 49.2 51.9 54.6 57.5 60.4 63.3 66.4 69.5 72.7 76 79.3 82.7 86.1 89.6 93.1 96.6 100.2 103.8 107.5 111.1 114.8 118.4 122.1 125.7 129.3 </table>
    <table name="#triggeredId"> 0 0 0 0 0 0 0 0 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 5604 </table>
    <table name="#ench1runSpd"> 1 2 2 3 4 4 5 6 6 7 7 8 9 9 10 11 11 12 13 13 14 14 15 16 16 17 18 18 19 20 </table>
    <table name="#ench2cAtkAdd"> 65 81 96 112 127 143 158 174 189 205 220 236 251 267 282 298 313 329 344 360 375 391 406 422 437 453 468 484 499 515 </table>
    <table name="#ench3pAtkSpd"> 1 1 1 1 1 1 1 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 5 </table>
    <table name="#enchMagicLvl"> 76 76 76 77 77 77 78 78 78 79 79 79 80 80 80 81 81 81 82 82 82 83 83 83 84 84 84 85 85 85 </table>
    <set name="activationChance" val="50" /> <!-- TODO: Remove when support for effects using conditions is done -->
    <set name="chanceType" val="ON_HIT" /> <!-- TODO: Remove when support for effects using conditions is done -->
    <set name="magicLvl" val="#magicLvl" />
    <set name="operateType" val="P" />
    <set name="targetType" val="SELF" />
    <set name="triggeredId" val="#triggeredId" /> <!-- TODO: Remove when support for effects using conditions is done -->
    <enchant1 name="magicLvl" val="#enchMagicLvl" />
    <enchant2 name="magicLvl" val="#enchMagicLvl" />
    <enchant3 name="magicLvl" val="#enchMagicLvl" />
    <!-- TODO: Remove when support for effects using conditions is done -->
    <cond>
        <using kind="Sword,Big Sword,Blunt,Big Blunt" />
    </cond>
    <for>
        <add order="0x40" stat="pAtk" val="#pAtk">
            <using kind="Sword,Big Sword,Blunt,Big Blunt" />
        </add>
        <!-- TODO: Require support for effects using conditions -->
        <!-- <effect name="ChanceSkillTrigger" abnormalTime="-1" val="0" activationChance="50" chanceType="ON_HIT" triggeredId="#triggeredId"> -->
        <!-- <using kind="Sword,Big Sword,Blunt,Big Blunt" /> -->
        <!-- </effect> -->
    </for>
    <enchant1for>
        <add order="0x40" stat="pAtk" val="129.3">
            <using kind="Sword,Big Sword,Blunt,Big Blunt" />
        </add>
        <add order="0x40" stat="runSpd" val="#ench1runSpd">
            <using kind="Sword,Big Sword,Blunt,Big Blunt" />
        </add>
        <!-- TODO: Require support for effects using conditions -->
        <!-- <effect name="ChanceSkillTrigger" abnormalTime="-1" val="0" activationChance="50" chanceType="ON_HIT" triggeredId="5604"> -->
        <!-- <using kind="Sword,Big Sword,Blunt,Big Blunt" /> -->
        <!-- </effect> -->
    </enchant1for>
    <enchant2for>
        <add order="0x40" stat="pAtk" val="129.3">
            <using kind="Sword,Big Sword,Blunt,Big Blunt" />
        </add>
        <add order="0x40" stat="cAtkAdd" val="#ench2cAtkAdd">
            <using kind="Sword,Big Sword,Blunt,Big Blunt" />
        </add>
        <!-- TODO: Require support for effects using conditions -->
        <!-- <effect name="ChanceSkillTrigger" abnormalTime="-1" val="0" activationChance="50" chanceType="ON_HIT" triggeredId="5604"> -->
        <!-- <using kind="Sword,Big Sword,Blunt,Big Blunt" /> -->
        <!-- </effect> -->
    </enchant2for>
    <enchant3for>
        <add order="0x40" stat="pAtk" val="129.3">
            <using kind="Sword,Big Sword,Blunt,Big Blunt" />
        </add>
        <add order="0x40" stat="pAtkSpd" val="#ench3pAtkSpd">
            <using kind="Sword,Big Sword,Blunt,Big Blunt" />
        </add>
        <!-- TODO: Require support for effects using conditions -->
        <!-- <effect name="ChanceSkillTrigger" abnormalTime="-1" val="0" activationChance="50" chanceType="ON_HIT" triggeredId="5604"> -->
        <!-- <using kind="Sword,Big Sword,Blunt,Big Blunt" /> -->
        <!-- </effect> -->
    </enchant3for>
</skill>
*/


// ONLY mark skill in model that it is learned from forgottenScroll
int SkillsDB::loadForgottenSkillTree() {
    // notify
    if( m_splashScreenPointer ) {
        QSplashScreen *spl = (QSplashScreen *)m_splashScreenPointer;
        spl->showMessage( QString("Loading forgotten skill tree: "), Qt::AlignLeft|Qt::AlignBottom );
        qApp->processEvents();
    }
    // open file
    QFile f( "data/forgottenSkillTree.xml" );
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        qDebug( "SkillsDB: Failed to read XML: forgottenSkillTree.xml" );
        return 0;
    }
    int num_skills_read = 0;
    QXmlStreamReader xml( &f );
    while( !xml.atEnd() && !xml.hasError() ) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if( token == QXmlStreamReader::StartDocument )
            continue;
        if( token == QXmlStreamReader::StartElement ) {
            if( xml.name() == "skillTree" ) {
                //QXmlStreamAttributes attrs = xml.attributes();
                //QString treeType = attrs.value( "type" ).toString();
                //int     classId  = attrs.value( "classId" ).toInt();
                //qDebug( "FST: treeType=[%s], classId=%d", treeType.toUtf8().data(), classId );
                // loop sub-elements
                while( !((xml.name() == "skillTree") && (xml.tokenType() == QXmlStreamReader::EndElement)) ) {
                    //qDebug( "    token name=[%s], type=[%s]", xml.name().toUtf8().data(), xml.tokenString().toUtf8().data() );
                    if( (xml.name() == "skill") && (xml.tokenType() == QXmlStreamReader::StartElement) ) {
                        // here we ant only skillId
                        QXmlStreamAttributes skillAttrs = xml.attributes();
                        int skillId = skillAttrs.value( "skillId" ).toInt();
                        if( skillId > 0 ) {
                            if( _skills.contains(skillId) ) {
                                _skills[ skillId ].setForgottenScroll( true );
                                num_skills_read++;
                            }
                        }
                    }
                    xml.readNext();
                }
            }
        }
    }
    if( xml.hasError() ) {
        qCritical( "SkillsDB: FST XML read error: %s",
            xml.errorString().toUtf8().data() );
    }
    xml.clear();
    f.close();
    return num_skills_read;
}
