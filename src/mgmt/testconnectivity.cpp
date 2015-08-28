#include "mgmt/testconnectivity.h"
#include "ui_testconnectivity.h"
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
Worker::Worker(TestConnectivity * caller){
    this->caller = caller;
}

void Worker::test_connections(){
    map<int, QMovie*> inv;
    for(map<QMovie*, int>::iterator i = caller->myMovies.begin() ; i != caller->myMovies.end(); i++){
        inv[i->second] = i->first;
    }
    char buffer[1024];
    char *line_p;
    string command;
    FILE * fp;
    for(map<int, QMovie*>::iterator i = inv.begin() ; i != inv.end(); i++){

        command = "ssh -o ConnectTimeout=5 "+caller->ui->listWidget->item(i->first)->text().toStdString()+" hostname";
        fp = popen(command.c_str(), "r");

        if (!fp)
        {
            emit host_disconnected(i->second);
        }

        line_p = fgets(buffer, sizeof(buffer), fp);
        if(line_p){
            emit host_connected(i->second);
        }
        else{
             emit host_disconnected(i->second);
        }
        pclose(fp);
    }
}
TestConnectivity::TestConnectivity(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestConnectivity)
{
    ui->setupUi(this);
}

TestConnectivity::TestConnectivity(QWidget *parent, ClusterConfiguration &conf) :
    QDialog(parent),
    ui(new Ui::TestConnectivity)
{
    ui->setupUi(this);
    this->doneButton = new QPushButton(tr("Done"));
    ui->buttonBox->addButton(this->doneButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->setVisible(false);
    this->manager = ((ClusterManager*)(this->parent()))->manager;
    this->conf = conf;
    QMovie * tmpMovie;
    for(unsigned int i = 0 ; i < this->conf.hosts.size(); i++){
        ui->listWidget->addItem(new QListWidgetItem(tr(this->conf.hosts[i].c_str())));
        tmpMovie = new QMovie(QString(this->manager->adhash_home.c_str())+"/images/loading.gif");
        myMovies[tmpMovie] = i;
        connect(tmpMovie,SIGNAL(frameChanged(int)),this,SLOT(setButtonIcon(int)));
    }
    // if movie doesn't loop forever, force it to.
    for(map<QMovie*, int>::iterator i = myMovies.begin() ; i != myMovies.end(); i++){
        if (i->first->loopCount() != -1)
            connect(i->first,SIGNAL(finished()),i->first,SLOT(start()));
        i->first->start();
    }

    total = ui->listWidget->count();
    Worker * worker = new Worker(this);
    worker->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(operate()), worker, SLOT(test_connections()));
    connect(worker, SIGNAL(host_connected(QMovie*)), this, SLOT(successful(QMovie*)));
    connect(worker, SIGNAL(host_disconnected(QMovie*)), this, SLOT(failed(QMovie*)));
    connect(this, SIGNAL(rejected()), &workerThread, SLOT(quit()));
    connect(doneButton, SIGNAL(clicked()), this, SLOT(accept()));
    workerThread.start();
    emit operate();
}

TestConnectivity::~TestConnectivity()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

void TestConnectivity::setButtonIcon(int){
    QMovie* caller = (QMovie*)QObject::sender();
    int id = this->myMovies[caller];
    if(ui->listWidget->count() > 0){
        ui->listWidget->item(id)->setIcon(QIcon(caller->currentPixmap()));
    }
}

void TestConnectivity::successful(QMovie * m){
    total--;
    int id = myMovies[m];
    m->stop();
    ui->listWidget->item(id)->setIcon(QIcon(QString(this->manager->adhash_home.c_str())+"/images/check.png"));
    this->conf.up_hosts.push_back(ui->listWidget->item(id)->text().toStdString());
    if(total == 0){
        this->ui->buttonBox->setVisible(true);
    }
}

void TestConnectivity::failed(QMovie * m){
    total--;
    int id = myMovies[m];
    m->stop();
    ui->listWidget->item(id)->setIcon(QIcon(QString(this->manager->adhash_home.c_str())+"/images/failed.png"));
    this->conf.down_hosts.push_back(ui->listWidget->item(id)->text().toStdString());
    if(total == 0){
        this->ui->buttonBox->setVisible(true);
    }
}

void TestConnectivity::reject(){
}

void TestConnectivity::accept(){
    this->done(1);
}
