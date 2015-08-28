#ifndef BROWSEDIALOG_H
#define BROWSEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>
#include <sys/stat.h>
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
class BrowseDialog;
}

class BrowseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BrowseDialog(QWidget *parent = 0);
    ~BrowseDialog();
    QString path;

private:
    Ui::BrowseDialog *ui;

private slots:
    void browse();
    void accept();
};

#endif // BROWSEDIALOG_H
