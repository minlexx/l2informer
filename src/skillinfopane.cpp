#include "skillinfopane.h"
#include "ui_skillinfopane.h"
#include "db/skillsdb.h"

SkillInfoPane::SkillInfoPane(QWidget *parent): QWidget(parent), ui(new Ui::SkillInfoPane) {
    ui->setupUi(this);
    this->initTree();
}

SkillInfoPane::~SkillInfoPane() {
    delete ui;
}


void SkillInfoPane::initTree() {
    //this->ui->tree->setIconSize( QSize( 32, 32 ) );
    this->ui->tree->setColumnCount( 2 );
    this->ui->tree->setHeaderHidden( false );
    QStringList header_labels;
    header_labels << "set_name" << "set_value";
    this->ui->tree->setHeaderLabels( header_labels );
    this->ui->tree->setColumnWidth( 0, 150 );
    this->ui->tree->setColumnWidth( 1, 300 );
}


void SkillInfoPane::setSkillAndLevel( const L2Skill& skill, int level ) {
    m_skill = skill;
    m_level = level;
    // populate tree with sets
    QMap<QString, L2SkillSet> *skill_sets = m_skill.getAllSets();
    QMapIterator<QString, L2SkillSet> iter( *skill_sets );
    while( iter.hasNext() ) {
        iter.next();
        const L2SkillSet& skill_set = iter.value();
        this->addSetToTree( skill_set );
    }
    // effects
    // "default" effect
    L2SkillEffect *d_eff = m_skill.defaultEffect();
    if( d_eff->isValid() ) {
        this->addEffectToTree( *d_eff, "def.effect" );
    }
    //} else {
    //    qWarning( "SkillInfoPane: skill [%s] has no default effect?",
    //              this->m_skill.toString().toUtf8().data() );
    //}
    // all effects
    QList<L2SkillEffect> *all_effects = m_skill.getAllEffects();
    QListIterator<L2SkillEffect> eiter(*all_effects);
    while( eiter.hasNext() ) {
        L2SkillEffect eff = eiter.next();
        this->addEffectToTree( eff );
    }
}


void SkillInfoPane::addSetToTree( const L2SkillSet& sset ) {
    QString sval = sset.value();
    if( sset.is_tableRef() ) {
        sval = m_skill.getSetValueForLevelS( sset.name(), m_level );
    }
    QTreeWidgetItem *item = new QTreeWidgetItem();
    // header_labels << "set_name" << "set_value";
    item->setText( 0, sset.name() );
    //item->setIcon( 0, QIcon( skill.icon() ) );
    item->setText( 1, sval );
    //item->setData( 0, Qt::UserRole, QVariant( skill.skillId() ) );
    this->ui->tree->addTopLevelItem( item );
}


// static
bool SkillInfoPane::isStringTableRef( const QString &v ) {
    if( v.isEmpty() ) return false;
    QChar c = v.at( 0 );
    if( c.unicode() == L'#' )
        return true;
    return false;
}


QString SkillInfoPane::effectValueWithTables( const QString& v ) {
    if( isStringTableRef(v) ) {
        // get value from table
        return m_skill.getTableValueForLevelS( v, m_level );
    } else {
        return v; // value itself
    }
}


QString SkillInfoPane::effectToStringWithTables( const L2SkillEffect& eff, bool appendMods ) {
    QString ret;

    if( eff.self() )
        ret.append( "Self " );

    if( !eff.effectName().isEmpty() )
        ret.append( eff.effectName() );

    if( eff.abnormalTime() > 0 )
        ret.append( QString(" (%1s)").arg( eff.abnormalTime() ) );

    if( !eff.abnormalType().isEmpty() )
        ret.append( QString("type:%1").arg(eff.abnormalType()) );

    if( !eff.effectCount().isEmpty() )
        ret.append( QString(" cnt:%1").arg(eff.effectCount()) );

    if( !eff.effectPower().isEmpty() )
        ret.append( QString(" power:%1").arg(eff.effectPower()) );

    if( eff.effectVal() )
        ret.append( QString(" val:%1").arg(eff.effectVal()) );

    if( !eff.chanceType().isEmpty() ) {
        ret.append( " [" );
        ret.append( eff.chanceType() );
        if( eff.triggeredId() > 0 )
            ret.append( QString(" / trigger:%1").arg(eff.triggeredId()) );
            L2Skill triggeredSkill = SkillsDB::getInstance()->getSkill( eff.triggeredId() );
            if( triggeredSkill.isValid() ) {
                ret.append( "<" );
                ret.append( triggeredSkill.skillName() );
                ret.append( ">" );
            }
        if( !eff.triggeredLevel().isEmpty() )
            ret.append( QString(" lv%1").arg(eff.triggeredLevel()) );
        if( !eff.activationChance().isEmpty() )
            ret.append( QString(" / %1%").arg( this->effectValueWithTables(eff.activationChance()) ) );
        ret.append( "]" );
    }

    if( !eff.usingKind().isEmpty() )
        ret.append( QString(" [with %1]").arg(eff.usingKind()) );

    if( appendMods ) {
        // append stat modifiers
        const QList<L2SkillStatModifier> _mods = eff.statMods();
        if( _mods.size() > 0 ) {
            ret.append( ": " );
            QListIterator<L2SkillStatModifier> iter( _mods );
            while( iter.hasNext() ) {
                const L2SkillStatModifier& mod1 = iter.next();
                //ret.append( mod1.toString() );
                ret.append( mod1.opToString() );
                ret.append( this->effectValueWithTables( mod1.val() ) );
                ret.append( " " );
                ret.append( mod1.stat() );
                ret.append( " (" );
                ret.append( mod1.order() );
                ret.append( ")" );
                if( iter.hasNext() )
                    ret.append( "; " );
            }
        }
    }

    return ret;
}


void SkillInfoPane::addEffectToTree( const L2SkillEffect& eff, const QString& comment ) {
    QString effectName = eff.effectName();
    if( !comment.isEmpty() )
        effectName = comment;
    // construct item
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText( 0, effectName );
    item->setText( 1, this->effectToStringWithTables(eff) );
    QList<L2SkillStatModifier> stat_mods = eff.statMods();
    if( stat_mods.size() > 0 ) {
        // add stat mods as sub-items
        QListIterator<L2SkillStatModifier> miter(stat_mods);
        while(miter.hasNext()) {
            const L2SkillStatModifier& smod = miter.next();
            QString col2;
            col2.append( smod.opToString() );
            col2.append( this->effectValueWithTables( smod.val() ) );
            col2.append( " (" );
            col2.append( smod.order() );
            col2.append( ")" );
            QTreeWidgetItem *mod_item = new QTreeWidgetItem();
            mod_item->setText( 0, smod.stat() );
            mod_item->setText( 1, col2 );
            item->addChild( mod_item );
        }
    }
    // add item
    this->ui->tree->addTopLevelItem( item );
    item->setExpanded( true );
}
