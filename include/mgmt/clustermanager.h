#ifndef CLUSTERMANAGER_H
#define CLUSTERMANAGER_H

#include <QDialog>
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
class AdHashMgmt;
namespace Ui {
class ClusterManager;
}

class ClusterManager : public QDialog
{
    Q_OBJECT

public:
    explicit ClusterManager(QWidget *parent = 0);
    ~ClusterManager();
    AdHashMgmt *manager;

private:
    Ui::ClusterManager *ui;

private slots:
    void removeConf();
    void reject();
    void addConf();
    void testConn();
};

#endif // CLUSTERMANAGER_H
