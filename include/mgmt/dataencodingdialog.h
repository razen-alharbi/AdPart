#ifndef DATAENCODINGDIALOG_H
#define DATAENCODINGDIALOG_H

#include <QDialog>
#include <QThread>
#include "mgmt/database.h"
#include "common/utils.h"
#include "mgmt/TurtleParser.hpp"
#include "common/profiler.h"
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

namespace Ui {
class DataEncodingDialog;
}

struct triple{
        triple(){}
        triple(int s, int p, int o, Type::ID _ot){
                subject = s;
                predicate = p;
                object = o;
                ot = _ot;
        }
        int subject;
        int predicate;
        int object;
        Type::ID ot;
        string print(){
                if(ot == 1){
                        return "<"+toString(subject)+"> <"+toString(predicate)+"> \""+toString(object)+"\" .";
                }
                else{
                        return "<"+toString(subject)+"> <"+toString(predicate)+"> <"+toString(object)+"> .";
                }
        }
};

class DataEncodingDialog : public QDialog
{
    Q_OBJECT
    QThread workerThread;
public:
    DataEncodingDialog(QWidget *parent, Database *db);
    ~DataEncodingDialog();
    Ui::DataEncodingDialog *ui;
    Database *db;


private slots:
    void reject();
    void updatePB(int v);

signals:
    void operate();
};

class DataEncoder : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:

    DataEncoder(DataEncodingDialog * caller);
    DataEncodingDialog *caller;
private:
    void dump_encoded_data(FILE * pFile, vector<int> & data, int count);
    void dump_dictionaries(string &in_file_name, vector<string> &preds, vector<string> &verts);
    void reEncodeData(string &fn, string & fn_final, vector<int> &newPredsIDs, vector<int> &newVertsIDs);
public slots:
    void encodeData();
signals:
    void updateProgress(int m);
};

#endif // DATAENCODINGDIALOG_H
