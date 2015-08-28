#ifndef PARTITIONINGDIALOG_H
#define PARTITIONINGDIALOG_H

#include <QDialog>
#include "mgmt/adhashmgmt.h"
#include <QProcess>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include "common/profiler.h"
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
class PartitioningDialog;
}

class PartitioningDialog : public QDialog
{
    Q_OBJECT

public:
    PartitioningDialog(QWidget *parent, Database &db);
    ~PartitioningDialog();
    int num_parts;
    QString cluster_conf;

private:
    Ui::PartitioningDialog *ui;
    Database* db;
    QString partition();
    QFutureWatcher<QString> m_futureWatcher;
    QFuture<QString> future;

private slots:
    void startPartitioning();
    void donePartitioning();
};

#endif // PARTITIONINGDIALOG_H
