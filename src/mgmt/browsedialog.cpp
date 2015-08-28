#include "mgmt/browsedialog.h"
#include "ui_browsedialog.h"
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
BrowseDialog::BrowseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrowseDialog)
{
    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));
}

BrowseDialog::~BrowseDialog()
{
    delete ui;
}

void BrowseDialog::browse(){
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select ADHASH_HOME"), QDir::home().path());

    if (!directory.isEmpty()) {
        this->ui->pathTF->setText(directory);
    }
}

void BrowseDialog::accept(){
    struct stat fileAtt;
    QString tmp_path = this->ui->pathTF->text().trimmed();
    if(!tmp_path.isEmpty()){
        if(stat(tmp_path.toStdString().c_str(), &fileAtt) != 0){
            QMessageBox::critical(NULL, QObject::tr("I/O Error"),
                                  QObject::tr("Path does not exist!"),
                                  QMessageBox::Ok, QMessageBox::Ok);
            this->ui->pathTF->clear();
        }
        else{
            this->path = tmp_path;
            this->setResult(1);
            this->done(1);
        }
    }
}
