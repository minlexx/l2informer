#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QMenu>
#include <QClipboard>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include "tab_classes.h"
#include "ui_tab_classes.h"
#include "ui_utils.h"

#include "db/charclassdb.h"
#include "db/skilltree.h"
#include "db/skillsdb.h"


Tab_Classes::Tab_Classes( QWidget *parent ): QWidget(parent), ui(new Ui::Tab_Classes) {
    ui->setupUi( this );
    m_hidePassiveSkills = false;
    m_skillNameFilter.clear();
    m_filterTimer = new QTimer( this );
    m_filterTimer->setSingleShot( true );
    this->ui->chk_hidePassive->setChecked( false );
}


Tab_Classes::~Tab_Classes() {
    delete ui;
}


void Tab_Classes::initView() {
    //
    QList<int> sizes = this->ui->splitter->sizes();
    sizes.replace( 0, 300 );
    sizes.replace( 1, 700 );
    this->ui->splitter->setSizes( sizes );
    //
    CharClassDB *classDb = CharClassDB::getInstance();
    QList<L2CharClass> bases = classDb->getBaseClassesList();
    QListIterator<L2CharClass> iter( bases );
    while( iter.hasNext() ) {
        const L2CharClass& cl = iter.next();
        //
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText( 0, cl.name() );
        item->setData( 0, Qt::UserRole, QVariant(cl.classId()) );
        //
        this->ui->tree_classes->addTopLevelItem( item );
        //
        populateChildClasses( item, cl.classId() );
    }
    //
    treeViewSkills_init( this->ui->tree_skills );
    //
    connectSignals();
}


void Tab_Classes::connectSignals() {
    QObject::connect( this->ui->tree_classes, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                      this, SLOT(onTree_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)) );
    QObject::connect( this->ui->cb_minLvl, SIGNAL(activated(int)), this, SLOT(onCbMinLvlActivated(int)) );
    QObject::connect( this->ui->cb_maxLvl, SIGNAL(activated(int)), this, SLOT(onCbMaxLvlActivated(int)) );
    QObject::connect( this->ui->chk_hidePassive, SIGNAL(clicked()), this, SLOT(onChk_hidePassive()) );
    QObject::connect( this->ui->le_skillNameFilter, SIGNAL(textEdited(QString)),
                      this, SLOT(onSkillNameFilterChanged(QString)) );
    QObject::connect( this->m_filterTimer, SIGNAL(timeout()), this, SLOT(onFilterTimer()) );
    // skills tree connections
    //QObject::connect( this->ui->tree_skills, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
    //                  this, SLOT(onTreeSkill_itemClicked(QTreeWidgetItem*,int)) );
    QObject::connect( this->ui->tree_skills, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
                      this, SLOT(onTreeSkill_itemPressed(QTreeWidgetItem*,int)) );
}


void Tab_Classes::populateChildClasses( QTreeWidgetItem *item, int classId ) {
    CharClassDB *classDb = CharClassDB::getInstance();
    QList<L2CharClass> subs = classDb->getChildClassesList( classId );
    QListIterator<L2CharClass> iter( subs );
    while( iter.hasNext() ) {
        const L2CharClass& cl = iter.next();
        //
        QTreeWidgetItem *item2 = new QTreeWidgetItem();
        item2->setText( 0, cl.name() );
        item2->setData( 0, Qt::UserRole, QVariant(cl.classId()) );
        //
        item->addChild( item2 );
        //
        populateChildClasses( item2, cl.classId() );
    }
}


void Tab_Classes::fillSuggestlevelsCombo( int level_min, int level_max ) {
    if( level_min > level_max ) level_min = level_max;
    if( level_max < level_min ) level_max = level_min;
    //
    int i;
    this->ui->cb_minLvl->clear();
    this->ui->cb_maxLvl->clear();
    for( i=level_min; i<=level_max; i++ ) {
        QString itemText = QString( "%1" ).arg( i );
        this->ui->cb_minLvl->addItem( itemText, QVariant(i) );
        this->ui->cb_maxLvl->addItem( itemText, QVariant(i) );
    }
    this->ui->cb_minLvl->setCurrentIndex( 0 );
    this->ui->cb_maxLvl->setCurrentIndex( level_max-level_min );
}


void Tab_Classes::onCbMinLvlActivated( int index ) {
    int min_lvl = this->ui->cb_minLvl->itemData( index, Qt::UserRole ).toInt();
    int max_lvl = this->ui->cb_maxLvl->currentData( Qt::UserRole ).toInt();
    // fix upper bound
    if( min_lvl > max_lvl ) {
        max_lvl = min_lvl;
        this->ui->cb_maxLvl->setCurrentIndex( index );
    }
    int classId = this->getCurrentSelectedTreeClassId();
    this->fillClassSkillsForLevel( classId, min_lvl, max_lvl );
}


