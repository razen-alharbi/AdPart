#ifndef TESTCONNECTIVITY_H
#define TESTCONNECTIVITY_H

#include <QDialog>
#include <QMovie>
#include <iostream>
#include "mgmt/adhashmgmt.h"
#include <map>
#include <QThread>
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
class AdHashMgmt;
using namespace std;
namespace Ui {
class TestConnectivity;
}

class TestConnectivity : public QDialog
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit TestConnectivity(QWidget *parent = 0);
    TestConnectivity(QWidget *parent, ClusterConfiguration &c);
    ~TestConnectivity();
    map<QMovie*, int> myMovies;
    Ui::TestConnectivity *ui;
private:
    AdHashMgmt * manager;
    ClusterConfiguration conf;
    int total;
    QPushButton * doneButton;

private slots:
    void setButtonIcon(int frame);
    void successful(QMovie* m);
    void failed(QMovie* m);
    void reject();
    void accept();
signals:
    void operate();
};


class Worker : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:

    Worker(TestConnectivity * caller);
    TestConnectivity *caller;
public slots:
    void test_connections();
signals:
    void host_connected(QMovie* m);
    void host_disconnected(QMovie* m);
};

#endif // TESTCONNECTIVITY_H
