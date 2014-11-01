#ifndef UTILS_DB_H
#define UTILS_DB_H

#include <QSqlDatabase>

// escape value for inserting into queries, according to driver rules
QString db_escape_value( QSqlDatabase *db, const QString& value );

#endif // UTILS_DB_H
