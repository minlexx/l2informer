#include <QFile>
#include <QVariant>
#include "skilltree.h"
#include "charclassdb.h"


SkillTree *SkillTree::_s_instance = NULL;


SkillTree *SkillTree::getInstance() {
    if( !_s_instance ) {
        _s_instance = new SkillTree();
    }
    return _s_instance;
}


SkillTree::SkillTree() {
    _classId_skills.clear();
    _skillId_skills.clear();
}


bool SkillTree::load() {
    QFile f( "data/classSkillTree.xml" );
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        qDebug( "Failed to read skill tree: data/classSkillTree.xml" );
        return false;
    }
    QXmlStreamReader xml( &f );
    while( !xml.atEnd() && !xml.hasError() ) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if( token == QXmlStreamReader::StartDocument )
            continue;
        if( token == QXmlStreamReader::StartElement ) {
            if( xml.name() == "list" )
                continue;
            // <skillTree type="classSkillTree" classId="1" parentClassId="0">
            if( xml.name() == "skillTree" ) {
                int classId = -1;
                int parentClassId = -1;
                QXmlStreamAttributes attrs = xml.attributes();
                bool ok;
                if( attrs.hasAttribute("classId") )
                    classId = attrs.value("classId").toInt( &ok );
                if( attrs.hasAttribute("parentClassId") )
                    parentClassId = attrs.value("parentClassId").toInt();
                if( (classId >= 0) && ok )
                    parseClassSkillTree( xml, classId, parentClassId );
            }
        }
    }
    if( xml.hasError() ) {
        qCritical( "XML read error: (data/classSkillTree.xml) %s", xml.errorString().toLocal8Bit().data() );
    } else {
        qDebug( "SkillTree: loaded %d skill learn trees for %d classes",
            _skillId_skills.size(), _classId_skills.size() );
    }
    xml.clear();
    f.close();

    int num_fs_skills = this->loadForgottenScrollSkills();
    qDebug( "SkillTree: loaded %d forgotten scroll skills", num_fs_skills );

    return true;
}


int SkillTree::loadForgottenScrollSkills() {
    // open file
    QFile f( "data/forgottenSkillTree.xml" );
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        qDebug( "SkillTree: Failed to read XML: data/forgottenSkillTree.xml" );
        return 0;
    }
    int num_loaded = 0;
    QXmlStreamReader xml( &f );
    while( !xml.atEnd() && !xml.hasError() ) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if( token == QXmlStreamReader::StartDocument )
            continue;
        if( token == QXmlStreamReader::StartElement ) {
            if( xml.name() == "skillTree" ) {
                int classId = -1;
                int parentClassId = -1;
                bool ok = false;
                QXmlStreamAttributes attrs = xml.attributes();
                if( attrs.hasAttribute("classId") )
                    classId = attrs.value( "classId" ).toInt( &ok );
                if( attrs.hasAttribute("parentClassId") )
                    parentClassId = attrs.value( "parentClassId" ).toInt();
                // forgotten skills tree XML does not have a parent class ID set there,
                // try to find it out from CharClassDB
                if( parentClassId == -1 ) {
                    L2CharClass charClass = CharClassDB::getInstance()->getClassById( classId );
                    if( charClass.isValid() ) {
                        parentClassId = charClass.parentClassId();
                        //qDebug( "SkillTree::loadForgottenScrollSkills(): got parent class ID for class %d: %d",
                        //    classId, parentClassId );
                    }
                }
                //qDebug( "FST: forgotten skills for classId=%d", classId );
                if( (classId > 0) && ok )
                    num_loaded += this->parseClassSkillTree( xml, classId, parentClassId );
            }
        }
    }
    if( xml.hasError() ) {
        qCritical( "SkillTree: FST XML read error: %s",
            xml.errorString().toUtf8().data() );
    }
    xml.clear();
    f.close();
    return num_loaded;
}


