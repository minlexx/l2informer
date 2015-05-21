#ifndef SKILLINFOWND_H
#define SKILLINFOWND_H

#include <QWidget>
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
    L2Skill m_skill;

private:
    Ui::SkillInfoWnd *ui;
};


void Launch_SkillInfoWnd(const L2Skill& skill, QWidget *parent = 0);


#endif // SKILLINFOWND_H
