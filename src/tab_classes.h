#ifndef TAB_CLASSES_H
#define TAB_CLASSES_H

#include <QWidget>
#include <QTreeWidgetItem>


namespace Ui {
class Tab_Classes;
}


class Tab_Classes : public QWidget {
    Q_OBJECT

public:
    explicit Tab_Classes( QWidget *parent = 0 );
    ~Tab_Classes();

public slots:
    void initView();

    void onTree_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous );
    void onTreeSkill_itemPressed( QTreeWidgetItem *item, int column );
    void onCbMinLvlActivated( int index );
    void onCbMaxLvlActivated( int index );
    void onChk_hidePassive();
    void onActionSkillInfo( bool checked );

protected:
    void connectSignals();
    int  getCurrentSelectedTreeClassId();
    void populateChildClasses( QTreeWidgetItem *item, int classId );
    void fillSuggestlevelsCombo( int level_min, int level_max );
    void fillClassSkillsForLevel( int classId, int lv_min, int lv_max );

private:
    Ui::Tab_Classes *ui;
    bool m_hidePassiveSkills;
};

#endif // TAB_CLASSES_H
