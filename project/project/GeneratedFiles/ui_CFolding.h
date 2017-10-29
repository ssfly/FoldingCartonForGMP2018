/********************************************************************************
** Form generated from reading UI file 'CFolding.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CFOLDING_H
#define UI_CFOLDING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CFoldingClass
{
public:
    QAction *actionInitialize;
    QAction *actionLoad_2;
    QAction *actionSave;
    QAction *actionOptimize;
    QAction *actionLoadtest;
    QWidget *centralWidget;
    QToolButton *MoveTool;
    QToolButton *VertexTool;
    QToolButton *EdgeTool;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *show2dLayout;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *show3dLayout;
    QScrollArea *candiLayout;
    QWidget *scrollAreaWidgetContents;
    QToolButton *SymmetryTool;
    QToolButton *mergeTool;
    QToolButton *faceTool;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *CFoldingClass)
    {
        if (CFoldingClass->objectName().isEmpty())
            CFoldingClass->setObjectName(QStringLiteral("CFoldingClass"));
        CFoldingClass->resize(908, 588);
        actionInitialize = new QAction(CFoldingClass);
        actionInitialize->setObjectName(QStringLiteral("actionInitialize"));
        actionLoad_2 = new QAction(CFoldingClass);
        actionLoad_2->setObjectName(QStringLiteral("actionLoad_2"));
        actionSave = new QAction(CFoldingClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionOptimize = new QAction(CFoldingClass);
        actionOptimize->setObjectName(QStringLiteral("actionOptimize"));
        actionLoadtest = new QAction(CFoldingClass);
        actionLoadtest->setObjectName(QStringLiteral("actionLoadtest"));
        centralWidget = new QWidget(CFoldingClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MoveTool = new QToolButton(centralWidget);
        MoveTool->setObjectName(QStringLiteral("MoveTool"));
        MoveTool->setGeometry(QRect(400, 10, 31, 31));
        MoveTool->setCheckable(true);
        MoveTool->setAutoExclusive(true);
        VertexTool = new QToolButton(centralWidget);
        VertexTool->setObjectName(QStringLiteral("VertexTool"));
        VertexTool->setGeometry(QRect(400, 50, 31, 31));
        VertexTool->setCheckable(true);
        VertexTool->setAutoExclusive(true);
        EdgeTool = new QToolButton(centralWidget);
        EdgeTool->setObjectName(QStringLiteral("EdgeTool"));
        EdgeTool->setGeometry(QRect(400, 90, 31, 31));
        EdgeTool->setCheckable(true);
        EdgeTool->setAutoExclusive(true);
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(20, 10, 371, 371));
        show2dLayout = new QVBoxLayout(verticalLayoutWidget);
        show2dLayout->setSpacing(6);
        show2dLayout->setContentsMargins(11, 11, 11, 11);
        show2dLayout->setObjectName(QStringLiteral("show2dLayout"));
        show2dLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayoutWidget_2 = new QWidget(centralWidget);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(440, 10, 431, 371));
        show3dLayout = new QVBoxLayout(verticalLayoutWidget_2);
        show3dLayout->setSpacing(6);
        show3dLayout->setContentsMargins(11, 11, 11, 11);
        show3dLayout->setObjectName(QStringLiteral("show3dLayout"));
        show3dLayout->setContentsMargins(0, 0, 0, 0);
        candiLayout = new QScrollArea(centralWidget);
        candiLayout->setObjectName(QStringLiteral("candiLayout"));
        candiLayout->setGeometry(QRect(20, 390, 851, 141));
        candiLayout->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        candiLayout->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        candiLayout->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 849, 122));
        candiLayout->setWidget(scrollAreaWidgetContents);
        SymmetryTool = new QToolButton(centralWidget);
        SymmetryTool->setObjectName(QStringLiteral("SymmetryTool"));
        SymmetryTool->setGeometry(QRect(400, 170, 31, 31));
        SymmetryTool->setCheckable(true);
        SymmetryTool->setAutoExclusive(true);
        mergeTool = new QToolButton(centralWidget);
        mergeTool->setObjectName(QStringLiteral("mergeTool"));
        mergeTool->setGeometry(QRect(400, 210, 31, 31));
        mergeTool->setCheckable(true);
        mergeTool->setAutoExclusive(true);
        faceTool = new QToolButton(centralWidget);
        faceTool->setObjectName(QStringLiteral("faceTool"));
        faceTool->setGeometry(QRect(400, 130, 31, 31));
        faceTool->setCheckable(true);
        faceTool->setAutoExclusive(true);
        CFoldingClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(CFoldingClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        CFoldingClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        mainToolBar->addAction(actionLoad_2);
        mainToolBar->addAction(actionSave);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionInitialize);
        mainToolBar->addAction(actionOptimize);
        mainToolBar->addAction(actionLoadtest);

        retranslateUi(CFoldingClass);

        QMetaObject::connectSlotsByName(CFoldingClass);
    } // setupUi

    void retranslateUi(QMainWindow *CFoldingClass)
    {
        CFoldingClass->setWindowTitle(QApplication::translate("CFoldingClass", "CFolding", 0));
        actionInitialize->setText(QApplication::translate("CFoldingClass", "Initialize", 0));
        actionLoad_2->setText(QApplication::translate("CFoldingClass", "Load", 0));
        actionSave->setText(QApplication::translate("CFoldingClass", "Save", 0));
        actionOptimize->setText(QApplication::translate("CFoldingClass", "Optimize", 0));
        actionLoadtest->setText(QApplication::translate("CFoldingClass", "loadtest", 0));
        MoveTool->setText(QApplication::translate("CFoldingClass", "M", 0));
        VertexTool->setText(QApplication::translate("CFoldingClass", "V", 0));
        EdgeTool->setText(QApplication::translate("CFoldingClass", "E", 0));
        SymmetryTool->setText(QApplication::translate("CFoldingClass", "S", 0));
        mergeTool->setText(QApplication::translate("CFoldingClass", "O", 0));
        faceTool->setText(QApplication::translate("CFoldingClass", "F", 0));
    } // retranslateUi

};

namespace Ui {
    class CFoldingClass: public Ui_CFoldingClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CFOLDING_H
