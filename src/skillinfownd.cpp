#include "skillinfownd.h"
#include "ui_skillinfownd.h"


void Launch_SkillInfoWnd(const L2Skill& skill, QWidget *parent) {
    SkillInfoWnd *wnd = new SkillInfoWnd(skill, parent);
    wnd->show();
}


SkillInfoWnd::SkillInfoWnd(const L2Skill& skill, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkillInfoWnd)
{
    m_skill = skill;
    ui->setupUi(this);
    this->setWindowTitle( m_skill.skillName() );
    QString lbl = QString("%1 (levels: %2, enchants: %3)")
            .arg(m_skill.skillName())
            .arg(m_skill.numLevels())
            .arg(m_skill.numEnchants());
    this->ui->lbl_skillName->setText(lbl);
    QPixmap skill_ico = m_skill.icon();
    this->ui->lbl_icon->setPixmap(skill_ico);
}


SkillInfoWnd::~SkillInfoWnd() {
    delete ui;
}
