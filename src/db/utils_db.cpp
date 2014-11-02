#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlField>
#include <QXmlStreamReader>
#include "utils_db.h"


QString db_escape_value( QSqlDatabase *db, const QString& value ) {
    QString ret = value;
    if( !db ) return ret;
    // construct field
    QSqlField field( "field", QVariant::String );
    field.setAutoValue( false );
    field.setDefaultValue( QString("") );
    field.setGenerated( false );
    field.setReadOnly( false );
    field.setRequired( false );
    field.setValue( value );
    // get driver
    QSqlDriver *drv = db->driver();
    if( drv ) {
        ret = drv->formatValue( field );
    }
    return ret;
}


UnparsedSkillParameterException::UnparsedSkillParameterException( int skillId, QXmlStreamReader& xml ) {
    _skillId = skillId;
    _token = xml.name().toString();
    _line = xml.lineNumber();
}

QString UnparsedSkillParameterException::toString() const {
    return QString( "skill [%1] has unparsed token in XML: [%2] at line %3" ).arg( _skillId ).arg( _token ).arg( _line );
}

UnparsedSkillForParameterException::UnparsedSkillForParameterException( int skillId, QXmlStreamReader& xml ) {
    _skillId = skillId;
    _token = xml.name().toString();
    _line = xml.lineNumber();
}

QString UnparsedSkillForParameterException::toString() const {
    return QString( "skill [%1] has unparsed 'for' token in XML: [%2] at line %3" ).arg( _skillId ).arg( _token ).arg( _line );
}

UnparsedSkillAttributeException::UnparsedSkillAttributeException( int skillId, const QString& token, qint64 line ) {
    _skillId = skillId;
    _token = token;
    _line = line;
}

QString UnparsedSkillAttributeException::toString() const {
    return QString( "skill [%1] has unparsed attribute in XML: [%2] at line %3" ).arg( _skillId ).arg( _token ).arg( _line );
}
