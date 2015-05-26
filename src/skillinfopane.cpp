#include "skillinfopane.h"
#include "ui_skillinfopane.h"

SkillInfoPane::SkillInfoPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkillInfoPane)
{
    ui->setupUi(this);
}

SkillInfoPane::~SkillInfoPane()
{
    delete ui;
}
