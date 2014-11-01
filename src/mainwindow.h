#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "tab_classes.h"
#include "tab_search.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

protected:
    void createTabClasses();
    void createTabSearch();

    Tab_Classes *m_tab_classes;
    Tab_Search  *m_tab_search;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
