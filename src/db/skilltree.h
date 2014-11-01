#ifndef SKILLTREE_H
#define SKILLTREE_H

#include "../models/skilltreeentry.h"
#include <QMap>
#include <QList>
#include <QXmlStreamReader>


class SkillTree {
public:
    static SkillTree *getInstance();

protected:
    static SkillTree *_s_instance;
    SkillTree();

public:
    bool load();

public:
    QList<SkillTreeEntry> getClassSkills( int classId );
    QList<SkillTreeEntry> getClassSkillsForLevel( int classId, int minLvl, int maxLvl );

    QList<SkillTreeEntry> getSkillLearnTree( int skillId );

protected:
    int  parseClassSkillTree( QXmlStreamReader &xml, int classId, int parentClassId );
    int  loadForgottenScrollSkills();

protected:
    QMap<int, QList<SkillTreeEntry> > _classId_skills;
    QMap<int, QList<SkillTreeEntry> > _skillId_skills;
};

#endif // SKILLTREE_H
