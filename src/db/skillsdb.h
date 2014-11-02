#ifndef SKILLSDB_H
#define SKILLSDB_H

#include <QList>
#include <QMap>
#include <QXmlStreamReader>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "../models/l2skill.h"

class SkillsDB {
public:
    static SkillsDB *getInstance();

    bool load( void *splashScreenPointer = NULL );

    L2Skill getSkill( int skillId );
    QList<L2Skill> findSkill( const QString& skillNamePart );

protected:
    static SkillsDB *_s_instance;
    SkillsDB();

    int     loadSkillXML( const QString& fileName );
    L2Skill parseSkillDefinition( QXmlStreamReader& xml );
    void    parseSkillTableDefinition( L2Skill& skill, QXmlStreamReader& xml );
    void    parseSkillSetDefinition( L2Skill& skill, QXmlStreamReader& xml );
    void    parseSkillForDefinition( L2Skill& skill, QXmlStreamReader& xml );
    void    parseSkillEnchantDefinition( L2Skill& skill, QXmlStreamReader& xml );
    void    parseSkillCondDefinition( L2Skill& skill, QXmlStreamReader& xml );

    void    parseSkillEffectStatMod( L2Skill& skill, L2SkillEffect& eff, QXmlStreamReader& xml );

    int     loadForgottenSkillTree();

protected: // SQL Db funcs
    bool  _initDb();
    void  _detectSqlError( const QSqlQuery& query );
    void  _addSkillToDb( const L2Skill& skill );
    int   _db_count_skills();

protected:
    QMap<int, L2Skill> _skills; // skill_id => L2Skill info
    QSqlDatabase _db;
    void *m_splashScreenPointer;
};

#endif // SKILLSDB_H
