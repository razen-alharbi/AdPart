#ifndef CLUSTERDIALOG_H
#define CLUSTERDIALOG_H

#include <QDialog>
#include <iostream>
#include <QPushButton>
#include <QStatusBar>
#include <QProgressBar>
#include "mgmt/adhashmgmt.h"
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
using namespace std;
namespace Ui {
class ClusterDialog;
}

class ClusterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClusterDialog(QWidget *parent = 0);
    ~ClusterDialog();

private:
    Ui::ClusterDialog *ui;

private slots:
    void accept();
    void reject();
};

#endif // CLUSTERDIALOG_H
