#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <QTreeWidget>
#include <QTreeWidgetItem>


class L2Skill;
class SkillTreeEntry;

enum TVS_FLAGS {
    TVS_DEFAULT = 0,
    TVS_COLUMN_CLASS = 1
};

void treeViewSkills_init( QTreeWidget *tree, TVS_FLAGS flags = TVS_DEFAULT );
void treeViewSkills_addSkill( QTreeWidget *tree, const L2Skill& skill, const SkillTreeEntry& learn_entry );
void treeViewSkills_addSkill_noEntry( QTreeWidget *tree, const L2Skill& skill );

#endif // UI_UTILS_H
