#include "mgmt/clusterdialog.h"
#include "ui_clusterdialog.h"
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
ClusterDialog::ClusterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClusterDialog)
{
    ui->setupUi(this);
    this->setResult(0);
    ui->confName->setFocus();
    QPushButton * okButton = new QPushButton(tr("&OK"));
    okButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
    cancelButton->setCheckable(true);
    cancelButton->setAutoDefault(false);

    ui->buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

ClusterDialog::~ClusterDialog()
{
    delete ui;
}

void ClusterDialog::accept(){
    AdHashMgmt* mgmt = ((ClusterManager*)(this->parent()))->manager;
    string conf_name = ui->confName->text().trimmed().toStdString();
    QString localHost;
    if(!conf_name.empty()){
        if(mgmt->configurations.find(conf_name) == mgmt->configurations.end()){
            QString all = ui->clusterEditor->toPlainText().trimmed();
            if(!all.isEmpty()){
                QStringList splits = all.split("\n");
                char buf[32];
                if (gethostname(buf, 32) == -1 ) {
                    QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                          QObject::tr("Could not get local host name."),
                                          QMessageBox::Ok, QMessageBox::Ok);
                }
                localHost = QString(buf).trimmed().toLower();
                if(localHost != splits.at(0).trimmed().toLower() && splits.at(0).toLower().trimmed() != "localhost"){
                    QMessageBox::critical(NULL, QObject::tr("Master error"),
                                          QObject::tr("The master hostname does not match the local host name!"),
                                          QMessageBox::Ok, QMessageBox::Ok);
                }
                else{
                    string conf_file_name = mgmt->adhash_home+"/conf/"+conf_name;
                    ofstream cluster_stream(conf_file_name.c_str());
                    if (!cluster_stream.is_open()){
                        QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                              QObject::tr("Could not write cluster configuration!"),
                                              QMessageBox::Ok, QMessageBox::Ok);
                        return;
                    }
                    else{
                        ClusterConfiguration c = ClusterConfiguration(conf_name);
                        set<string> unique;
                        string hostName;
                        string command;
                        FILE * fp;
                        AdHashMgmt * manager = (AdHashMgmt*)this->parent()->parent();
                        string path = manager->adhash_home.substr(0, manager->adhash_home.find_last_of('/')+1);
                        for(int i = 0 ; i < splits.size() ; i++){
                            hostName = splits.at(i).trimmed().toStdString();
                            if(!hostName.empty() && (unique.find(hostName) == unique.end())){
                                unique.insert(hostName);
                                command = "scp -r "+manager->adhash_home+" "+hostName+":"+path;

                                fp = popen(command.c_str(), "r");
                                if (!fp)
                                {
                                    QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                                          QObject::tr("Could not scp AdHash Home!"),
                                                          QMessageBox::Ok, QMessageBox::Ok);
                                }
                                pclose(fp);
                                c.hosts.push_back(hostName);
                                cluster_stream << hostName<<endl;
                            }
                        }
                        mgmt->configurations[conf_name] = c;
                        cluster_stream.close();
                    }
                    this->done(1);
                }
            }
        }
        else{
            QMessageBox::critical(NULL, QObject::tr("Configuration Exists"),
                                  QObject::tr("A configuration with the same name already exists!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
        }
    }
}

void ClusterDialog::reject(){
    this->done(0);
}
