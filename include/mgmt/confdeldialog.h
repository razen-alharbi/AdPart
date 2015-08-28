#ifndef CONFDELDIALOG_H
#define CONFDELDIALOG_H

#include <QDialog>
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
class ConfDelDialog;
}

class ConfDelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfDelDialog(QWidget *parent = 0);
    ~ConfDelDialog();
    bool deleteContent;

private:
    Ui::ConfDelDialog *ui;

private slots:
    void updateState();
    void accept();
};

#endif // CONFDELDIALOG_H
