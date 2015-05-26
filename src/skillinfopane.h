#ifndef SKILLINFOPANE_H
#define SKILLINFOPANE_H

#include <QWidget>
#include "models/l2skill.h"

namespace Ui {
class SkillInfoPane;
}

class SkillInfoPane: public QWidget {
    Q_OBJECT

public:
    explicit SkillInfoPane(QWidget *parent = 0);
    ~SkillInfoPane();

public:
    void setSkillAndLevel( const L2Skill& skill, int level );

protected:
    void initTree();
    void addSetToTree( const L2SkillSet& sset );
    void addEffectToTree( const L2SkillEffect& eff, const QString& comment = QString() );

    static bool isStringTableRef( const QString& v );
    QString effectValueWithTables( const QString& v );
    QString effectToStringWithTables( const L2SkillEffect& eff, bool appendMods = true );

protected:
    L2Skill m_skill;
    int     m_level;

private:
    Ui::SkillInfoPane *ui;
};

#endif // SKILLINFOPANE_H
