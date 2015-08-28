#include "engine/comparisontab.h"
#include "ui_comparisontab.h"
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
ComparisonTab::ComparisonTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComparisonTab)
{
    ui->setupUi(this);
    MasterGUI * master = (MasterGUI*)parent;
    ui->bio2rdf->setPixmap(QPixmap(QString(string(master->adhash_home+"/images/bio2rdf.png").c_str())));
    ui->lubm1b->setPixmap(QPixmap(QString(string(master->adhash_home+"/images/lubm.png").c_str())));
    ui->partout_time->setPixmap(QPixmap(QString(string(master->adhash_home+"/images/partout_time.png").c_str())));
    ui->partout_comm->setPixmap(QPixmap(QString(string(master->adhash_home+"/images/partout_comm.png").c_str())));
    ui->bio2rdf->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    ui->bio2rdf->setScaledContents(true);
    ui->lubm1b->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    ui->lubm1b->setScaledContents(true);
    ui->partout_time->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    ui->partout_time->setScaledContents(true);
    ui->partout_comm->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    ui->partout_comm->setScaledContents(true);
    QHeaderView* header = this->ui->startupTab->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    ui->bio2rdf->resize(0.55*ui->bio2rdf->size());

}

ComparisonTab::~ComparisonTab()
{
    delete ui;
}
