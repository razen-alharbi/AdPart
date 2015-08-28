#include "mgmt/clustermanager.h"
#include "ui_clustermanager.h"
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
ClusterManager::ClusterManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClusterManager)
{
    ui->setupUi(this);
    this->ui->addButton->setFocus();
    this->manager = (AdHashMgmt*)parent;
    map<string, ClusterConfiguration>::iterator it;
    for(it = manager->configurations.begin(); it != manager->configurations.end(); it++){
        ui->confList->addItem(QString(it->first.c_str()));
    }
    if(ui->confList->count() > 0)
        ui->confList->setCurrentRow(0);
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addConf()));
    connect(ui->remove, SIGNAL(clicked()), this, SLOT(removeConf()));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->testButton, SIGNAL(clicked()), this, SLOT(testConn()));
}

ClusterManager::~ClusterManager()
{
    delete ui;
}

void ClusterManager::removeConf(){
    if(ui->confList->currentRow() != -1){
        string conf_to_remove = ui->confList->currentItem()->text().toStdString();
        string conf_file_name = manager->adhash_home+"/conf/"+conf_to_remove;

        for(map<string, Database>::iterator it = manager->databases.begin(); it != manager->databases.end() ; it++){
            if(it->second.usesClusterConfiguration(conf_to_remove)){
                QMessageBox::critical(NULL, QObject::tr("Cluster Used"),
                                      QObject::tr("The cluster is used by some data configurations. Delete the data first!"),
                                      QMessageBox::Ok, QMessageBox::Ok);
                return ;
            }
        }


        if(remove(conf_file_name.c_str() ) != 0 ){
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Could not delete cluster configuration!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
        }
        else{
            ui->confList->takeItem(ui->confList->currentRow());
            manager->configurations.erase(conf_to_remove);
        }

    }
}

void ClusterManager::reject(){
    this->done(1);
}

void ClusterManager::addConf(){
    ClusterDialog b(this) ;
    b.setFixedSize(b.width(), b.height());
    b.exec();
    if(b.result()){
        ui->confList->clear();
        map<string, ClusterConfiguration>::iterator it;
        for(it = manager->configurations.begin(); it != manager->configurations.end(); it++){
            ui->confList->addItem(QString(it->first.c_str()));
        }
        if(ui->confList->count() > 0)
            ui->confList->setCurrentRow(0);
    }
}

void ClusterManager::testConn(){
    if(ui->confList->currentRow() != -1){
        string conf_to_test = ui->confList->currentItem()->text().toStdString();
        TestConnectivity b(this, this->manager->configurations[conf_to_test]) ;
        b.setFixedSize(b.width(), b.height());
        b.exec();
    }
}
