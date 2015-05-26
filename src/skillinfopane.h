#ifndef SKILLINFOPANE_H
#define SKILLINFOPANE_H

#include <QWidget>

namespace Ui {
class SkillInfoPane;
}

class SkillInfoPane : public QWidget
{
    Q_OBJECT

public:
    explicit SkillInfoPane(QWidget *parent = 0);
    ~SkillInfoPane();

private:
    Ui::SkillInfoPane *ui;
};

#endif // SKILLINFOPANE_H
