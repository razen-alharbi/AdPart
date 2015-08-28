/********************************************************************************
** Form generated from reading UI file 'comparisontab.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPARISONTAB_H
#define UI_COMPARISONTAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ComparisonTab
{
public:

    void setupUi(QWidget *ComparisonTab)
    {
        if (ComparisonTab->objectName().isEmpty())
            ComparisonTab->setObjectName(QStringLiteral("ComparisonTab"));
        ComparisonTab->resize(400, 300);

        retranslateUi(ComparisonTab);

        QMetaObject::connectSlotsByName(ComparisonTab);
    } // setupUi

    void retranslateUi(QWidget *ComparisonTab)
    {
        ComparisonTab->setWindowTitle(QApplication::translate("ComparisonTab", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class ComparisonTab: public Ui_ComparisonTab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPARISONTAB_H
