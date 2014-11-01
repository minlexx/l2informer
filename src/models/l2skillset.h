#ifndef L2SKILLSET_H
#define L2SKILLSET_H

#include <QString>

class L2SkillSet {
public:
    L2SkillSet();
    L2SkillSet( const QString& name, const QString& value );
    L2SkillSet( const L2SkillSet& other );
    const L2SkillSet& operator=( const L2SkillSet& other );
    bool operator==( const L2SkillSet& other );

    void setName( const QString& name );
    void setValue( const QString& value );

    QString name() const { return _name; }
    QString value() const { return _value; }
    bool is_tableRef() const;

protected:
    QString _name;
    QString _value;
};

#endif // L2SKILLSET_H
