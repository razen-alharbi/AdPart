#include "mgmt/adhashmgmt.h"
#include "ui_adhashmgmt.h"
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
AdHashMgmt::AdHashMgmt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdHashMgmt)
{
    ui->setupUi(this);
    this->displayed = false;
    QPushButton * loadButton = new QPushButton(tr("&Load New Data"));
    QPushButton * partButton = new QPushButton(tr("&Partition"));
    QPushButton * delButton = new QPushButton(tr("&Delete"));
    loadButton->setDefault(true);
    partButton->setDefault(false);
    delButton->setDefault(false);
    ui->buttonBox->addButton(loadButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(partButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(delButton, QDialogButtonBox::ActionRole);
    connect(loadButton, SIGNAL(clicked()), this, SLOT(load_data()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(deleteDB()));
    connect(partButton, SIGNAL(clicked()), this, SLOT(partitionData()));
    ui->buttonBox->buttons().at(1)->setVisible(false);
    ui->buttonBox->buttons().at(2)->setVisible(false);

    QPushButton * confDelButton = new QPushButton(tr("&Delete"));
    QPushButton * launchButton = new QPushButton(tr("&Launch"));
    this->ui->launcherBox->addButton(confDelButton, QDialogButtonBox::ActionRole);
    this->ui->launcherBox->addButton(launchButton, QDialogButtonBox::ActionRole);
    this->ui->frame_2->setVisible(false);
    connect(confDelButton, SIGNAL(clicked()), this, SLOT(deleteConf()));
    connect(launchButton, SIGNAL(clicked()), this, SLOT(launchEngine()));

    connect(ui->actionLoad_Data, SIGNAL(triggered()), this, SLOT(load_data()));
    connect(ui->actionManage_Clusters, SIGNAL(triggered()), this, SLOT(manage_clusters()));
    connect(ui->databasesList, SIGNAL(itemSelectionChanged()), this, SLOT(databaseSelected()));
    connect(ui->configurationsList, SIGNAL(itemSelectionChanged()), this, SLOT(configurationSelected()));
}

AdHashMgmt::~AdHashMgmt()
{
    delete ui;
}

int AdHashMgmt::resolve_adhash_home(){
    struct stat fileAtt;
    string adhash_meta_path = QDir::home().path().append("/.adhash").toStdString();
    string home_file_path = QDir::home().path().append("/.adhash/home").toStdString();

    if (stat(adhash_meta_path.c_str(), &fileAtt) != 0){
        int status = mkdir(adhash_meta_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(status){
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Could not create .adhash meta folder. AdHash will terminate!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
            return 0;
        }
    }

    if (stat(home_file_path.c_str(), &fileAtt) == 0){
        ifstream home_in_stream(home_file_path.c_str());
        if (!home_in_stream.is_open()){
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Could not open metadata file. AdHash will terminate!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
            return 0;
        }
        getline(home_in_stream, this->adhash_home);
        home_in_stream.close();
        return 1;
    }
    else{
        BrowseDialog b(this) ;
        b.setFixedSize(b.width(), b.height());
        b.exec();
        if(!b.result()){
            return 0;
        }
        else{
            this->adhash_home = b.path.toStdString();
            ofstream home_out_stream(home_file_path.c_str());
            if (!home_out_stream.is_open()){
                QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                      QObject::tr("Could not write metadata. AdHash will terminate!"),
                                      QMessageBox::Ok, QMessageBox::Ok);
                return 0;
            }
            else{
                string log_path = this->adhash_home+"/logs/";
                if (stat(log_path.c_str(), &fileAtt) != 0){
                    int status = mkdir(log_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                    if(status){
                        QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                              QObject::tr("Could not create log folder. AdHash will terminate!"),
                                              QMessageBox::Ok, QMessageBox::Ok);
                        return 0;
                    }
                }
                home_out_stream << this->adhash_home<<endl;
                home_out_stream.close();
                return 1;
            }
        }
    }
    return 0;
}

int AdHashMgmt::find_cluster_configuration(vector<string>& conf_names){
    struct stat fileAtt;
    string conf_dir = this->adhash_home+"/conf/";
    if (stat(conf_dir.c_str(), &fileAtt) != 0){
        int status = mkdir(conf_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(status){
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Could not create conf folder. AdHash will terminate!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
            return -1;
        }
        return 0;
    }
    else{
        DIR *dp;
        int i = 0;
        struct dirent *ep;
        dp = opendir(conf_dir.c_str());
        conf_names.clear();
        if (dp != NULL){
            while((ep = readdir (dp))){
                if(((int)ep->d_type) == 8){
                    i++;
                    conf_names.push_back(string(ep->d_name));
                }
            }
            closedir (dp);
        }
        else{
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Could not open conf folder. AdHash will terminate!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
            return -1;
        }
        return i;
    }
}

void AdHashMgmt::manage_clusters(){
    ClusterManager b(this) ;
    b.setFixedSize(b.width(), b.height());
    b.exec();
}

void AdHashMgmt::load_clusters_configurations(vector<string> &conf_names){
    ifstream cluster_stream;
    string cluster_conf, line;
    ClusterConfiguration c;
    for(unsigned i = 0 ; i < conf_names.size(); i++){
        cluster_conf = this->adhash_home+"/conf/"+conf_names[i];
        cluster_stream.open(cluster_conf.c_str());
        if (!cluster_stream.is_open()){
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Could not open cluster configuration file. AdHash will terminate!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
        }
        c = ClusterConfiguration(conf_names[i]);
        while(getline(cluster_stream, line)){
            c.hosts.push_back(line);
        }
        this->configurations[conf_names[i]] = c;
        cluster_stream.close();
    }
}

void AdHashMgmt::load_databases(){
    struct stat fileAtt;
    string databases_path = QDir::home().path().append("/.adhash/databases").toStdString();
    ifstream db_stream;
    Database db;
    string line;
    if (stat(databases_path.c_str(), &fileAtt) == 0){
        db_stream.open(databases_path.c_str());
        if (db_stream.is_open()){
            while(getline(db_stream, line)){
                db = Database(line);
                this->databases[db.name] = db;
                this->ui->databasesList->addItem(QString(db.name.c_str()));
            }
            if(ui->databasesList->count() > 0){
                this->ui->databasesList->setCurrentRow(0);
            }
        }
        db_stream.close();
    }
}

void AdHashMgmt::showEvent(QShowEvent *){
    if(!this->displayed){
        this->displayed = true;
        if(!resolve_adhash_home()){
            QApplication::quit();
        }
        ui->label->setPixmap(QPixmap(QString(string(this->adhash_home+"/images/header-kaust.jpg").c_str())));
        vector<string> conf_names;
        int num_conf = find_cluster_configuration(conf_names);
        switch(num_conf){
        case -1:
            QApplication::quit();
        case 0:
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Cluster Configuration", "No cluster configuration was found! Click Yes to configure it now or No to configure later..",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                this->manage_clusters();
            }
            break;
        default:
            load_clusters_configurations(conf_names);
            break;
        }
        this->load_databases();
    }
}

void AdHashMgmt::load_data(){
    LoadDataDialog ld_dialog(this) ;
    ld_dialog.setFixedSize(ld_dialog.width(), ld_dialog.height());
    ld_dialog.exec();
    if(ld_dialog.result()){
        Database db = Database(ld_dialog.dbName.toStdString(),
                               ld_dialog.inputFile.toStdString(),
                               ld_dialog.databaseDirectory.toStdString(),
                               ld_dialog.queryPath.toStdString());
        this->databases[ld_dialog.dbName.toStdString()] = db;
        struct stat fileAtt;
        int status;
        if (stat(db.directory.c_str(), &fileAtt) != 0){
            status = mkdir(db.directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if(status){
                QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                      QObject::tr("Could not create the database folder!"),
                                      QMessageBox::Ok, QMessageBox::Ok);
                return;
            }
        }
        if (stat(db.dic_folder.c_str(), &fileAtt) != 0){
            status = mkdir(db.dic_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if(status){
                QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                      QObject::tr("Could not create dictionary folder!"),
                                      QMessageBox::Ok, QMessageBox::Ok);
                return;
            }
        }
        if (stat(db.data_folder.c_str(), &fileAtt) != 0){
            status = mkdir(db.data_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if(status){
                QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                      QObject::tr("Could not create data folder!"),
                                      QMessageBox::Ok, QMessageBox::Ok);
                return;
            }
        }
        if (stat(db.parts_folder.c_str(), &fileAtt) != 0){
            status = mkdir(db.parts_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if(status){
                QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                      QObject::tr("Could not create data partitions folder!"),
                                      QMessageBox::Ok, QMessageBox::Ok);
                return;
            }
        }

        DataEncodingDialog encode_dialog(this, &this->databases[ld_dialog.dbName.toStdString()]) ;
        encode_dialog.setFixedSize(encode_dialog.width(), encode_dialog.height());
        encode_dialog.exec();
        if(encode_dialog.result()){
            this->databases[ld_dialog.dbName.toStdString()].encoded = 1;
            this->ui->databasesList->addItem(ld_dialog.dbName);
            ofstream databases_stream(QDir::home().path().append("/.adhash/databases").toStdString().c_str());
            map<string, Database>::iterator it;
            for(it = this->databases.begin(); it != this->databases.end(); it++){
                databases_stream<<it->second.print()<<endl;
            }
            databases_stream.close();
            if(this->ui->databasesList->count() != 0){
                this->ui->databasesList->setCurrentRow(this->ui->databasesList->count()-1);
            }
        }
    }
}

void AdHashMgmt::updateSummary(){
    if(this->ui->databasesList->currentRow() != -1){
        QString text = this->databases[this->ui->databasesList->currentItem()->text().toStdString()].printFormatted().c_str();
        if(this->ui->configurationsList->currentRow() != -1){
            QStringList list = this->ui->configurationsList->currentItem()->text().split(",");
            text += "\nCluster: "+list[0];
            text += "\n#Partitions: "+list[1];
        }
        this->ui->summaryText->setText(text);
    }
}

void AdHashMgmt::databaseSelected(){
    this->ui->configurationsList->clear();
    if(this->ui->databasesList->currentRow() != -1){
        Database * db = &this->databases[this->ui->databasesList->currentItem()->text().toStdString()];
        ui->buttonBox->buttons().at(0)->setVisible(true);
        ui->buttonBox->buttons().at(1)->setVisible(true);
        ui->buttonBox->buttons().at(2)->setVisible(true);
        if(db->parts.size() > 0){
            for(unsigned int i = 0 ; i < db->parts.size(); i++){
                this->ui->configurationsList->addItem(QString(db->parts[i].cluster_conf+","+QString::number(db->parts[i].num_parts)));
            }
            this->ui->configurationsList->setCurrentRow(0);
        }
        else{
            this->ui->frame_2->setVisible(false);
            this->updateSummary();
        }
    }
    else{
        ui->buttonBox->buttons().at(0)->setVisible(true);
        ui->buttonBox->buttons().at(1)->setVisible(false);
        ui->buttonBox->buttons().at(2)->setVisible(false);
        ui->configurationsList->clear();
        this->ui->frame_2->setVisible(false);
        this->ui->summaryText->clear();
    }
}

void AdHashMgmt::configurationSelected(){
    if(this->ui->configurationsList->currentRow() != -1){
        this->ui->frame_2->setVisible(true);
        this->updateSummary();
    }
    else{
        this->ui->frame_2->setVisible(false);
        this->updateSummary();
    }
}

void AdHashMgmt::deleteDB(){
    if(this->ui->databasesList->currentRow() != -1){
        DBDeleteDialog * d = new DBDeleteDialog(this);
        d->exec();
        if(d->result()){
            string dbName = this->ui->databasesList->currentItem()->text().toStdString();
            Database * db = &this->databases[dbName];
            if(d->deleteContent){
                vector<ClusterConfiguration *> confs;
                set<string> unique;
                for(unsigned int i = 0 ; i < db->parts.size() ; i++){
                    if(unique.find(db->parts[i].cluster_conf.toStdString()) == unique.end()){
                        unique.insert(db->parts[i].cluster_conf.toStdString());
                        confs.push_back(&this->configurations[db->parts[i].cluster_conf.toStdString()]);
                    }
                }

                ContentDelDialog * del = new ContentDelDialog(this, db, confs, -1);
                del->exec();
            }
            this->databases.erase(dbName);
            this->ui->databasesList->takeItem(this->ui->databasesList->currentRow());
            ofstream databases_stream(QDir::home().path().append("/.adhash/databases").toStdString().c_str());
            map<string, Database>::iterator it;
            for(it = this->databases.begin(); it != this->databases.end(); it++){
                databases_stream<<it->second.print()<<endl;
            }
            databases_stream.close();
        }
    }
}

void AdHashMgmt::deleteConf(){
    if(this->ui->configurationsList->currentRow() != -1){
        ConfDelDialog * d = new ConfDelDialog(this);
        d->exec();
        if(d->result()){
            Database * db = &this->databases[this->ui->databasesList->currentItem()->text().toStdString()];
            QString confText = this->ui->configurationsList->currentItem()->text();
            QStringList splits = confText.split(",");

            if(d->deleteContent){
                vector<ClusterConfiguration *> confs;
                confs.push_back(&this->configurations[splits[0].toStdString()]);
                ContentDelDialog * del = new ContentDelDialog(this, db, confs, splits[1].toInt());
                del->exec();
            }
            db->removeConf(confText);
            this->ui->configurationsList->takeItem(this->ui->configurationsList->currentRow());
            ofstream databases_stream(QDir::home().path().append("/.adhash/databases").toStdString().c_str());
            map<string, Database>::iterator it;
            for(it = this->databases.begin(); it != this->databases.end(); it++){
                databases_stream<<it->second.print()<<endl;
            }
            databases_stream.close();
        }
    }
}

void AdHashMgmt::partitionData(){
    if(this->ui->databasesList->currentRow() != -1){
        PartitioningDialog * d = new PartitioningDialog(this, this->databases[this->ui->databasesList->currentItem()->text().toStdString()]);
        d->exec();
        if(d->result()){
            ofstream databases_stream(QDir::home().path().append("/.adhash/databases").toStdString().c_str());
            map<string, Database>::iterator it;
            for(it = this->databases.begin(); it != this->databases.end(); it++){
                databases_stream<<it->second.print()<<endl;
            }
            databases_stream.close();
            this->ui->configurationsList->addItem(QString(d->cluster_conf+","+QString::number(d->num_parts)));
            this->ui->configurationsList->setCurrentRow(this->ui->configurationsList->count()-1);
        }
    }
}

void AdHashMgmt::launchEngine(){
    struct stat fileAtt;
    string command;
    FILE * fp;
    char buffer[1024];
    char *line_p;
    string executable = this->adhash_home+"/Release/engine";
    if(stat(executable.c_str(), &fileAtt) != 0){
        QMessageBox::critical(NULL, QObject::tr("Launch Failed"),
                              QObject::tr("AdHash executable does not exist!"),
                              QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    Database db = this->databases[this->ui->databasesList->currentItem()->text().toStdString()];
    QStringList confText = this->ui->configurationsList->currentItem()->text().split(",");
    QString num_parts = QString::number(confText.at(1).toInt()+1);
    QString confFile = QString(this->adhash_home.c_str())+"/conf/"+confText.at(0);
    string inputFileName = db.inputFile.substr(db.inputFile.find_last_of('/')+1);
    command = "mpirun -np "+num_parts.toStdString()+" -machinefile "+confFile.toStdString()+" "+executable+" "+this->adhash_home+" "+db.name+" "+db.directory+" "+inputFileName+" "+db.query_folder;
    fp = popen(command.c_str(), "r");
    if (!fp)
    {
        QMessageBox::critical(NULL, QObject::tr("Launch Failed"),
                              QObject::tr("Could not launch AdHash!"),
                              QMessageBox::Ok, QMessageBox::Ok);
    }
//    while((line_p = fgets(buffer, sizeof(buffer), fp))!=NULL){
//        cout<<line_p<<endl;
//    }
//    pclose(fp);
}

