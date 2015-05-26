#include "l2skillstatmodifier.h"


L2SkillStatModifier::L2SkillStatModifier() {
    _order.clear();
    _stat.clear();
    _val.clear();
    _op = OP_NONE;
}


L2SkillStatModifier::L2SkillStatModifier( const L2SkillStatModifier& other ) {
    (*this) = other;
}


const L2SkillStatModifier& L2SkillStatModifier::operator=( const L2SkillStatModifier& other ) {
    if( this == &other ) return (*this);
    this->_order = other._order;
    this->_stat = other._stat;
    this->_val = other._val;
    this->_op = other._op;
    return (*this);
}


void L2SkillStatModifier::setOpS( const QString& kind ) {
    if( kind == "add" ) {
        setOp( OP_ADD );
    } else if( kind == "sub" ) {
        setOp( OP_SUB );
    } else if( kind == "mul" ) {
        setOp( OP_MUL );
    } else if( kind == "div" ) {
        setOp( OP_DIV );
    } else if( kind == "basemul" ) {
        setOp( OP_BASEMUL );
    } else if( kind == "set" ) {
        setOp( OP_SET );
    } else if( kind == "share" ) {
        setOp( OP_SHARE );
    } else {
        qWarning( "L2SkillStatModifier: UNKNOWN OP = [%s]", kind.toUtf8().data() );
    }
}


bool L2SkillStatModifier::is_tableRef() const {
    if( _val.isEmpty() ) return false;
    QChar c = _val.at( 0 );
    if( c.unicode() == L'#' )
        return true;
    return false;
}


QString L2SkillStatModifier::toString() const {
    QString ret;
    ret.append( this->opToString() );
    ret.append( this->_val );
    ret.append( " " );
    ret.append( this->_stat );
    ret.append( " (" );
    ret.append( this->_order );
    ret.append( ")" );
    return ret;
}


QString L2SkillStatModifier::opToString() const {
    QString ret;
    switch( _op ) {
    case OP_ADD: ret.append( "+" ); break;
    case OP_SUB: ret.append( "-" ); break;
    case OP_MUL: ret.append( "*" ); break;
    case OP_DIV: ret.append( "/" ); break;
    case OP_BASEMUL: ret.append( "base*" ); break;
    case OP_SET: ret.append( "set " ); break;
    case OP_SHARE: ret.append( "share " ); break;
    default: ret.append( "UNK_OP " ); break;
    }
    return ret;
}
