#ifndef TAB_SEARCH_H
#define TAB_SEARCH_H

#include <QWidget>

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

protected:
    void connectSignals();
    void performSearch( QString text );

private:
    Ui::Tab_Search *ui;
};

#endif // TAB_SEARCH_H
