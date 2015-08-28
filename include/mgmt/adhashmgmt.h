#ifndef ADHASHMGMT_H
#define ADHASHMGMT_H

#include "common/utils.h"
#include "mgmt/browsedialog.h"
#include "mgmt/clusterdialog.h"
#include "mgmt/testconnectivity.h"
#include "mgmt/loaddatadialog.h"
#include "mgmt/database.h"
#include "mgmt/dataencodingdialog.h"
#include "mgmt/cluster_configuration.h"
#include "mgmt/clustermanager.h"
#include "mgmt/dbdeletedialog.h"
#include "mgmt/partitioningdialog.h"
#include "mgmt/confdeldialog.h"
#include "mgmt/contentdeldialog.h"
#include <QFuture>
#include <QFutureWatcher>

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
class AdHashMgmt;
}

class AdHashMgmt : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdHashMgmt(QWidget *parent = 0);
    ~AdHashMgmt();
    string adhash_home;
    map<string, Database> databases;
    map<string, ClusterConfiguration> configurations;
private:
    Ui::AdHashMgmt *ui;
    int resolve_adhash_home();
    int find_cluster_configuration(vector<string>& conf_names);
    void load_clusters_configurations(vector<string>& conf_names);
    void load_databases();
    void showEvent(QShowEvent*);
    bool displayed;
private slots:
    void load_data();
    void manage_clusters();
    void updateSummary();
    void databaseSelected();
    void configurationSelected();
    void deleteDB();
    void deleteConf();
    void partitionData();
    void launchEngine();
};

#endif // ADHASHMGMT_H
