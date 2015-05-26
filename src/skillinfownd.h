#ifndef SKILLINFOWND_H
#define SKILLINFOWND_H

#include <QList>
#include <QWidget>
#include "skillinfopane.h"
#include "models/l2skill.h"

namespace Ui {
class SkillInfoWnd;
}

class SkillInfoWnd : public QWidget {
    Q_OBJECT

public:
    explicit SkillInfoWnd(const L2Skill& skill, QWidget *parent = 0);
    ~SkillInfoWnd();

protected:
    void fillTopInfo();
    void fillLevelPanels();
    void add_skillXmlTab();

protected:
    L2Skill m_skill;
    QList<SkillInfoPane *> m_panels;

private:
    Ui::SkillInfoWnd *ui;
};


void Launch_SkillInfoWnd(const L2Skill& skill, QWidget *parent = 0);


#endif // SKILLINFOWND_H
