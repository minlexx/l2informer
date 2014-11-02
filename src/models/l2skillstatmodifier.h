#ifndef L2SKILLSTATMODIFIER_H
#define L2SKILLSTATMODIFIER_H

#include <QString>

class L2SkillStatModifier {
public:
    enum OP_TYPE {
        OP_NONE = 0,
        OP_ADD = 1,
        OP_SUB = 2,
        OP_MUL = 3,
        OP_DIV = 4,
        OP_BASEMUL = 5,
        OP_SET = 6,
        OP_SHARE = 7
    };

public:
    L2SkillStatModifier();
    L2SkillStatModifier( const L2SkillStatModifier& other );
    const L2SkillStatModifier& operator=( const L2SkillStatModifier& other );

    OP_TYPE op() const { return _op; }
    void setOp( OP_TYPE op ) { _op = op; }
    void setOpS( const QString& kind );

    QString order() const { return _order; }
    void setOrder( const QString& o ) { this->_order = o; }

    QString stat() const { return _stat; }
    void setStat( const QString &st ) { this->_stat = st; }

    QString val() const { return _val; }
    void setVal( const QString& v ) { this->_val = v; }

    QString toString() const;

protected:
    OP_TYPE _op;
    QString _order;
    QString _stat;
    QString _val;
    // TODO: table reference? _val may point to skill table
    // <add order="0x40" stat="rEvas" val="#ench2rEvas" />
    // <table name="#magicLvl"> 20 24 ...</table>
};

#endif // L2SKILLSTATMODIFIER_H
