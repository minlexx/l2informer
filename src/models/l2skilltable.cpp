#include "l2skilltable.h"

L2SkillTable::L2SkillTable() {
    _name.clear();
    _value.clear();
}


L2SkillTable::L2SkillTable( const QString& name, const QString& value ) {
    setName( name );
    setValue( value );
}


L2SkillTable::L2SkillTable( const L2SkillTable& other ) {
    _name.clear();
    _value.clear();
    (*this) = other;
}


const L2SkillTable& L2SkillTable::operator=( const L2SkillTable& other ) {
    if( this == &other ) return *this;
    _name = other._name;
    _value = other._value;
    return *this;
}


bool L2SkillTable::operator==( const L2SkillTable& other ) {
    return (_name.compare( other._name ) == 0);
}


void L2SkillTable::setName( const QString& name ) {
    _name = name;
}


void L2SkillTable::setValue( const QString& value ) {
    _value = value;
}
