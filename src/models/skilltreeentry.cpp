#include "skilltreeentry.h"

SkillTreeEntry::SkillTreeEntry() {
    _skillName.clear();
    _classId = _parentClassId = _skillId = _skillLvl = -1;
    _getLevel = 0;
    _autoGet = _learnByNpc = _learnByFS = false;
    _spNeeded = 0;
}


SkillTreeEntry::SkillTreeEntry( const QString& skillName, int classId,
    int parentClassId, int skillId, int skillLvl, int getLevel,
    int sp_needed,
    bool autoGet,
    bool learnByNpc,
    bool learnByFS ) {
    _skillName = skillName;
    _classId = classId;
    _parentClassId = parentClassId;
    _skillId = skillId;
    _skillLvl = skillLvl;
    _getLevel = getLevel;
    _autoGet = autoGet;
    _learnByNpc = learnByNpc;
    _spNeeded = sp_needed;
    _learnByFS = learnByFS;
}


SkillTreeEntry::SkillTreeEntry( const SkillTreeEntry& other ) {
    (*this) = other;
}


const SkillTreeEntry& SkillTreeEntry::operator=( const SkillTreeEntry& other ) {
    if( this == &other ) return *this;
    _skillName  = other._skillName;
    _classId    = other._classId;
    _parentClassId = other._parentClassId;
    _skillId    = other._skillId;
    _skillLvl   = other._skillLvl;
    _getLevel   = other._getLevel;
    _autoGet    = other._autoGet;
    _learnByNpc = other._learnByNpc;
    _spNeeded   = other._spNeeded;
    _learnByFS  = other._learnByFS;
    return *this;
}


bool SkillTreeEntry::operator==( const SkillTreeEntry& other ) {
    if( (_classId == other._classId) &&
        (_skillId == other._skillId) &&
        (_skillLvl == other._skillLvl) &&
        (_getLevel == other._getLevel) )
        return true;
    return false;
}


QString SkillTreeEntry::toString() const {
    QString ret = QString("%1 (lv %2) [ID=%3]").arg(_skillName).arg(_skillLvl).arg(_skillId);
    return ret;
}
