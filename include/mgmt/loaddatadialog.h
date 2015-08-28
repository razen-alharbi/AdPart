#ifndef LOADDATADIALOG_H
#define LOADDATADIALOG_H

#include "common/utils.h"
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
namespace Ui {
class LoadDataDialog;
}

class LoadDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadDataDialog(QWidget *parent = 0);
    ~LoadDataDialog();
    QString inputFile;
    QString databaseDirectory;
    QString dbName;
    QString queryPath;
    QString cachedPath;
private:
    Ui::LoadDataDialog *ui;

private slots:
    void browseFile();
    void browseDirectory();
    void browseDirectory2();
    void accept();
};

#endif // LOADDATADIALOG_H
