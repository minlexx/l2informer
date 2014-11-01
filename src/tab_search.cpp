#include "tab_search.h"
#include "ui_tab_search.h"
#include "ui_utils.h"
#include "db/skillsdb.h"

Tab_Search::Tab_Search( QWidget *parent ): QWidget( parent ), ui(new Ui::Tab_Search) {
    ui->setupUi(this);
}

Tab_Search::~Tab_Search() {
    delete ui;
}

void Tab_Search::initView() {
    treeViewSkills_init( this->ui->tree_skills, TVS_COLUMN_CLASS );
    connectSignals();
}

void Tab_Search::connectSignals() {
    QObject::connect( this->ui->btn_search, SIGNAL(clicked()), this, SLOT(onSearchBtnClicked()) );
    QObject::connect( this->ui->le_skillName, SIGNAL(textEdited(QString)), this, SLOT(onSearchTextEdited(QString)) );
}

void Tab_Search::onSearchBtnClicked() {
    QString text = this->ui->le_skillName->text();
    this->performSearch( text );
}

void Tab_Search::onSearchTextEdited( QString text ) {
    Q_UNUSED(text);
}


void Tab_Search::performSearch( QString text ) {
    QList<L2Skill> res = SkillsDB::getInstance()->findSkill( text );
    this->ui->tree_skills->clear();
    if( res.size() > 0 ) {
        QListIterator<L2Skill> iter( res );
        while( iter.hasNext() ) {
            const L2Skill &skill = iter.next();
            treeViewSkills_addSkill_noEntry( this->ui->tree_skills, skill );
        }
    }
}
