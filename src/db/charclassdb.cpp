#include "charclassdb.h"
#include <QFile>
#include <QXmlStreamReader>


CharClassDB *CharClassDB::_s_instance = NULL;


CharClassDB *CharClassDB::getInstance() {
    if( _s_instance == NULL ) {
        _s_instance = new CharClassDB();
    }
    return _s_instance;
}


CharClassDB::CharClassDB() {
}


bool CharClassDB::load() {
    QFile f( "data/classList.xml" );
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        qDebug( "Failed to read class list: data/classList.xml" );
        return false;
    }
    QXmlStreamReader xml( &f );
    while( !xml.atEnd() && !xml.hasError() ) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if( token == QXmlStreamReader::StartDocument )
            continue;
        if( token == QXmlStreamReader::StartElement ) {
            if( xml.name() == "class" ) {
                // <class classId="12" name="Sorcerer" serverName="sorcerer" parentClassId="11" />
                QXmlStreamAttributes attrs = xml.attributes();
                int id = -1;
                int parent_id = -1;
                QString name;
                if( attrs.hasAttribute( "classId" ) )
                    id = attrs.value( "classId" ).toInt();
                if( attrs.hasAttribute( "parentClassId" ) )
                    parent_id = attrs.value( "parentClassId" ).toInt();
                if( attrs.hasAttribute( "name" ) )
                    name = attrs.value( "name" ).toString();
                //
                //qDebug( "Loaded class [%s] (%d, %d)", name.toLocal8Bit().data(), id, parent_id );
                //
                L2CharClass l2_class( id, name, parent_id );
                m_classDb[ id ] = l2_class;
                //
                token = xml.readNext();
                while( token != QXmlStreamReader::EndElement )
                    xml.readNext();
            }
        }
    }
    if( xml.hasError() ) {
        qCritical( "XML read error: (data/classList.xml) %s", xml.errorString().toLocal8Bit().data() );
    }
    xml.clear();
    f.close();
    qDebug( "CharClassDB: Read class list: %d classes", m_classDb.size() );
    return true;
}


L2CharClass CharClassDB::getClassById( int classId ) {
    L2CharClass ret;
    if( m_classDb.contains( classId ) ) {
        ret = m_classDb[ classId ];
        return ret;
    }
    qWarning( "CharClassDB: requested nonexistent class ID %d!", classId );
    return ret;
}


QList<L2CharClass> CharClassDB::getBaseClassesList() {
    return getChildClassesList( -1 );
}


QList<L2CharClass> CharClassDB::getChildClassesList( int baseClassId ) {
    QList<L2CharClass> ret;
    QMapIterator<int, L2CharClass> iter( m_classDb );
    while( iter.hasNext() ) {
        iter.next();
        //
        const L2CharClass& cl = iter.value();
        if( cl.parentClassId() == baseClassId )
            ret.append( cl );
    }
    return ret;
}
