#ifndef L2SKILLTABLE_H
#define L2SKILLTABLE_H

#include <QString>
#include <QList>


class L2SkillTable {
public:
    L2SkillTable();
    L2SkillTable( const QString& name, const QString& text );
    L2SkillTable( const L2SkillTable& other );

    const L2SkillTable& operator=( const L2SkillTable& other );
    bool operator==( const L2SkillTable& other );

public:
    void setName( const QString& name );
    void setValue( const QString& value );

    QString name() const { return _name; }
    QString value() const { return _value; }

    // TODO:
    QList<int> toListInt() const;
    QList<float> toListFloat() const;

protected:
    QString  _name;
    QString  _value;
};

#endif // L2SKILLTABLE_H
