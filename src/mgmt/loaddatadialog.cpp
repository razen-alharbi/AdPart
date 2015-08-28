#include "mgmt/loaddatadialog.h"
#include "ui_loaddatadialog.h"
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
LoadDataDialog::LoadDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDataDialog)
{
    ui->setupUi(this);
    QPalette palette = ui->frame->palette();
    palette.setColor( backgroundRole(), QColor(236, 232, 228) );
    ui->frame->setPalette( palette );
    ui->frame->setAutoFillBackground( true );
    cachedPath = QDir::home().path();
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseFile()));
    connect(ui->browseButton_2, SIGNAL(clicked()), this, SLOT(browseDirectory()));
    connect(ui->browseButton_3, SIGNAL(clicked()), this, SLOT(browseDirectory2()));
}

LoadDataDialog::~LoadDataDialog()
{
    delete ui;
}

void LoadDataDialog::browseFile(){
    QString inputFile = QFileDialog::getOpenFileName(this, tr("Select input file (.nt)"), cachedPath);

    if (!inputFile.isEmpty()) {
        cachedPath = inputFile.left(inputFile.lastIndexOf('/'));
        this->ui->pathTF->setText(inputFile);
    }
}

void LoadDataDialog::browseDirectory(){
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select database directory"), cachedPath);

    if (!directory.isEmpty()) {
        cachedPath = inputFile;
        this->ui->pathTF_2->setText(directory);
    }
}

void LoadDataDialog::browseDirectory2(){
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select query files directory"), cachedPath);

    if (!directory.isEmpty()) {
        cachedPath = inputFile;
        this->ui->pathTF_4->setText(directory);
    }
}

void LoadDataDialog::accept(){
//    this->ui->pathTF_3->setText("LUBM-160");
//    this->ui->pathTF->setText("/home/harbir/data/lubm/string/160/lubm_160.nt");
//    this->ui->pathTF_2->setText("/home/harbir/data/lubm/string/160/db");
//    this->ui->pathTF_4->setText("");
    struct stat fileAtt;
    dbName = this->ui->pathTF_3->text().trimmed();
    if(!dbName.isEmpty()){
        if(((AdHashMgmt*)this->parent())->databases.find(dbName.toStdString()) == ((AdHashMgmt*)this->parent())->databases.end()){
            QString tmp_path = this->ui->pathTF->text().trimmed();
            if(!tmp_path.isEmpty()){
                if(stat(tmp_path.toStdString().c_str(), &fileAtt) != 0){
                    QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                          QObject::tr("Path does not exist!"),
                                          QMessageBox::Ok, QMessageBox::Ok);
                    this->ui->pathTF->clear();
                }
                else{
                    this->inputFile = tmp_path;
                    tmp_path = this->ui->pathTF_2->text().trimmed();
                    if(!tmp_path.isEmpty()){
                        if(stat(tmp_path.toStdString().c_str(), &fileAtt) != 0){
                            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                                  QObject::tr("Path does not exist!"),
                                                  QMessageBox::Ok, QMessageBox::Ok);
                            this->ui->pathTF_2->clear();
                        }
                        else{
                            this->databaseDirectory = tmp_path;
                            tmp_path = this->ui->pathTF_4->text().trimmed();
                            if(!tmp_path.isEmpty()){
                                if(stat(tmp_path.toStdString().c_str(), &fileAtt) != 0){
                                    QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                                          QObject::tr("Path does not exist!"),
                                                          QMessageBox::Ok, QMessageBox::Ok);
                                    this->ui->pathTF_4->clear();
                                }
                                else{
                                    this->queryPath = tmp_path;
                                    this->setResult(1);
                                    this->done(1);
                                }
                            }
                            else{
                                this->setResult(1);
                                this->done(1);
                            }
                        }
                    }
                }
            }
        }
        else{
            QMessageBox::critical(NULL, QObject::tr("Logical Error"),
                                  QObject::tr("A database with the same name already exists!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
        }
    }
}
