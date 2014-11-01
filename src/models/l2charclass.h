#ifndef L2CHARCLASS_H
#define L2CHARCLASS_H

#include <QString>

class L2CharClass {
public:
    L2CharClass();
    L2CharClass( int id, const QString& name, int parent_id=-1 );
    L2CharClass( const L2CharClass &other );
    const L2CharClass& operator=( const L2CharClass &other );
    bool operator==( const L2CharClass& other );

public:
    bool isValid() const { return (m_id != -1); }
    bool hasParent() const { return (m_parent_id != -1); }
    int classId() const { return m_id; }
    int parentClassId() const { return m_parent_id; }
    const QString& name() const { return m_name; }

protected:
    int m_id;
    int m_parent_id;
    QString m_name;
};

#endif // L2CHARCLASS_H
