#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QMessageBox>
#include "mainwindow.h"
#include "db/charclassdb.h"
#include "db/skilltree.h"
#include "db/skillsdb.h"


void loadClassDb( QSplashScreen *splash );
void loadSkillTree( QSplashScreen *splash );
void loadSkillsDb( QSplashScreen *splash );


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap splash_pic( "data/splash.png" );
    QSplashScreen *splash = new QSplashScreen( splash_pic );
    splash->show();
    a.processEvents();
    // load
    loadClassDb( splash );
    loadSkillTree( splash );
    loadSkillsDb( splash );

    MainWindow w;
    w.show();

    splash->finish( &w );

    return a.exec();
}



void loadClassDb( QSplashScreen *splash ) {
    splash->showMessage( "Loading char classes", Qt::AlignLeft|Qt::AlignBottom );
    qApp->processEvents();
    //
    CharClassDB *classes = CharClassDB::getInstance();
    if( !classes->load() )
        QMessageBox::critical( splash, QString("Load error!"),
            QString("Failed to load L2 class DB!") );
}


void loadSkillTree( QSplashScreen *splash ) {
    splash->showMessage( "Loading skill tree", Qt::AlignLeft|Qt::AlignBottom );
    qApp->processEvents();
    SkillTree *skillTree = SkillTree::getInstance();
    if( !skillTree->load() )
        QMessageBox::critical( splash, QString("Load error!"),
            QString("Failed to load skill trees!") );
}


void loadSkillsDb( QSplashScreen *splash ) {
    splash->showMessage( "Loading all skills", Qt::AlignLeft|Qt::AlignBottom );
    qApp->processEvents();
    SkillsDB *db = SkillsDB::getInstance();
    if( !db->load( splash ) )
        QMessageBox::critical( splash, QString("Load error!"),
            QString("Failed to load skills DB!") );
}
