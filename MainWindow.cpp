#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Function.h"
#include "util.h"
#include "colors.h"
#include "MyGraphicsView.h"
#include "GraphicsViewZoom.h"
#include "Highlighter.h"
#include "FrameForLineNumbers.h"
#include "DrawingForm.h"
#include "RuntimeProvider.h"
#include "CodeEditorTabPage.h"

#include <TabToolbar/TabToolbar.h>
#include <TabToolbar/Page.h>
#include <TabToolbar/Group.h>
#include <TabToolbar/SubGroup.h>
#include <TabToolbar/StyleTools.h>
#include <TabToolbar/Builder.h>

#include <QDebug>
#include <QSharedPointer>
#include <QPen>
#include <QDockWidget>
#include <QResizeEvent>
#include <QColorDialog>
#include <QLabel>
#include <QMenu>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

#include <functional>

MainWindow::MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_df(df)
{

    ui->setupUi(this);

    m_currentProgram.reset( new Program(ui->tabWidget->currentWidget(), rp));
    m_programs.append(m_currentProgram);

    ui->tabWidget->addTab(m_currentProgram->m_tabPage, tr("noname ") + QString::number(m_programs.size()));

    tt::Builder ttb(this);
    ttb.SetCustomWidgetCreator("textEdit", []() { return new QTextEdit(); });
    ttb.SetCustomWidgetCreator("checkBox", []() { return new QCheckBox(); });
    ttb.SetCustomWidgetCreator("pushButton", []() { return new QPushButton(); });
    tt::TabToolbar* tabToolbar = ttb.CreateTabToolbar(":/tt/tabtoolbar.json");
    addToolBar(Qt::TopToolBarArea, tabToolbar);
    tabToolbar->SetStyle("Vienna");

//    ttb["customTextEdit"]->setMaximumWidth(100);

//    ((QCheckBox*)ttb["customCheckBox"])->setText("Check 1");
//    QPushButton* btn = (QPushButton*)ttb["customEditButton"];
//    btn->setText("Edit");
//    static bool kek = true;
//    tt::Page* editPage = (tt::Page*)ttb["Edit"];
//    QObject::connect(btn, &QPushButton::clicked, [editPage]()
//                     {
//                         if(kek)
//                             editPage->hide();
//                         else
//                             editPage->show();
//                         kek = !kek;
//                     });

//    QObject::connect(tabToolbar, &tt::TabToolbar::SpecialTabClicked, this, [this]()
//                     {
//                         QMessageBox::information(this, "Kek", "Cheburek");
//                     });

//    //create buttons for each style
//    tt::Group* stylesGroup = (tt::Group*)ttb["Styles"];
//    stylesGroup->AddSeparator();
//    QStringList styles = tt::GetRegisteredStyles();
//    for(int i=0; i<styles.size(); i++)
//    {
//        const QString styleName = styles.at(i);
//        QPushButton* btn = new QPushButton(styleName, this);
//        QObject::connect(btn, &QPushButton::clicked, [styleName, tabToolbar]() { tabToolbar->SetStyle(styleName); });
//        stylesGroup->AddWidget(btn);
//    }

//    tt::RegisterStyle("NoStyle", []()
//                      {
//                          tt::StyleParams* params = new tt::StyleParams();
//                          params->UseTemplateSheet = false;
//                          params->AdditionalStyleSheet = "";
//                          return params;
//                      });
//    btn = (QPushButton*)ttb["nativeStyleButton"];
//    btn->setText("No Style");
//    QObject::connect(btn, &QPushButton::clicked, [tabToolbar]() { tabToolbar->SetStyle("NoStyle"); });
//    btn = (QPushButton*)ttb["defaultStyleButton"];
//    btn->setText("Default");
//    QObject::connect(btn, &QPushButton::clicked, [tabToolbar]() { tabToolbar->SetStyle(tt::GetDefaultStyle()); });


    connect(ui->actionRun, &QAction::triggered, this, [this]() {runCurrentCode();});


}

// <a target="_blank" href="https://icons8.com/icon/3439/about">About</a> icon by <a target="_blank" href="https://icons8.com">Icons8</a>

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reportError(QString err)
{
    static bool firstMessage = true;
    static QLabel *label = nullptr;
    if(firstMessage)
    {
        label = new QLabel;
        statusBar()->addPermanentWidget(label, 1);
        firstMessage = false;
    }
    label->setText(err);
    qWarning() << err;
}


void MainWindow::runCurrentCode()
{
    m_df->reset();
    m_df->drawCoordinateSystem();

    m_currentProgram->run();
}

void MainWindow::setCurrentStatement(const QString &newCurrentStatement)
{
    m_currentProgram->setCurrentStatement(newCurrentStatement);
}

Program::Program(QWidget *tabContainer, RuntimeProvider *rp) : m_rp(rp)
{
    m_tabPage = new CodeEditorTabPage(tabContainer, rp);
}

void Program::setCurrentStatement(const QString &newCurrentStatement)
{
    currentStatement = newCurrentStatement;

}

void Program::run()
{
    m_rp->reset();
    QStringList codelines = m_tabPage->get_textEdit()->toPlainText().split("\n");
    m_rp->parse(codelines);
    m_rp->execute();

}
