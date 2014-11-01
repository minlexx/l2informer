#include "l2skill.h"

L2Skill::L2Skill() {
    _skillId = -1;
    _skillName.clear();
    _levels = 0;
    _iconName.clear();
    _forgottenScroll = false;
    //
    _sets.clear();
    _tables.clear();
}

L2Skill::L2Skill( const L2Skill& other ) {
    (*this) = other;
}


const L2Skill& L2Skill::operator=( const L2Skill& other ) {
    if( this == &other ) return (*this);
    //
    _skillId = other._skillId;
    _skillName = other._skillName;
    _levels = other._levels;
    _iconName = other._iconName;
    _pixmap = other._pixmap;
    _forgottenScroll = other._forgottenScroll;
    //
    _sets = other._sets;
    _tables = other._tables;
    //
    return (*this);
}


bool L2Skill::operator==( const L2Skill& other ) {
    return (this->_skillId == other._skillId);
}


QString L2Skill::toString() const {
    QString ret = QString( "%1[ID=%2]" ).arg( _skillName ).arg( _skillId );
    return ret;
}


void L2Skill::setSkillId( int skillId ) { _skillId = skillId; }

void L2Skill::setSkillName( const QString& name ) { _skillName = name; }

void L2Skill::setNumLevels( int levels ) { _levels = levels; }

void L2Skill::setIconName( const QString& iconName ) { _iconName = iconName; }

void L2Skill::setForgottenScroll( bool fs ) { _forgottenScroll = fs; }


QPixmap L2Skill::icon() const {
    if( this->_pixmap.isNull() ) {
        L2Skill *non_const_this = const_cast<L2Skill *>(this);
        QString realName = non_const_this->_iconName;
        if( realName.startsWith( "icon." ) )
            realName = realName.mid( 5 );
        QString iconPath = QString( "data/icons/png/%1.png" ).arg( realName );
        if( !non_const_this->_pixmap.load( iconPath ) ) {
            qWarning( "L2Skill: ERROR: failed to load icon from [%s], skill %s",
                      iconPath.toUtf8().data(), this->toString().toUtf8().data() );
        }
    }
    return this->_pixmap;
}


void L2Skill::addSet( const QString& set_name, const QString& set_value ) {
    L2SkillSet skill_set( set_name, set_value );
    _sets.insert( set_name, skill_set );
}


void L2Skill::addTable( const QString& table_name, const QString& table_value ) {
    L2SkillTable skill_table( table_name, table_value );
    _tables.insert( table_name, skill_table );
}


bool L2Skill::isSetTable( const QString& set_name ) const {
    if( set_name.isEmpty() ) {
        qDebug( "L2Skill::isSetTable(): empty set name given!" );
        return false;
    }
    if( _sets.contains( set_name ) ) {
        const L2SkillSet& set = _sets.value( set_name );
        return set.is_tableRef(); // true way
    } else {
        qWarning( "L2Skill::isSetTable( %s ): no such set! (skill %s)", set_name.toUtf8().data(), toString().toUtf8().data() );
        return false;
    }
}


QString L2Skill::getSetValueS( const QString& set_name, bool issueWarning ) const {
    QString ret;
    if( _sets.contains( set_name ) ) {
        const L2SkillSet& set = _sets.value( set_name );
        return set.value();
    } else {
        if( issueWarning )
            qWarning( "L2Skill::getSetValueS( \"%s\" ): no such set! (skill %s)", set_name.toUtf8().data(), toString().toUtf8().data() );
    }
    return ret;
}


/*float L2Skill::getSetValueF( const QString& set_name, int level ) const {
    if( set_name.isEmpty() ) {
        qDebug( "L2Skill::getSetValueF( %s, %d ): empty set name given!", set_name.toUtf8().data(), level );
        return 0.0f;
    }
    if( _sets.contains( set_name ) ) {
        const L2SkillSet& set = _sets.value( set_name );
        if( !set.is_tableRef() ) {
            qWarning( "L2Skill::getSetValueF( %s, %d ): set is not a table!",
                      set_name.toUtf8().data(), level );
            QString s_val = set.value();
            return s_val.toFloat();
        }
        // this is table reference
        QString table_name = set.value();
        if( _tables.contains(table_name) ) {
            const L2SkillTable& table = _tables.value( table_name );
            return table.value( level );
        } else {
            qWarning( "L2Skill::getSetValueF( %s, %d ): cannot find table for set!", set_name.toUtf8().data(), level );
        }
    }
    qWarning( "L2Skill::getSetValueF( %s, %d ): no such set!", set_name.toUtf8().data(), level );
    return 0.0f;
}*/


bool L2Skill::isMagic() const {
    QString val = getSetValueS( "isMagic", false );
    return (val.compare( "1" ) == 0);
}


bool L2Skill::isPassive() const {
    QString val = getSetValueS( "operateType" );
    return (val.compare( "P" ) == 0);
}


bool L2Skill::isToggle() const {
    QString val = getSetValueS( "operateType" );
    return (val.compare( "T" ) == 0);
}


QString L2Skill::getSkillType() const {
    QString val = getSetValueS( "skillType", false ); // too many skills don't have skillType mentioined
    return val;
}


int  L2Skill::getMagicLvl() const {
    QString val = getSetValueS( "magicLvl" );
    if( val.isEmpty() ) return 0;
    int ret = 0;
    bool ok = false;
    ret = val.toInt( &ok );
    if( !ok ) return 0;
    return ret;
}
