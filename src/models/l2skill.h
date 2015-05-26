#ifndef L2SKILL_H
#define L2SKILL_H

#include <QString>
#include <QMap>
#include <QList>
#include <QPixmap>

#include "l2skilltable.h"
#include "l2skillset.h"
#include "l2skilleffect.h"


class L2Skill {
public:
    L2Skill();
    L2Skill( const L2Skill& other );
    const L2Skill& operator=( const L2Skill& other );
    bool operator==( const L2Skill& other );

    void setSkillId( int skillId );
    void setSkillName( const QString& name );
    void setNumLevels( int levels );
    void setNumEnchants( int n ) { _num_enchants = n; }
    void setIconName( const QString& iconName );
    void setForgottenScroll( bool fs );

    bool isValid() const { return _skillId != -1; }

    int      skillId() const { return _skillId; }
    QString  skillName() const { return _skillName; }
    int      numLevels() const { return _levels; }
    int      numEnchants() const { return _num_enchants; }
    QString  iconName() const { return _iconName; }
    QPixmap  icon() const;
    bool     forgottenScroll() const { return _forgottenScroll; }

    QString  toString() const;

    bool     isMagic() const;
    bool     isPassive() const;
    bool     isToggle() const;
    QString  getSkillType() const;
    int      getMagicLvl() const;

public:
    // sets/tables/effects
    void addSet( const QString& set_name, const QString& set_value );
    void addTable( const QString& table_name, const QString& table_value );
    void addEffect( const L2SkillEffect& eff );

    L2SkillEffect *defaultEffect() { return &_defaultEffect; }
    const L2SkillEffect *defaultEffect() const { return &_defaultEffect; }

    QMap<QString, L2SkillSet> *getAllSets();
    QList<L2SkillEffect> *getAllEffects();

    bool     isSetTable( const QString& set_name ) const;
    QString  getSetValueS( const QString& set_name, bool issueWarning = true ) const;
    //float   getSetValueF( const QString& set_name, int level ) const;
    QString  getSetValueForLevelS( const QString& set_name, int level ) const;
    QString  getTableValueForLevelS( const QString& table_name, int level ) const;

protected:
    int     _skillId;
    QString _skillName;
    int     _levels;
    int     _num_enchants;
    QString _iconName;
    QPixmap _pixmap;
    bool    _forgottenScroll;
    // sets and tables
    QMap<QString, L2SkillSet>   _sets;
    QMap<QString, L2SkillTable> _tables;
    // effects
    L2SkillEffect _defaultEffect;
    QList<L2SkillEffect> _effects;
};

#endif // L2SKILL_H
