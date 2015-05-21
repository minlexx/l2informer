#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QMenu>
#include <QClipboard>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include "tab_search.h"
#include "ui_tab_search.h"
#include "ui_utils.h"
#include "db/skillsdb.h"
#include "skillinfownd.h"

Tab_Search::Tab_Search( QWidget *parent ): QWidget( parent ), ui(new Ui::Tab_Search) {
    m_as_timer = new QTimer(this);
    m_as_timer->setSingleShot(true);
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
    QObject::connect( this->ui->le_skillName, SIGNAL(returnPressed()), this, SLOT(onSearchBtnClicked()) );
    QObject::connect( this->m_as_timer, SIGNAL(timeout()), this, SLOT(onTimerAutoSearch()) );
    QObject::connect( this->ui->tree_skills, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
                      this, SLOT(onTreeSkill_itemPressed(QTreeWidgetItem*,int)) );
}

void Tab_Search::onSearchBtnClicked() {
    QString text = this->ui->le_skillName->text();
    this->performSearch( text );
}

void Tab_Search::onSearchTextEdited( QString text ) {
    Q_UNUSED(text);
    this->m_as_timer->start( 1000 );
}

void Tab_Search::onTimerAutoSearch() {
    QString text = this->ui->le_skillName->text();
    int tlen = text.size();
    if( tlen > 3 ) {
        this->onSearchBtnClicked();
    }
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


void Tab_Search::onTreeSkill_itemPressed( QTreeWidgetItem *item, int column ) {
    Q_UNUSED(column);
    // get skill ID from item data
    bool ok = false;
    int skillId = item->data( 0, Qt::UserRole ).toInt( &ok );
    if( !ok ) return;
    if( skillId < 1 ) return;
    L2Skill skill = SkillsDB::getInstance()->getSkill( skillId );
    if( !skill.isValid() ) return;
    //
    Qt::MouseButtons btns = QGuiApplication::mouseButtons();
    if( btns & Qt::RightButton ) {
        QMenu *menu = new QMenu( this->ui->tree_skills );
        QAction *a1 = menu->addAction( "Show skill details..." );
        QAction *a2 = menu->addAction( "Copy skill name and ID to clipboard" );
        QAction *a3 = menu->addAction( "Open XML with skill definition" );
        QAction *ret =menu->exec( QCursor::pos() );
        if( ret ) {
            if( ret == a1 ) {
                // launch skillinfo window
                Launch_SkillInfoWnd( skill );
            } else if ( ret == a2 ) {
                QString skillInfo = QString( "%1 ID = %2" ).arg( skill.skillName() ).arg( skillId );
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText( skillInfo );
            } else if ( ret == a3 ) {
                int s100 = skillId / 100;
                char ss[512];
                sprintf( ss, "%03d00-%03d99", s100, s100 );
                QString xmlName = QString::fromUtf8( ss );
                //QString cwd = QCoreApplication::applicationDirPath();
                QString cwd = QDir::current().absolutePath();
                QString xmlFullName = QString( "file:///%1/data/skills/%2.xml" ).arg( cwd ).arg( xmlName );
                qDebug( "Launching: %s", xmlFullName.toUtf8().data() );
                QDesktopServices::openUrl( QUrl(xmlFullName) );
            }
        }
        delete menu;
    }
}
