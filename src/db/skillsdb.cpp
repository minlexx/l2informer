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
    // current = V:/dev/l2/misc/SkillsInformer
    QDir skillsDir = QDir::current();
    skillsDir.cd( "data/skills" );
    // qDebug( "skills dir: %s", skillsDir.absolutePath().toUtf8().data() ); // OK
    // list all *.xml files in subdirectory data/skills
    QStringList nameFilters;
    nameFilters << "*.xml";
    QList<QFileInfo> fileList = skillsDir.entryInfoList(
        nameFilters, QDir::Files | QDir::Readable, QDir::Name );
    int num_xmls_done = 0;
    int num_fs_skills = 0;
    QListIterator<QFileInfo> iter( fileList );
    while( iter.hasNext() ) {
        const QFileInfo& fileInfo = iter.next();
        //qDebug( "file: %s", fileInfo.absoluteFilePath().toUtf8().data() );
        this->loadSkillXML( fileInfo.absoluteFilePath() );
        num_xmls_done++;
        //if( num_xmls_done >= 1 )
        //    break;
    }
    // load forgotten skill tree
    num_fs_skills = this->loadForgottenSkillTree();
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
    QXmlStreamAttributes attrs = xml.attributes();
    int skillId       = attrs.value( "id" ).toInt();
    int num_levels    = attrs.value( "levels" ).toInt();
    QString skillName = attrs.value( "name" ).toString();
    //qDebug( " SkillsDB: parsing skill %d (%s) levels=%d", skillId, skillName.toUtf8().data(), num_levels );
    xml.readNext();
    //
    L2Skill skill;
    skill.setSkillId( skillId );
    skill.setSkillName( skillName );
    skill.setNumLevels( num_levels );
    // read until the end of element named "skill"
    while( !((xml.tokenType() == QXmlStreamReader::EndElement) && (xml.name() == "skill")) ) {
        if( xml.name() == "table" ) {
            this->parseSkillTableDefinition( skill, xml );
        } else if( xml.name() == "set" ) {
            this->parseSkillSetDefinition( skill, xml );
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
