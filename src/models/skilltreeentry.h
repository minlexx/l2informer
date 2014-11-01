#ifndef SKILLTREEENTRY_H
#define SKILLTREEENTRY_H

#include <QString>

class SkillTreeEntry {
public:
    SkillTreeEntry();
    SkillTreeEntry( const QString& skillName, int classId,
        int parentClassId, int skillId, int skillLvl, int getLevel,
        int sp_needed,
        bool autoGet=false,
        bool learnByNpc=false,
        bool learnByFS=false );
    SkillTreeEntry( const SkillTreeEntry& other );
    const SkillTreeEntry& operator=( const SkillTreeEntry& other );
    bool operator==( const SkillTreeEntry& other );

public:
    QString skillName() const { return _skillName; }
    int classId() const { return _classId; }
    int parentClassId() const { return _parentClassId; }
    int skillId() const { return _skillId; }
    int skillLvl() const { return _skillLvl; }
    int getLevel() const { return _getLevel; }
    bool isAutoGet() const { return _autoGet; }
    bool isLearnByNpc() const { return _learnByNpc; }
    int  sp_needed() const { return _spNeeded; }
    bool isLearnByFS() const { return _learnByFS; }

    QString toString() const;

protected:
    QString _skillName;
    int _classId;
    int _parentClassId;
    int _skillId;
    int _skillLvl;
    int _getLevel;
    bool _autoGet;
    bool _learnByNpc;
    int  _spNeeded;
    bool _learnByFS;
};

#endif // SKILLTREEENTRY_H
