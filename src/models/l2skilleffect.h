#ifndef L2SKILLEFFECT_H
#define L2SKILLEFFECT_H

#include <QString>
#include <QList>
#include "l2skillstatmodifier.h"

class L2SkillEffect {
public:
    L2SkillEffect();
    L2SkillEffect( const L2SkillEffect& other );
    const L2SkillEffect& operator=( const L2SkillEffect& other );

    bool isValid() const;

    QString effectName() const { return _effectName; }
    void setEffectName( const QString& name );

    QString abnormalType() const { return _abnormalType; }
    void setAbnormalType( const QString& at );

    int abnormalTime() const { return _abnormalTime; }
    void setAbnormalTime( int atm );

    int abnormalLvl() const { return _abnormalLvl; }
    void setAbnormalLvl( int anl );

    int effectVal() const { return _effectVal; }
    void setEffectVal( int ev );

    QString usingKind() const { return _usingKind; }
    void setUsingKind( const QString& kind );

    bool self() const { return _self; }
    void setSelf( bool self ) { _self = self; }

    bool noicon() const { return _noicon; }
    void setNoicon( bool ni ) { _noicon = ni; }

    QString effectCount() const { return _effectCount; }
    void setEffectCount( const QString& c ) { _effectCount = c; }

    QString effectPower() const { return _effectPower; }
    void setEffectPower( const QString& p ) { _effectPower = p; }

    int abnormalVisualEffect() const { return _abnormalVisualEffect; }
    void setAbnormalVisualEffect( int a ) { _abnormalVisualEffect = a; }

    int triggeredId() const { return _triggeredId; }
    void setTriggeredId( int tid ) { _triggeredId = tid; }

    QString triggeredLevel() const { return _triggeredLevel; }
    void setTriggeredLevel( const QString& lv ) { _triggeredLevel = lv; }

    QString chanceType() const { return _chanceType; }
    void setChanceType( const QString& ct ) { _chanceType = ct; }

    QString activationMinDamage() const { return _activationMinDamage; }
    void setActivationMinDamage( const QString& A ) { _activationMinDamage = A; }

    QString special() const { return _special; }
    void setSpecial( const QString& sp ) { _special = sp; }

    QString activationChance() const { return _activationChance; }
    void setActivationChance( const QString& a ) { _activationChance = a; }

    QString activationSkills() const { return _activationSkills; }
    void setActivationSkills( const QString& a ) { _activationSkills = a; }

    void addStatMod( L2SkillStatModifier mod );
    const QList<L2SkillStatModifier> statMods() const { return _mods; }

    QString toString( bool appendMods = false ) const;

protected:
    QString _effectName;
    QString _abnormalType;
    int     _abnormalTime;
    int     _abnormalLvl;
    int     _effectVal;
    QString _usingKind;
    bool    _self;
    bool    _noicon;
    QString _effectCount;
    QString _effectPower;
    int     _abnormalVisualEffect;
    int     _triggeredId;
    QString _triggeredLevel;
    QString _chanceType;
    QString _special;
    QString _activationChance;
    QString _activationMinDamage;
    QString _activationSkills;
    // effect stat modifiers
    QList<L2SkillStatModifier> _mods;
};

#endif // L2SKILLEFFECT_H