void Tab_Classes::onCbMaxLvlActivated( int index ) {
    int max_lvl = this->ui->cb_maxLvl->itemData( index, Qt::UserRole ).toInt();
    int min_lvl = this->ui->cb_minLvl->currentData( Qt::UserRole ).toInt();
    // fix lower bound
    if( max_lvl < min_lvl ) {
        min_lvl = max_lvl;
        this->ui->cb_minLvl->setCurrentIndex( index );
    }
    int classId = this->getCurrentSelectedTreeClassId();
    this->fillClassSkillsForLevel( classId, min_lvl, max_lvl );
}


void Tab_Classes::onChk_hidePassive() {
    if( this->ui->chk_hidePassive->isChecked() )
        this->m_hidePassiveSkills = true;
    else
        this->m_hidePassiveSkills = false;
    // update (refill) skills
    int min_lvl = this->ui->cb_minLvl->currentData( Qt::UserRole ).toInt();
    int max_lvl = this->ui->cb_maxLvl->currentData( Qt::UserRole ).toInt();
    int classId = this->getCurrentSelectedTreeClassId();
    this->fillClassSkillsForLevel( classId, min_lvl, max_lvl );
}


void Tab_Classes::onSkillNameFilterChanged( QString text ) {
    m_skillNameFilter = text;
    m_filterTimer->start( 1000 );
}


void Tab_Classes::onFilterTimer() {
    if( m_skillNameFilter.size() > 3 ) {
        int min_lvl = this->ui->cb_minLvl->currentData( Qt::UserRole ).toInt();
        int max_lvl = this->ui->cb_maxLvl->currentData( Qt::UserRole ).toInt();
        int classId = this->getCurrentSelectedTreeClassId();
        // qDebug( "timer: filter changed: [%s] lv [%d..%d] class [%d]",
        //     m_skillNameFilter.toUtf8().data(), min_lvl, max_lvl, classId );
        this->fillClassSkillsForLevel( classId, min_lvl, max_lvl );
    }
}


int Tab_Classes::getCurrentSelectedTreeClassId() {
    QTreeWidgetItem *curItem = this->ui->tree_classes->currentItem();
    if( !curItem ) return -1; // WTF
    bool ok = false;
    int classId = curItem->data( 0, Qt::UserRole ).toInt( &ok );
    if( !ok ) return -1;
    return classId;
}


void Tab_Classes::onTree_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous ) {
    Q_UNUSED( previous );
    int classId = current->data( 0, Qt::UserRole ).toInt();
    L2CharClass cl = CharClassDB::getInstance()->getClassById( classId );
    QList<SkillTreeEntry> class_skills = SkillTree::getInstance()->getClassSkills( classId );
    //qDebug( "Tab_Classes: new current class: [%s] (%d)",
    //        current->text(0).toUtf8().data(), classId );
    this->ui->le_classId->setText( QString("%1").arg(classId) );
    this->ui->le_className->setText( current->text(0) );
    this->ui->le_classId_parent->setText( QString("%1").arg(cl.parentClassId()) );
    // find min-max learn levels for skills
    QListIterator<SkillTreeEntry> iter( class_skills );
    int levels_min = 999, levels_max = 0;
    while( iter.hasNext() ) {
        const SkillTreeEntry& e = iter.next();
        if( e.getLevel() < levels_min ) levels_min = e.getLevel();
        if( e.getLevel() > levels_max ) levels_max = e.getLevel();
    }
    //
    //qDebug( "found suggested levels: [%d..%d]", levels_min, levels_max );
    this->fillSuggestlevelsCombo( levels_min, levels_max );
    this->fillClassSkillsForLevel( classId, levels_min, levels_max );
}


void Tab_Classes::fillClassSkillsForLevel( int classId, int lv_min, int lv_max ) {
    if( classId == -1 ) return;
    this->ui->tree_skills->clear();
    SkillsDB *skills_db = SkillsDB::getInstance();
    QList<SkillTreeEntry> class_skills = SkillTree::getInstance()->getClassSkillsForLevel( classId, lv_min, lv_max );
    QListIterator<SkillTreeEntry> iter( class_skills );
    while( iter.hasNext() ) {
        const SkillTreeEntry& entry = iter.next();
        const L2Skill& skill = skills_db->getSkill( entry.skillId() );
        // default - add skill
        bool add_skill = true;
        // hide passive?
        if( this->m_hidePassiveSkills ) {
            if( skill.isPassive() )
                add_skill = false; // do not add passive skills
        }
        // name filter
        if( !m_skillNameFilter.isEmpty() && (m_skillNameFilter.size() > 3) ) {
            if( !skill.skillName().contains(m_skillNameFilter, Qt::CaseInsensitive) )
                add_skill = false; // no filter substring in skill name
        }
        if( add_skill )
            treeViewSkills_addSkill( this->ui->tree_skills, skill, entry );
    }
}


void Tab_Classes::onTreeSkill_itemPressed( QTreeWidgetItem *item, int column ) {
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
                qDebug( "Launch skill info window for skill: %s", skill.toString().toUtf8().data() );
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


void Tab_Classes::onActionSkillInfo( bool checked ) {
    Q_UNUSED(checked);
    qDebug( "onActionSkillInfo()" );
}
