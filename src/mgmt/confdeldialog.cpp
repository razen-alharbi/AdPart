#include "mgmt/confdeldialog.h"
#include "ui_confdeldialog.h"
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
ConfDelDialog::ConfDelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfDelDialog)
{
    deleteContent = false;
    ui->setupUi(this);
    ui->label->setPixmap(QPixmap("/home/harbir/razen/AdHashGUI/images/blueQM.png"));
    connect(this->ui->delContent, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

ConfDelDialog::~ConfDelDialog()
{
    delete ui;
}

void ConfDelDialog::updateState(){
    deleteContent = ui->delContent->isChecked();
}

void ConfDelDialog::accept(){
    this->done(1);
}
