#ifndef UTILS_DB_H
#define UTILS_DB_H

#include <QString>
class QSqlDatabase;
class QXmlStreamReader;

// escape value for inserting into queries, according to driver rules
QString db_escape_value( QSqlDatabase *db, const QString& value );


class UnparsedSkillParameterException {
public:
    explicit UnparsedSkillParameterException( int skillId, QXmlStreamReader& xml );
    QString toString() const;
protected:
    int _skillId;
    QString _token;
    qint64 _line;
};


class UnparsedSkillForParameterException {
public:
    explicit UnparsedSkillForParameterException( int skillId, QXmlStreamReader &xml );
    QString toString() const;
protected:
    int _skillId;
    QString _token;
    qint64 _line;
};


class UnparsedSkillAttributeException {
public:
    explicit UnparsedSkillAttributeException( int skillId, const QString& token, qint64 line );
    QString toString() const;
protected:
    int _skillId;
    QString _token;
    qint64 _line;
};


#endif // UTILS_DB_H
