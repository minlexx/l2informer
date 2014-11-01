#include "l2skillset.h"

L2SkillSet::L2SkillSet() {
    _name.clear();
    _value.clear();
}


L2SkillSet::L2SkillSet( const QString& name, const QString& value ) {
    setName( name );
    setValue( value );
}


L2SkillSet::L2SkillSet( const L2SkillSet& other ) {
    (*this) = other;
}


const L2SkillSet& L2SkillSet::operator=( const L2SkillSet& other ) {
    if( this == &other ) return *this;
    _name = other.name();
    _value = other.value();
    return *this;
}


bool L2SkillSet::operator==( const L2SkillSet& other ) {
    return ( (_name.compare(other._name) == 0) && (_value.compare(other._value) == 0) );
}


void L2SkillSet::setName( const QString& name ) {
    _name = name;
}


void L2SkillSet::setValue( const QString& value ) {
    _value = value;
}


bool L2SkillSet::is_tableRef() const {
    if( _value.isEmpty() ) return false;
    QChar c = _value.at( 0 );
    if( c.unicode() == L'#' )
        return true;
    return false;
}
