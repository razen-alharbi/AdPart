#include "mgmt/contentdeldialog.h"
#include "ui_contentdeldialog.h"
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
ContentDelDialog::ContentDelDialog(QWidget *parent, Database *db, vector<ClusterConfiguration *> &confs, int num_parts) :
    QDialog(parent),
    ui(new Ui::ContentDelDialog)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));

    ContentDeleter * deleter = new ContentDeleter(this, db, confs, num_parts);
    deleter->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), deleter, SLOT(deleteLater()));
    connect(this, SIGNAL(operate()), deleter, SLOT(deleteContent()));
    connect(deleter, SIGNAL(updateProgress(int)), this, SLOT(updatePB(int)));
    connect(deleter, SIGNAL(updateStatus(QString)), this, SLOT(updateStatusLabel(QString)));
    workerThread.start();
    emit operate();
}

ContentDelDialog::~ContentDelDialog()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

void ContentDelDialog::reject(){
    workerThread.quit();
    workerThread.wait();
    this->done(0);
}

void ContentDelDialog::updatePB(int v){
    if(v == -1){
        this->done(0);
        return;
    }
    if(v == 1000){
        this->done(1);
        return;
    }
    this->ui->progressBar->setValue(v);
}

void ContentDelDialog::updateStatusLabel(QString msg){
    this->ui->groupBox->setTitle(msg);
}

ContentDeleter::ContentDeleter(ContentDelDialog * caller, Database *db, vector<ClusterConfiguration *> &confs, int num_parts){
    this->caller = caller;
    this->db = db;
    this->confs = confs;
    this->num_parts = num_parts;
}

void ContentDeleter::deleteContent(){
    if(num_parts != -1){//delete single partition
        this->deleteSinglePartition();
    }
    else{
        this->deleteDatabase();
    }
}

void ContentDeleter::deleteSinglePartition(){
    QString path = QString(this->db->parts_folder.c_str())+QString::number(this->num_parts);
    this->caller->ui->progressBar->setRange(0,100);
    char buffer[1024];
    char *line_p;
    string command;
    FILE * fp;
    int progress;
    QString msg;

    for(unsigned int i = 0 ; i < confs[0]->hosts.size(); i++){
        msg = "Deleting contents from "+QString(confs[0]->hosts[i].c_str())+"...";
        emit updateStatus(msg);
        command = "ssh -o ConnectTimeout=5 "+confs[0]->hosts[i]+" \"rm -r "+path.toStdString()+"\"";
        fp = popen(command.c_str(), "r");

        if (!fp)
        {
            emit updateProgress(-1);
        }

        line_p = fgets(buffer, sizeof(buffer), fp);
        cout<<line_p<<endl;
        progress = (int)(1.0*(i+1)/confs[0]->hosts.size()*100);
        emit updateProgress(progress);
        pclose(fp);
    }
    emit updateProgress(1000);
}

void ContentDeleter::deleteDatabase(){
    char buffer[1024];
    char *line_p;
    string command;
    FILE * fp;
    int progress;
    QString msg;
    int total_hosts = 0, counter = 0;

    for(unsigned int j = 0 ; j < confs.size(); j++){
        total_hosts += confs[j]->hosts.size()-1;
    }
    this->deleteDataFromMaster();
    if(confs.size() == 0){
        emit updateProgress(1000);
        return;
    }
    this->caller->ui->progressBar->setRange(0,100);
    for(unsigned int j = 0 ; j < confs.size(); j++){
        for(unsigned int i = 1 ; i < confs[j]->hosts.size(); i++){
            counter++;
            msg = "Deleting contents from "+QString(confs[j]->hosts[i].c_str())+"...";
            emit updateStatus(msg);
            command = "ssh -o ConnectTimeout=5 "+confs[j]->hosts[i]+" echo \"rm -r "+db->parts_folder+"\"";
            fp = popen(command.c_str(), "r");

            if (!fp)
            {
                emit updateProgress(-1);
            }

            line_p = fgets(buffer, sizeof(buffer), fp);
            cout<<line_p<<endl;
            progress = (int)(1.0*counter/total_hosts*100);
            emit updateProgress(progress);
            pclose(fp);
        }
    }
    emit updateProgress(1000);
}

void ContentDeleter::deleteDataFromMaster(){
    QString hostname;
    char buffer[1024];
    char *line_p;
    string command;
    FILE * fp;
    if(confs.size() == 0){
        char buf[32];
        if (gethostname(buf, 32) == -1 ) {
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Could not get host name. Local content could not be deleted!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
        }
        else{
            hostname = QString(buf);
        }
    }
    else{
        hostname = QString(confs[0]->hosts[0].c_str());
    }

    emit updateStatus("Deleting database contents from "+hostname+" (master)...");

    command = "rm -r "+db->directory;
    fp = popen(command.c_str(), "r");
    if (!fp){
        emit updateProgress(-1);
    }
    line_p = fgets(buffer, sizeof(buffer), fp);
    cout<<line_p<<endl;
    pclose(fp);
}
