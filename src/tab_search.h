#ifndef TAB_SEARCH_H
#define TAB_SEARCH_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QTimer>

namespace Ui {
class Tab_Search;
}

class Tab_Search : public QWidget {
    Q_OBJECT

public:
    explicit Tab_Search( QWidget *parent = 0 );
    ~Tab_Search();

public slots:
    void initView();
    void onSearchBtnClicked();
    void onSearchTextEdited( QString text );
    void onTimerAutoSearch();
    void onTreeSkill_itemPressed( QTreeWidgetItem *item, int column );

protected:
    void connectSignals();
    void performSearch( QString text );

protected:
    QTimer *m_as_timer;

private:
    Ui::Tab_Search *ui;
};

#endif // TAB_SEARCH_H
