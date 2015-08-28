#ifndef COMPARISONTAB_H
#define COMPARISONTAB_H

#include <QWidget>
#include "engine/mastergui.h"
//---------------------------------------------------------------------------
// AdHash
// (c) 2015 Razen M. AlHarbi. Web site: http://cloud.kaust.edu.sa/Pages/adhash.aspx
//
// This work is licensed under the Creative Commons
// Attribution-Noncommercial-Share Alike 3.0 Unported License. To view a copy
// of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300,
// San Francisco, California, 94105, USA.
//---------------------------------------------------------------------------
namespace Ui {
class ComparisonTab;
}

class ComparisonTab : public QWidget
{
    Q_OBJECT

public:
    explicit ComparisonTab(QWidget *parent = 0);
    ~ComparisonTab();

private:
    Ui::ComparisonTab *ui;
};

#endif // COMPARISONTAB_H
