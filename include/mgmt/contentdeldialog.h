#ifndef CONTENTDELDIALOG_H
#define CONTENTDELDIALOG_H


#include <QDialog>
#include <QThread>
#include "mgmt/database.h"
#include "common/utils.h"
#include "mgmt/cluster_configuration.h"
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
class ContentDelDialog;
}

class ContentDelDialog : public QDialog
{
    Q_OBJECT
    QThread workerThread;
public:
    ContentDelDialog(QWidget *parent, Database *db, vector<ClusterConfiguration *> &confs, int num_parts);
    ~ContentDelDialog();
    Ui::ContentDelDialog *ui;


private slots:
    void reject();
    void updatePB(int v);
    void updateStatusLabel(QString msg);

signals:
    void operate();
};

class ContentDeleter : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    ContentDeleter(ContentDelDialog * caller, Database *db, vector<ClusterConfiguration *> &confs, int num_parts);
    ContentDelDialog *caller;
private:
    Database *db;
    vector<ClusterConfiguration *> confs;
    int num_parts;
public slots:
    void deleteContent();
    void deleteSinglePartition();
    void deleteDatabase();
    void deleteDataFromMaster();
signals:
    void updateProgress(int m);
    void updateStatus(QString msg);
};
#endif // CONTENTDELDIALOG_H
