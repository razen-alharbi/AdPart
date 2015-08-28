/********************************************************************************
** Form generated from reading UI file 'mastergui.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MASTERGUI_H
#define UI_MASTERGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MasterGUI
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_5;
    QTabWidget *tabs;
    QWidget *main_tab;
    QGridLayout *gridLayout_4;
    QFrame *frame_5;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_7;
    QPlainTextEdit *fileContent;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout;
    QGridLayout *gridLayout_8;
    QFrame *frame_3;
    QGridLayout *gridLayout_6;
    QLabel *label_6;
    QLabel *exec_type;
    QLabel *label_3;
    QLabel *loaded_label;
    QLabel *label_4;
    QLabel *num_queries_label;
    QLabel *label_2;
    QLabel *label_7;
    QLabel *wall_time_label;
    QLabel *label_8;
    QLabel *num_parallel_label;
    QLabel *num_distrib_label;
    QPushButton *execButton;
    QPushButton *browseButton;
    QComboBox *qList;
    QLineEdit *queryPath;
    QCheckBox *enableAdaptivity;
    QGroupBox *resultBox;
    QGridLayout *gridLayout_5;
    QTableWidget *resultTable;
    QLabel *gnuLabel;
    QWidget *comp_tab;
    QGridLayout *gridLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_13;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_10;
    QLabel *test2;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_9;
    QLabel *test2_2;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_11;
    QLabel *test2_3;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_12;
    QLabel *test2_4;
    QPushButton *pushButton;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MasterGUI)
    {
        if (MasterGUI->objectName().isEmpty())
            MasterGUI->setObjectName(QStringLiteral("MasterGUI"));
        MasterGUI->resize(1250, 819);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MasterGUI->sizePolicy().hasHeightForWidth());
        MasterGUI->setSizePolicy(sizePolicy);
        centralWidget = new QWidget(MasterGUI);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        frame_2 = new QFrame(frame);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(frame_2);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        label->setPixmap(QPixmap(QString::fromUtf8("../../images/header-kaust.jpg")));

        horizontalLayout->addWidget(label);

        label_5 = new QLabel(frame_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        QFont font;
        font.setPointSize(14);
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_5);


        verticalLayout_2->addWidget(frame_2);

        tabs = new QTabWidget(frame);
        tabs->setObjectName(QStringLiteral("tabs"));
        main_tab = new QWidget();
        main_tab->setObjectName(QStringLiteral("main_tab"));
        gridLayout_4 = new QGridLayout(main_tab);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        frame_5 = new QFrame(main_tab);
        frame_5->setObjectName(QStringLiteral("frame_5"));
        sizePolicy1.setHeightForWidth(frame_5->sizePolicy().hasHeightForWidth());
        frame_5->setSizePolicy(sizePolicy1);
        QPalette palette;
        QBrush brush(QColor(0, 255, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush);
        frame_5->setPalette(palette);
        frame_5->setAutoFillBackground(false);
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame_5);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        groupBox_4 = new QGroupBox(frame_5);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy3);
        groupBox_4->setAutoFillBackground(true);
        gridLayout_7 = new QGridLayout(groupBox_4);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        fileContent = new QPlainTextEdit(groupBox_4);
        fileContent->setObjectName(QStringLiteral("fileContent"));
        fileContent->setEnabled(true);
        QPalette palette1;
        QBrush brush1(QColor(0, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        fileContent->setPalette(palette1);
        fileContent->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        fileContent->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        fileContent->setLineWrapMode(QPlainTextEdit::NoWrap);
        fileContent->setReadOnly(true);
        fileContent->setBackgroundVisible(false);

        gridLayout_7->addWidget(fileContent, 0, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_4, 0, 1, 1, 1);

        groupBox_2 = new QGroupBox(frame_5);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy4(QSizePolicy::Maximum, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy4);
        groupBox_2->setAutoFillBackground(true);
        formLayout = new QFormLayout(groupBox_2);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        gridLayout_8 = new QGridLayout();
        gridLayout_8->setSpacing(6);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        frame_3 = new QFrame(groupBox_2);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        gridLayout_6 = new QGridLayout(frame_3);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label_6 = new QLabel(frame_3);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_6->addWidget(label_6, 2, 0, 1, 1);

        exec_type = new QLabel(frame_3);
        exec_type->setObjectName(QStringLiteral("exec_type"));
        QPalette palette2;
        QBrush brush2(QColor(170, 0, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        QBrush brush3(QColor(190, 190, 190, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        exec_type->setPalette(palette2);

        gridLayout_6->addWidget(exec_type, 2, 1, 1, 1);

        label_3 = new QLabel(frame_3);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_6->addWidget(label_3, 1, 0, 1, 1);

        loaded_label = new QLabel(frame_3);
        loaded_label->setObjectName(QStringLiteral("loaded_label"));

        gridLayout_6->addWidget(loaded_label, 0, 1, 1, 1);

        label_4 = new QLabel(frame_3);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_6->addWidget(label_4, 0, 0, 1, 1);

        num_queries_label = new QLabel(frame_3);
        num_queries_label->setObjectName(QStringLiteral("num_queries_label"));

        gridLayout_6->addWidget(num_queries_label, 1, 1, 1, 1);

        label_2 = new QLabel(frame_3);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_6->addWidget(label_2, 5, 0, 1, 1);

        label_7 = new QLabel(frame_3);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_6->addWidget(label_7, 3, 0, 1, 1);

        wall_time_label = new QLabel(frame_3);
        wall_time_label->setObjectName(QStringLiteral("wall_time_label"));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette3.setBrush(QPalette::Active, QPalette::Text, brush2);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette3.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette3.setBrush(QPalette::Disabled, QPalette::Text, brush3);
        wall_time_label->setPalette(palette3);
        QFont font1;
        font1.setPointSize(9);
        wall_time_label->setFont(font1);

        gridLayout_6->addWidget(wall_time_label, 5, 1, 1, 1);

        label_8 = new QLabel(frame_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_6->addWidget(label_8, 4, 0, 1, 1);

        num_parallel_label = new QLabel(frame_3);
        num_parallel_label->setObjectName(QStringLiteral("num_parallel_label"));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette4.setBrush(QPalette::Active, QPalette::Text, brush2);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette4.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette4.setBrush(QPalette::Disabled, QPalette::Text, brush3);
        num_parallel_label->setPalette(palette4);

        gridLayout_6->addWidget(num_parallel_label, 3, 1, 1, 1);

        num_distrib_label = new QLabel(frame_3);
        num_distrib_label->setObjectName(QStringLiteral("num_distrib_label"));
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette5.setBrush(QPalette::Active, QPalette::Text, brush2);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette5.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette5.setBrush(QPalette::Disabled, QPalette::Text, brush3);
        num_distrib_label->setPalette(palette5);

        gridLayout_6->addWidget(num_distrib_label, 4, 1, 1, 1);


        gridLayout_8->addWidget(frame_3, 3, 0, 1, 2);

        execButton = new QPushButton(groupBox_2);
        execButton->setObjectName(QStringLiteral("execButton"));
        execButton->setEnabled(false);
        QSizePolicy sizePolicy5(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(execButton->sizePolicy().hasHeightForWidth());
        execButton->setSizePolicy(sizePolicy5);
        execButton->setLayoutDirection(Qt::LeftToRight);

        gridLayout_8->addWidget(execButton, 1, 1, 1, 1);

        browseButton = new QPushButton(groupBox_2);
        browseButton->setObjectName(QStringLiteral("browseButton"));
        sizePolicy5.setHeightForWidth(browseButton->sizePolicy().hasHeightForWidth());
        browseButton->setSizePolicy(sizePolicy5);

        gridLayout_8->addWidget(browseButton, 0, 1, 1, 1);

        qList = new QComboBox(groupBox_2);
        qList->setObjectName(QStringLiteral("qList"));
        sizePolicy1.setHeightForWidth(qList->sizePolicy().hasHeightForWidth());
        qList->setSizePolicy(sizePolicy1);

        gridLayout_8->addWidget(qList, 1, 0, 1, 1);

        queryPath = new QLineEdit(groupBox_2);
        queryPath->setObjectName(QStringLiteral("queryPath"));
        queryPath->setEnabled(false);
        QSizePolicy sizePolicy6(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(100);
        sizePolicy6.setHeightForWidth(queryPath->sizePolicy().hasHeightForWidth());
        queryPath->setSizePolicy(sizePolicy6);
        queryPath->setMinimumSize(QSize(100, 0));
        queryPath->setBaseSize(QSize(0, 0));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::WindowText, brush1);
        palette6.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        queryPath->setPalette(palette6);
        queryPath->setAcceptDrops(false);

        gridLayout_8->addWidget(queryPath, 0, 0, 1, 1);

        enableAdaptivity = new QCheckBox(groupBox_2);
        enableAdaptivity->setObjectName(QStringLiteral("enableAdaptivity"));
        enableAdaptivity->setChecked(false);

        gridLayout_8->addWidget(enableAdaptivity, 2, 0, 1, 1);


        formLayout->setLayout(0, QFormLayout::SpanningRole, gridLayout_8);


        gridLayout_2->addWidget(groupBox_2, 0, 0, 1, 1);


        gridLayout_4->addWidget(frame_5, 0, 0, 1, 1);

        resultBox = new QGroupBox(main_tab);
        resultBox->setObjectName(QStringLiteral("resultBox"));
        resultBox->setAutoFillBackground(true);
        gridLayout_5 = new QGridLayout(resultBox);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        resultTable = new QTableWidget(resultBox);
        resultTable->setObjectName(QStringLiteral("resultTable"));
        resultTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        gridLayout_5->addWidget(resultTable, 0, 1, 1, 1);

        gnuLabel = new QLabel(resultBox);
        gnuLabel->setObjectName(QStringLiteral("gnuLabel"));
        gnuLabel->setAlignment(Qt::AlignCenter);

        gridLayout_5->addWidget(gnuLabel, 0, 0, 1, 1);


        gridLayout_4->addWidget(resultBox, 1, 0, 1, 1);

        tabs->addTab(main_tab, QString());
        comp_tab = new QWidget();
        comp_tab->setObjectName(QStringLiteral("comp_tab"));
        gridLayout = new QGridLayout(comp_tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        scrollArea = new QScrollArea(comp_tab);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1188, 574));
        gridLayout_13 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy7(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy7);
        gridLayout_10 = new QGridLayout(groupBox);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        test2 = new QLabel(groupBox);
        test2->setObjectName(QStringLiteral("test2"));

        gridLayout_10->addWidget(test2, 0, 0, 1, 1);


        gridLayout_13->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        sizePolicy7.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy7);
        gridLayout_9 = new QGridLayout(groupBox_3);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        test2_2 = new QLabel(groupBox_3);
        test2_2->setObjectName(QStringLiteral("test2_2"));

        gridLayout_9->addWidget(test2_2, 0, 0, 1, 1);


        gridLayout_13->addWidget(groupBox_3, 0, 1, 1, 1);

        groupBox_5 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        sizePolicy7.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy7);
        gridLayout_11 = new QGridLayout(groupBox_5);
        gridLayout_11->setSpacing(6);
        gridLayout_11->setContentsMargins(11, 11, 11, 11);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        test2_3 = new QLabel(groupBox_5);
        test2_3->setObjectName(QStringLiteral("test2_3"));

        gridLayout_11->addWidget(test2_3, 0, 0, 1, 1);


        gridLayout_13->addWidget(groupBox_5, 1, 0, 1, 1);

        groupBox_6 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        sizePolicy7.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy7);
        gridLayout_12 = new QGridLayout(groupBox_6);
        gridLayout_12->setSpacing(6);
        gridLayout_12->setContentsMargins(11, 11, 11, 11);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        test2_4 = new QLabel(groupBox_6);
        test2_4->setObjectName(QStringLiteral("test2_4"));

        gridLayout_12->addWidget(test2_4, 0, 0, 1, 1);


        gridLayout_13->addWidget(groupBox_6, 1, 1, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 1);

        tabs->addTab(comp_tab, QString());

        verticalLayout_2->addWidget(tabs);


        gridLayout_3->addWidget(frame, 0, 0, 1, 1);

        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout_3->addWidget(pushButton, 1, 0, 1, 1);

        MasterGUI->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MasterGUI);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        statusBar->setLayoutDirection(Qt::LeftToRight);
        MasterGUI->setStatusBar(statusBar);

        retranslateUi(MasterGUI);

        tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MasterGUI);
    } // setupUi

    void retranslateUi(QMainWindow *MasterGUI)
    {
        MasterGUI->setWindowTitle(QApplication::translate("MasterGUI", "MasterGUI", 0));
        label->setText(QString());
        label_5->setText(QApplication::translate("MasterGUI", "Evaluating SPARQL Queries on Massive RDF Datasets", 0));
        groupBox_4->setTitle(QApplication::translate("MasterGUI", "Query file content:", 0));
        fileContent->setPlainText(QString());
        groupBox_2->setTitle(QApplication::translate("MasterGUI", "Select a query file:", 0));
        label_6->setText(QApplication::translate("MasterGUI", "- Execution type: ", 0));
        exec_type->setText(QString());
        label_3->setText(QApplication::translate("MasterGUI", "- Number of queries in file: ", 0));
        loaded_label->setText(QString());
        label_4->setText(QApplication::translate("MasterGUI", "- Number of loaded queries: ", 0));
        num_queries_label->setText(QString());
        label_2->setText(QApplication::translate("MasterGUI", "- Wall Time: ", 0));
        label_7->setText(QApplication::translate("MasterGUI", "- Parallel Joins:", 0));
        wall_time_label->setText(QString());
        label_8->setText(QApplication::translate("MasterGUI", "- Distributed Joins:", 0));
        num_parallel_label->setText(QString());
        num_distrib_label->setText(QString());
        execButton->setText(QApplication::translate("MasterGUI", "Execute", 0));
        browseButton->setText(QApplication::translate("MasterGUI", "Browse", 0));
        enableAdaptivity->setText(QApplication::translate("MasterGUI", "Enable Adaptivity", 0));
        resultBox->setTitle(QApplication::translate("MasterGUI", "Results", 0));
        gnuLabel->setText(QString());
        tabs->setTabText(tabs->indexOf(main_tab), QString());
        groupBox->setTitle(QApplication::translate("MasterGUI", "GroupBox", 0));
        test2->setText(QApplication::translate("MasterGUI", "TextLabel", 0));
        groupBox_3->setTitle(QApplication::translate("MasterGUI", "GroupBox", 0));
        test2_2->setText(QApplication::translate("MasterGUI", "TextLabel", 0));
        groupBox_5->setTitle(QApplication::translate("MasterGUI", "GroupBox", 0));
        test2_3->setText(QApplication::translate("MasterGUI", "TextLabel", 0));
        groupBox_6->setTitle(QApplication::translate("MasterGUI", "GroupBox", 0));
        test2_4->setText(QApplication::translate("MasterGUI", "TextLabel", 0));
        tabs->setTabText(tabs->indexOf(comp_tab), QApplication::translate("MasterGUI", "Comparison", 0));
        pushButton->setText(QApplication::translate("MasterGUI", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class MasterGUI: public Ui_MasterGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MASTERGUI_H
