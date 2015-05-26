#include "l2skilleffect.h"

L2SkillEffect::L2SkillEffect() {
    _effectName.clear();
    _abnormalType.clear();
    _abnormalTime = 0;
    _abnormalLvl = 0;
    _effectVal = 0;
    _usingKind.clear();
    _self = false;
    _noicon = false;
    _effectCount.clear();
    _effectPower.clear();
    _abnormalVisualEffect = 0;
    _triggeredId = 0;
    _triggeredLevel.clear();
    _chanceType.clear();
    _special.clear();
    _activationChance.clear();
    _activationMinDamage.clear();
    _activationSkills.clear();
    _mods.clear();
}


L2SkillEffect::L2SkillEffect( const L2SkillEffect& other ) {
    (*this) = other;
}


const L2SkillEffect& L2SkillEffect::operator=( const L2SkillEffect& other ) {
    if( this == &other ) return (*this);
    _effectName = other._effectName;
    _abnormalType = other._abnormalType;
    _abnormalTime = other._abnormalTime;
    _abnormalLvl = other._abnormalLvl;
    _effectVal = other._effectVal;
    _usingKind = other._usingKind;
    _self = other._self;
    _noicon = other._noicon;
    _effectCount = other._effectCount;
    _effectPower = other._effectPower;
    _abnormalVisualEffect = other._abnormalVisualEffect;
    _triggeredId = other._triggeredId;
    _triggeredLevel = other._triggeredLevel;
    _chanceType = other._chanceType;
    _special = other._special;
    _activationChance = other._activationChance;
    _activationMinDamage = other._activationMinDamage;
    _activationSkills = other._activationSkills;
    _mods.clear();
    _mods = other._mods;
    return (*this);
}


bool L2SkillEffect::isValid() const {
    if( !_effectName.isEmpty() )
        return true;
    if( _mods.size() > 0 )
        return true;
    return false;
}


void L2SkillEffect::setEffectName( const QString& name ) {
    _effectName = name;
}


void L2SkillEffect::setAbnormalType( const QString& at ) {
    _abnormalType = at;
}


void L2SkillEffect::setAbnormalTime( int atm ) {
    _abnormalTime = atm;
}


void L2SkillEffect::setAbnormalLvl( int anl ) {
    _abnormalLvl = anl;
}


void L2SkillEffect::setEffectVal( int ev ) {
    _effectVal = ev;
}


void L2SkillEffect::setUsingKind( const QString& kind ) {
    _usingKind = kind;
}


void L2SkillEffect::addStatMod( L2SkillStatModifier mod ) {
    _mods.append( mod );
}


QString L2SkillEffect::toString( bool appendMods ) const {
    QString ret;

    if( _self )
        ret.append( "Self " );

    if( !_effectName.isEmpty() )
        ret.append( _effectName );

    if( _abnormalTime > 0 )
        ret.append( QString(" (%1s)").arg(_abnormalTime) );

    if( !_abnormalType.isEmpty() )
        ret.append( QString(" type:%1").arg(_abnormalType) );

    if( !_effectCount.isEmpty() )
        ret.append( QString(" cnt:%1").arg(_effectCount) );

    if( !_effectPower.isEmpty() )
        ret.append( QString(" power:%1").arg(_effectPower) );

    if( _effectVal )
        ret.append( QString(" val:%1").arg(_effectVal) );

    if( !_chanceType.isEmpty() ) {
        ret.append( " " );
        ret.append( _chanceType );
        if( _triggeredId > 0 )
            ret.append( QString("/trigger:%1").arg(_triggeredId) );
        if( !_triggeredLevel.isEmpty() )
            ret.append( QString(" lv%1").arg(_triggeredLevel) );
        if( !_activationChance.isEmpty() )
            ret.append( QString("/%1%").arg(_activationChance) );
    }

    if( !_usingKind.isEmpty() )
        ret.append( QString(" [with %1]").arg(_usingKind) );

    if( appendMods ) {
        // append stat modifiers
        if( _mods.size() > 0 ) {
            ret.append( ": " );
            QListIterator<L2SkillStatModifier> iter( _mods );
            while( iter.hasNext() ) {
                const L2SkillStatModifier& mod1 = iter.next();
                ret.append( mod1.toString() );
                if( iter.hasNext() )
                    ret.append( ", " );
            }
        }
    }

    return ret;
}
