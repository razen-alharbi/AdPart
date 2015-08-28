#include "mgmt/partitioningdialog.h"
#include "ui_partitioningdialog.h"
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
PartitioningDialog::PartitioningDialog(QWidget *parent, Database &db):
    QDialog(parent),
    ui(new Ui::PartitioningDialog)
{
    this->db = &db;
    ui->setupUi(this);
    AdHashMgmt * manager = (AdHashMgmt*)parent;
    for(map<string, ClusterConfiguration>::iterator it = manager->configurations.begin(); it != manager->configurations.end(); it++){
        this->ui->clusterList->addItem(QString(it->first.c_str()));
    }
    connect(&m_futureWatcher, SIGNAL(finished()), this, SLOT(donePartitioning()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(startPartitioning()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

PartitioningDialog::~PartitioningDialog()
{
    delete ui;
}

void PartitioningDialog::startPartitioning(){
    QString cluster_conf = this->ui->clusterList->currentText();
    int num_parts = this->ui->numParts->value();


    if(!this->db->conf_exists(cluster_conf, num_parts)){
        this->ui->buttonBox->setEnabled(false);
        future = QtConcurrent::run(this, &PartitioningDialog::partition);
        m_futureWatcher.setFuture(future);
    }
    else{
        QMessageBox::critical(NULL, QObject::tr("Configuration Exists"),
                              QObject::tr("A partitioning with the same configuration already exists!"),
                              QMessageBox::Ok, QMessageBox::Ok);
    }


}

QString PartitioningDialog::partition(){
    Profiler prof;
    prof.startTimer("part");
    char buffer[1024];
    char *line_p;
    string command;
    FILE * fp;
    string inputFileName = db->inputFile.substr(db->inputFile.find_last_of('/')+1, db->inputFile.size());
    AdHashMgmt * caller = (AdHashMgmt*)this->parent();
    string result;
    string num_proc = toString(this->ui->numParts->value()+1);
    string machine_file = caller->adhash_home+"/conf/"+this->ui->clusterList->currentText().toStdString();
    string executable = caller->adhash_home+"/Release/parallel_partitioner";
    command = "mpirun -np "+num_proc+" -machinefile "+machine_file+" "+executable+" "+db->directory+" "+inputFileName;
    fp = popen(command.c_str(), "r");
    if (!fp)
    {
        return QString("error");
    }

    while((line_p = fgets(buffer, sizeof(buffer), fp))!=NULL){
        result = string(line_p);
    }
    pclose(fp);
    prof.pauseTimer("part");

    if(result != "failed"){
        result = "Partitioning Time: "+toString(prof.readPeriod("part"))+" seconds";
    }
    prof.clearTimer("part");
    return QString(result.c_str());
}

void PartitioningDialog::donePartitioning(){
    QString result = future.result();
    if(result != "failed"){
        cluster_conf = this->ui->clusterList->currentText();
        num_parts = this->ui->numParts->value();
        this->db->parts.push_back(part_conf(cluster_conf, num_parts));
        QMessageBox::information(this, QObject::tr("Partitioning Successful"), result, QMessageBox::Ok, QMessageBox::Ok);
        this->done(1);
    }
    else{
        QMessageBox::critical(this, QObject::tr("AdHash failed to create partition folders"), QObject::tr("Make sure you have write permission in all hosts!"), QMessageBox::Ok, QMessageBox::Ok);
        this->done(0);
    }
}