int SkillTree::parseClassSkillTree( QXmlStreamReader &xml, int classId, int parentClassId ) {
    // <skill skillName="Lucky" skillId="194" skillLvl="1" getLevel="1" autoGet="true" />
    // <skill skillName="Power Shot" skillId="56" skillLvl="2" getLevel="5" levelUpSp="50" learnedByNpc="true" />
    // <skill skillName="Aura Bird - Falcon" skillId="841" skillLvl="1" getLevel="75" learnedByFS="true" />
    xml.readNext();
    int num_loaded = 0;
    // read until the end of element named "skillTree"
    while( !((xml.tokenType() == QXmlStreamReader::EndElement) && (xml.name() == "skillTree")) ) {
        if( xml.name() == "skill" ) {
            QXmlStreamAttributes attrs = xml.attributes();
            QString skillName;
            int skillId = -1;
            int skillLvl = 0;
            int getLevel = 0;
            int levelUpSp = 0;
            bool autoGet = false;
            bool learnedByNpc = false;
            bool learnedByFS = false;
            if( attrs.hasAttribute("skillName") )
                skillName = attrs.value("skillName").toString();
            if( attrs.hasAttribute("skillId") )
                skillId = attrs.value("skillId").toInt();
            if( attrs.hasAttribute("skillLvl") )
                skillLvl = attrs.value("skillLvl").toInt();
            if( attrs.hasAttribute("getLevel") )
                getLevel = attrs.value("getLevel").toInt();
            if( attrs.hasAttribute("levelUpSp") )
                levelUpSp = attrs.value("levelUpSp").toInt();
            if( attrs.hasAttribute("autoGet") )
                autoGet = QVariant( attrs.value("autoGet").toString() ).toBool();
            if( attrs.hasAttribute("learnedByNpc") )
                learnedByNpc = QVariant( attrs.value("learnedByNpc").toString() ).toBool();
            if( attrs.hasAttribute("learnedByFS") )
                learnedByFS = QVariant( attrs.value("learnedByFS").toString() ).toBool();
            //
            if( skillId > -1 ) {
                num_loaded++;
                SkillTreeEntry entry( skillName, classId, parentClassId, skillId,
                    skillLvl, getLevel, levelUpSp, autoGet, learnedByNpc, learnedByFS );
                // add to class skill tree
                if( !this->_classId_skills.contains( classId ) ) { // first time
                    QList<SkillTreeEntry> list;
                    list.append( entry );
                    this->_classId_skills.insert( classId, list );
                } else { // already exists
                    QList<SkillTreeEntry> list_replacer;
                    list_replacer.append( this->_classId_skills.value( classId ) );
                    list_replacer.append( entry );
                    this->_classId_skills.insert( classId, list_replacer );
                }
                // add to skill learn tree
                if( !this->_skillId_skills.contains( skillId ) ) { // first time
                    QList<SkillTreeEntry> list;
                    list.append( entry );
                    this->_skillId_skills.insert( skillId, list );
                } else { // already exists
                    QList<SkillTreeEntry> list_replacer;
                    list_replacer.append( this->_skillId_skills.value( skillId ) );
                    list_replacer.append( entry );
                    this->_skillId_skills.insert( skillId, list_replacer );
                }
            }
        }
        xml.readNext();
    }
    return num_loaded;
}


QList<SkillTreeEntry> SkillTree::getClassSkills( int classId ) {
    QList<SkillTreeEntry> ret;
    if( this->_classId_skills.contains( classId ) ) {
        ret = this->_classId_skills.value( classId );
    }
    return ret;
}


QList<SkillTreeEntry> SkillTree::getClassSkillsForLevel( int classId, int minLvl, int maxLvl ) {
    QList<SkillTreeEntry> ret;
    QList<SkillTreeEntry> classSkills = getClassSkills( classId );
    QListIterator<SkillTreeEntry> iter( classSkills );
    while( iter.hasNext() ) {
        const SkillTreeEntry& entry = iter.next();
        int learnLvl = entry.getLevel();
        if( (minLvl <= learnLvl) && (learnLvl <= maxLvl) )
            ret.append( entry );
    }
    return ret;
}


QList<SkillTreeEntry> SkillTree::getSkillLearnTree( int skillId ) {
    QList<SkillTreeEntry> ret;
    if( this->_skillId_skills.contains( skillId ) ) {
        ret = this->_skillId_skills.value( skillId );
    }
    return ret;
}
