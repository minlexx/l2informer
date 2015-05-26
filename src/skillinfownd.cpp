#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include "skillinfownd.h"
#include "ui_skillinfownd.h"


void Launch_SkillInfoWnd(const L2Skill& skill, QWidget *parent) {
    SkillInfoWnd *wnd = new SkillInfoWnd(skill, parent);
    wnd->show();
}


SkillInfoWnd::SkillInfoWnd(const L2Skill& skill, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkillInfoWnd)
{
    m_skill = skill;
    ui->setupUi(this);
    this->fillTopInfo();
    this->add_skillXmlTab();
    this->fillLevelPanels();
}


SkillInfoWnd::~SkillInfoWnd() {
    delete ui;
}


void SkillInfoWnd::fillTopInfo() {
    this->setWindowTitle( m_skill.skillName() );
    QString lbl = QString("%1 (levels: %2, enchants: %3) ID=%4")
            .arg(m_skill.skillName())
            .arg(m_skill.numLevels())
            .arg(m_skill.numEnchants())
            .arg(m_skill.skillId());
    this->ui->lbl_skillName->setText(lbl);
    QPixmap skill_ico = m_skill.icon();
    this->ui->lbl_icon->setPixmap(skill_ico);
}


void SkillInfoWnd::fillLevelPanels() {
    int num_levels = m_skill.numLevels();
    int i;
    for( i=0; i<num_levels; i++ ) {
        SkillInfoPane *pane = new SkillInfoPane( this->ui->tabs );
        pane->setSkillAndLevel( m_skill, i );
        QString tabTitle = QString("lv%1").arg(i+1);
        this->ui->tabs->addTab( pane, tabTitle );
    }
}


void SkillInfoWnd::add_skillXmlTab() {
    int s100 = m_skill.skillId() / 100;
    char ss[512];
    sprintf( ss, "%03d00-%03d99", s100, s100 );
    QString xmlName = QString::fromUtf8( ss );
    QString cwd = QDir::current().absolutePath();
    QString xmlFullName = QString( "%1/data/skills/%2.xml" ).arg( cwd ).arg( xmlName );
    QFile f( xmlFullName );
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        qWarning( "SkillInfoWnd::add_skillXmlTab(): failed to open [%s]!",
                  xmlFullName.toUtf8().data() );
        return;
    }
    QTextStream fts( &f );
    QString all_xml = fts.readAll();
    f.close();
    if( all_xml.isEmpty() )
        return;
    int start_index = 0;
    int end_index = 0;
    // <skill id="467" levels="23" name="Soul Mastery">
    QString ttf = QString( "<skill id=\"%1\"" ).arg(m_skill.skillId());
    start_index = all_xml.indexOf( ttf, 0, Qt::CaseInsensitive );
    if( start_index == -1 ) {
        qWarning( "SkillInfoWnd::add_skillXmlTab(): cannot find [%s] in [%s]!",
                  ttf.toUtf8().data(),
                  xmlFullName.toUtf8().data() );
        return;
    }
    end_index = all_xml.indexOf( QString("</skill>"), start_index + 10, Qt::CaseInsensitive );
    if( end_index == -1 ) {
        qWarning( "SkillInfoWnd::add_skillXmlTab(): cannot find [</skill>] in [%s]!",
                  xmlFullName.toUtf8().data() );
        return;
    }
    end_index += 8;
    int len = end_index - start_index;
    QString skill_xml = all_xml.mid( start_index, len );
    //qDebug( "skill xml: [%s]", skill_xml.toUtf8().data() );
    QWidget *xml_panel = new QWidget( this );
    QVBoxLayout *w_vbox = new QVBoxLayout(xml_panel);
    QPlainTextEdit *pte = new QPlainTextEdit(xml_panel);
    xml_panel->setObjectName(QStringLiteral("xml_panel"));
    w_vbox->setObjectName(QStringLiteral("verticalLayout"));
    pte->setObjectName(QStringLiteral("plainTextEdit"));
    w_vbox->addWidget( pte );
    pte->setReadOnly( true );
    pte->setLineWrapMode( QPlainTextEdit::NoWrap );
    pte->appendPlainText( skill_xml );
    this->ui->tabs->addTab( xml_panel, QString("xml") );
}
