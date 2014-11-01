#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlField>
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
