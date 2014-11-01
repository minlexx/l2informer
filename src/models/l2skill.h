#ifndef L2SKILL_H
#define L2SKILL_H

#include <QString>
#include <QMap>
#include <QList>
#include <QPixmap>

#include "l2skilltable.h"
#include "l2skillset.h"


class L2Skill {
public:
    L2Skill();
    L2Skill( const L2Skill& other );
    const L2Skill& operator=( const L2Skill& other );
    bool operator==( const L2Skill& other );

    void setSkillId( int skillId );
    void setSkillName( const QString& name );
    void setNumLevels( int levels );
    void setIconName( const QString& iconName );
    void setForgottenScroll( bool fs );

    void addSet( const QString& set_name, const QString& set_value );
    void addTable( const QString& table_name, const QString& table_value );


    int      skillId() const { return _skillId; }
    QString  skillName() const { return _skillName; }
    int      numLevels() const { return _levels; }
    QString  iconName() const { return _iconName; }
    QPixmap  icon() const;
    bool     forgottenScroll() const { return _forgottenScroll; }

    QString  toString() const;

    bool     isMagic() const;
    bool     isPassive() const;
    bool     isToggle() const;
    QString  getSkillType() const;
    int      getMagicLvl() const;

protected:
    bool     isSetTable( const QString& set_name ) const;
    QString  getSetValueS( const QString& set_name, bool issueWarning = true ) const;
    //float   getSetValueF( const QString& set_name, int level ) const;

protected:
    int     _skillId;
    QString _skillName;
    int     _levels;
    QString _iconName;
    QPixmap _pixmap;
    bool    _forgottenScroll;
    // sets and tables
    QMap<QString, L2SkillSet>   _sets;
    QMap<QString, L2SkillTable> _tables;
};

#endif // L2SKILL_H
