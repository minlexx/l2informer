#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow( QWidget *parent ): QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    createTabClasses();
    createTabSearch();
}


MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::createTabClasses() {
    m_tab_classes = new Tab_Classes( this->ui->tabWidget );
    m_tab_classes->show();
    this->ui->tabWidget->addTab( m_tab_classes, QString("Class skills") );
    m_tab_classes->initView();
}


void MainWindow::createTabSearch() {
    m_tab_search = new Tab_Search( this->ui->tabWidget );
    m_tab_search->show();
    this->ui->tabWidget->addTab( m_tab_search, QString("Search all skills") );
    m_tab_search->initView();
}
