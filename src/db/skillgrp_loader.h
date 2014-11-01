#ifndef SKILLGRP_LOADER_H
#define SKILLGRP_LOADER_H

#include <QString>
#include <QMap>

class Skillgrp_loader {
public:
    Skillgrp_loader();

    bool load();

    QString getSkillIconName( int skillId ) const;

protected:
    QMap<int, QString> _skill_icons;
};

#endif // SKILLGRP_LOADER_H
