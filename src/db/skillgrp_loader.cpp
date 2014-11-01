#include "skillgrp_loader.h"

#include <QFile>
#include <QTextStream>


Skillgrp_loader::Skillgrp_loader() {
    _skill_icons.clear();
}


bool Skillgrp_loader::load() {
    QFile f( "data/Skillgrp.txt" );
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        qDebug( "Failed to read skills icons list: data/Skillgrp.txt" );
        return false;
    }
    int num_lines = 0;
    QTextStream ts( &f );
    QString line;
    while( !ts.atEnd() ) {
        line = ts.readLine();
        if( line.isNull() || line.isEmpty() )
            continue;
        num_lines++;
        QStringList sl = line.split( QChar(QChar::Tabulation) );
#if 0
        if( num_lines <= 2 ) {
            qDebug( "line %d:", num_lines );
            QStringListIterator iter( sl );
            while( iter.hasNext() ) {
                const QString& sss = iter.next();
                qDebug( "  [%s]", sss.toUtf8().data() );
            }
        }
#endif
        if( sl.size() >= 12 ) {
            // skip 1st line
            if( num_lines > 1 ) {
                QString s_id = sl.at(0);
                QString s_icon = sl.at(11);
                bool ok = false;
                int skill_id = s_id.toInt( &ok );
                if( ok ) {
                    _skill_icons.insert( skill_id, s_icon );
                    //qDebug( "%d=%s", skill_id, s_icon.toUtf8().data() );
                } else {
                    qWarning( "ERROR: Conversion error! (%s)", s_id.toUtf8().data() );
                }
            }
        } else {
            qWarning( "ERROR: failed to parse (split) string" );
        }
    }
    f.close();
    qDebug( "Skillgrp_loader: %d lines processed.", num_lines );
    return true;
}

/* line 2:
  [3] // 0 - skill id
  [1] // 1
  [0] // 2
  [0] // 3
  [9]  // 4
  [40]  // 5
  [3]    // 6
  [1.08000004] // 7
  [0]             // 8
  [S]              // 9
  [3]              // 10
  [icon.skill0003]  // 11 - icon
  []
  [0]
  [0]
  [0]
  [a,none\0]
  [0]
  [9]
  [11]
  [0]
  [a,none\0]  */


QString Skillgrp_loader::getSkillIconName( int skillId ) const {
    QString ret;
    if( _skill_icons.contains(skillId) ) {
        ret = _skill_icons.value(skillId);
    } else {
        qWarning( "Skillgrp_loader: WARNING: cannot find icon for skill %d", skillId );
    }
    return ret;
}
