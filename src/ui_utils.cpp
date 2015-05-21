#include "models/l2skill.h"
#include "models/skilltreeentry.h"
#include "db/skilltree.h"
#include "db/charclassdb.h"
#include "ui_utils.h"

void treeViewSkills_init( QTreeWidget *tree, TVS_FLAGS flags ) {
    tree->setIconSize( QSize( 32, 32 ) );
    tree->setColumnCount( 3 );
    tree->setHeaderHidden( false );
    QStringList header_labels;
    header_labels << "Skill" << "Learn lvl" << "SP" << "skillId" << "type";
    if( flags & TVS_COLUMN_CLASS ) {
        header_labels << "Learned by class";
    }
    tree->setHeaderLabels( header_labels );
    tree->setColumnWidth( 0, 250 );
    tree->setColumnWidth( 1, 50 );
    tree->setColumnWidth( 2, 70 );
    tree->setColumnWidth( 3, 45 );
    tree->setColumnWidth( 4, 100 );
    if( flags & TVS_COLUMN_CLASS ) {
        tree->setColumnWidth( 5, 100 );
    }
}


void treeViewSkills_addSkill( QTreeWidget *tree, const L2Skill& skill, const SkillTreeEntry& learn_entry ) {
    //qDebug( "skill: %s [icon = %s]", e.toString().toUtf8().data(), sk.iconName().toUtf8().data() );
    QTreeWidgetItem *item = new QTreeWidgetItem();
    // header_labels << "Skill" << "Learn lvl" << "SP" << "skillId" << "misc";
    item->setText( 0, QString("%1 (lv %2)").arg( skill.skillName() ).arg( learn_entry.skillLvl() ) );
    item->setIcon( 0, QIcon( skill.icon() ) );
    item->setText( 1, QString("%1").arg( learn_entry.getLevel() ) );
    if( learn_entry.isLearnByNpc() ) {
        item->setText( 2, QString("%1").arg( learn_entry.sp_needed() ) );
    } else if( learn_entry.isLearnByFS() ) {
        item->setText( 2, "Forg.Scroll" );
    } else {
        item->setText( 2, "Auto learn" );
    }
    item->setText( 3, QString("%1").arg( skill.skillId() ) );
    QString sk_type;
    bool is_passive = false;
    if( skill.isPassive() ) {
        sk_type.append( "Passive" );
        is_passive = true;
    } else if( skill.isToggle() ) {
        sk_type.append( "Toggle" );
    } else if( skill.isMagic() ) {
        sk_type.append( "Magic" );
    } else {
        sk_type.append( "Physical" );
    }
    // passive skills don't have skillType property, all others have
    if( !is_passive ) {
        QString skillType = skill.getSkillType();
        if( !skillType.isEmpty() ) {
            if( !sk_type.isEmpty() ) sk_type.append( ", " );
            sk_type.append( skillType );
        }
    }
    item->setText( 4, sk_type );
    //
    // save skill ID as user data for item
    item->setData( 0, Qt::UserRole, QVariant( skill.skillId() ) );
    //
    tree->addTopLevelItem( item );
}


void treeViewSkills_addSkill_noEntry( QTreeWidget *tree, const L2Skill& skill ) {
    // try to find skill learn entry...
    QList<SkillTreeEntry> skill_learn = SkillTree::getInstance()->getSkillLearnTree( skill.skillId() );
    QString learn_info;
    if( skill_learn.size() > 0 ) {
        CharClassDB *cdb = CharClassDB::getInstance();
        QListIterator<SkillTreeEntry> iter( skill_learn );
        while( iter.hasNext() ) {
            const SkillTreeEntry &entry = iter.next();
            int classId = entry.classId();
            L2CharClass cl = cdb->getClassById( classId );
            QString add = QString( "lvl %1 by %2 at %3" )
                    .arg( entry.skillLvl() )
                    .arg( cl.name() )
                    .arg( entry.getLevel() );
            if( !learn_info.isEmpty() )
                learn_info.append( ", " );
            learn_info.append( add );
        }
    }
    // construct widget item
    QTreeWidgetItem *item = new QTreeWidgetItem();
    // header_labels << "Skill" << "Learn lvl" << "SP" << "skillId" << "misc";
    item->setText( 0, skill.skillName() );
    item->setIcon( 0, QIcon( skill.icon() ) );
    item->setText( 1, QString("unk") );
    item->setText( 2, "-" );
    item->setText( 3, QString("%1").arg( skill.skillId() ) );
    // column 4
    QString sk_type;
    bool is_passive = false;
    if( skill.isPassive() ) {
        sk_type.append( "Passive" );
        is_passive = true;
    } else if( skill.isToggle() ) {
        sk_type.append( "Toggle" );
    } else if( skill.isMagic() ) {
        sk_type.append( "Magic" );
    } else {
        sk_type.append( "Physical" );
    }
    // passive skills don't have skillType property, all others have
    if( !is_passive ) {
        QString skillType = skill.getSkillType();
        if( !skillType.isEmpty() ) {
            if( !sk_type.isEmpty() ) sk_type.append( ", " );
            sk_type.append( skillType );
        }
    }
    item->setText( 4, sk_type );
    item->setText( 5, learn_info );
    // save skill ID as user data for item
    item->setData( 0, Qt::UserRole, QVariant( skill.skillId() ) );
    //
    tree->addTopLevelItem( item );
}
