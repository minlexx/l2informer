#ifndef CHARCLASSDB_H
#define CHARCLASSDB_H

#include <QString>
#include <QMap>
#include <QList>

#include "../models/l2charclass.h"

class CharClassDB {
public:
    static CharClassDB *getInstance();

protected:
    static CharClassDB *_s_instance;
    CharClassDB();

public:
    bool load();

public:
    QList<L2CharClass> getBaseClassesList();
    QList<L2CharClass> getChildClassesList( int baseClassId );
    L2CharClass getClassById( int classId );

protected:
    QMap<int, L2CharClass> m_classDb;
};

#endif // CHARCLASSDB_H
