#include "l2charclass.h"

L2CharClass::L2CharClass() {
    m_id        = -1;
    m_parent_id = -1;
    m_name.clear();
}

L2CharClass::L2CharClass( int id, const QString& name, int parent_id ) {
    m_id         = id;
    m_parent_id  = parent_id;
    m_name       = name;
}

L2CharClass::L2CharClass( const L2CharClass& other ) {
    (*this) = other;
}

const L2CharClass& L2CharClass::operator=( const L2CharClass& other ) {
    if( this == &other ) return *this;
    m_id         = other.m_id;
    m_parent_id  = other.m_parent_id;
    m_name       = other.m_name;
    return *this;
}

bool L2CharClass::operator==( const L2CharClass& other ) {
    return m_id == other.m_id;
}
