#include "mgmt/dataencodingdialog.h"
#include "ui_dataencodingdialog.h"
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
DataEncodingDialog::DataEncodingDialog(QWidget *parent, Database *db) :
    QDialog(parent),
    ui(new Ui::DataEncodingDialog)
{
    this->db = db;
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
    this->ui->progressBar->setRange(0,100);
    DataEncoder * encoder = new DataEncoder(this);
    encoder->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), encoder, SLOT(deleteLater()));
    connect(this, SIGNAL(operate()), encoder, SLOT(encodeData()));
    connect(encoder, SIGNAL(updateProgress(int)), this, SLOT(updatePB(int)));
    workerThread.start();
    emit operate();
}

DataEncodingDialog::~DataEncodingDialog()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

void DataEncodingDialog::reject(){
    workerThread.quit();
    workerThread.wait();
    this->done(0);
}

void DataEncodingDialog::updatePB(int v){
    if(v == 1000){
        this->done(1);
        return;
    }

    this->ui->progressBar->setValue(v);
    if(v == this->ui->progressBar->maximum()){
        this->ui->progressBar->setRange(0,0);
    }
}

DataEncoder::DataEncoder(DataEncodingDialog * caller){
    this->caller = caller;
}

void DataEncoder::encodeData(){
    ifstream file(this->caller->db->inputFile.c_str(), ios::binary | ios::ate);
    long long file_size = file.tellg();
    file.close();
    Type::ID objectType;
    string subject,predicate,object,objectSubType;
    ifstream fin(this->caller->db->inputFile.c_str());
    TurtleParser parser(fin);
    int sid, pid, oid, progress, so_id = 0, predicate_id = 0;
    long long counter = 0;
    boost::unordered_map<string, int> preds_lookup;
    boost::unordered_map<string, int> verts_lookup;
    boost::unordered_map<string, int>::iterator lookup_it;
    string in_file_name = this->caller->db->inputFile.substr(this->caller->db->inputFile.find_last_of('/')+1,this->caller->db->inputFile.size());
    string fn = this->caller->db->data_folder+in_file_name+"_tmp";
    string fn_final = this->caller->db->data_folder+in_file_name+"_encoded";
    FILE * pFile = fopen(fn.c_str(), "wb");
    vector<int> data;
    try {
        while (true) {
            try {
                if (!parser.parse(subject,predicate,object,objectType,objectSubType))
                    break;
            } catch (const TurtleParser::Exception& e) {
                cerr << e.message << endl;
                // recover...
                while (fin.get()!='\n') ;
                continue;
            }
            counter++;
            //lookup subject
            lookup_it = verts_lookup.find(subject);
            if (lookup_it == verts_lookup.end()) {
                verts_lookup[subject] = so_id;
                //verts.insert(subject);
                sid = so_id;
                so_id++;
            }
            else{
                sid = lookup_it->second;
            }

            //lookup predicate
            lookup_it = preds_lookup.find(predicate);
            if (lookup_it == preds_lookup.end()) {
                preds_lookup[predicate] = predicate_id;
                //preds.insert(predicate);
                pid = predicate_id;
                predicate_id++;
            }
            else{
                pid = lookup_it->second;
            }

            //lookup object
            for(unsigned i = 0 ; i < object.length() ;i++){
                if(object[i] == '\n' || object[i] == '\r')
                    object [i] = ' ';
            }
            if(objectType != 0){
                object = "\""+object+"\"";
            }
            lookup_it = verts_lookup.find(object);
            if (lookup_it == verts_lookup.end()) {
                //verts.insert(object);
                verts_lookup[object] = so_id;
                oid = so_id;
                so_id++;
            }
            else{
                oid = lookup_it->second;
            }

            data.push_back(sid);
            data.push_back(pid);
            data.push_back(oid);
            data.push_back(objectType);

            if(counter%65536 == 0){
                this->dump_encoded_data(pFile, data, data.size());
                data.clear();
                progress = (int)(((float)parser.get_parsed_so_far())/file_size*100);
                emit updateProgress(progress);
            }
        }
        progress = (int)(((float)parser.get_parsed_so_far())/file_size*100);
        this->dump_encoded_data(pFile, data, data.size());
        data.clear();
        fin.close();
        fclose(pFile);
        this->caller->db->num_triples = counter;
        this->caller->db->num_preds = preds_lookup.size();
        this->caller->db->num_verts = verts_lookup.size();
        emit updateProgress(progress);
        {
            this->caller->ui->groupBox->setTitle("Data Encoder Progress: ReEncoding string ID's. Please wait!");
            vector<string> v_data(verts_lookup.size());
            int counter = 0;
            for(lookup_it = verts_lookup.begin(); lookup_it != verts_lookup.end(); lookup_it++)
                v_data[counter++] = lookup_it->first;
            sort(v_data.begin(), v_data.end());
            vector<string> p_data(preds_lookup.size());
            counter = 0;
            for(lookup_it = preds_lookup.begin(); lookup_it != preds_lookup.end(); lookup_it++)
                p_data[counter++] = lookup_it->first;
            sort(p_data.begin(), p_data.end());
            vector<int> newPredsIDs(preds_lookup.size());
            vector<int> newVertsIDs(verts_lookup.size());
            counter = 0;
            for(unsigned i = 0; i < p_data.size(); i++){
                newPredsIDs[preds_lookup[p_data[i]]] = counter++;
            }
            counter = 0;
            for(unsigned i = 0; i < v_data.size(); i++){
                newVertsIDs[verts_lookup[v_data[i]]] = counter++;
            }
            this->reEncodeData(fn, fn_final, newPredsIDs, newVertsIDs);
            remove(fn.c_str());
            newPredsIDs.clear();
            newVertsIDs.clear();
            caller->ui->groupBox->setTitle("Data Encoder Progress: Dumping dictionaries. Please wait!");
            this->dump_dictionaries(in_file_name, p_data, v_data);
        }
        preds_lookup.clear();
        verts_lookup.clear();
        emit updateProgress(1000);
    }catch (const TurtleParser::Exception&) {
        return ;
    }
}

void DataEncoder::reEncodeData(string &fn, string & fn_final, vector<int> &newPredsIDs, vector<int> &newVertsIDs){
    FILE * inFile, *outFile;
    long int num_rec, records_per_iteration, num_iterations;
    char * buffer;
    size_t result;
    vector<int> data;

    inFile = fopen(fn.c_str(), "rb");
    outFile = fopen(fn_final.c_str(), "wb");

    if(inFile==NULL || outFile==NULL){
        cout<<("Could not open the file")<<endl;
        exit (2);
    }

    buffer = (char*) malloc(sizeof(char)*BUFFERSIZE);
    if(buffer == NULL){
        cout<<("Memory error")<<endl;
        exit (2);
    }

    fseek (inFile , 0 , SEEK_END);
    num_rec = ftell (inFile)/RECORDSIZE;
    records_per_iteration = BUFFERSIZE/RECORDSIZE;
    data.resize(BUFFERSIZE/sizeof(int));
    num_iterations = ceil(1.0*num_rec/records_per_iteration);
    rewind (inFile);
    int subj, obj, pred, type, index = 0;
    for(unsigned int i = 0 ; i < num_iterations; i++){
        result = fread(buffer, RECORDSIZE, records_per_iteration, inFile);
        for(unsigned int j = 0 ; j < result*RECORDSIZE ; j+=RECORDSIZE){
            memcpy(&subj, buffer+j, sizeof(int));
            memcpy(&pred, buffer+j+4, sizeof(int));
            memcpy(&obj, buffer+j+8, sizeof(int));
            memcpy(&type, buffer+j+12, sizeof(int));
            data[index++] = newVertsIDs[subj];
            data[index++] = newPredsIDs[pred];
            data[index++] = newVertsIDs[obj];
            data[index++] = type;
        }
        this->dump_encoded_data(outFile, data, index);
        index = 0;
    }
    fclose(inFile);
    fclose(outFile);
    free(buffer);
}

void DataEncoder::dump_encoded_data(FILE * pFile, vector<int> & data, int count){
    fwrite(&data[0], sizeof(int), count, pFile);
}

void DataEncoder::dump_dictionaries(string &in_file_name, vector<string> &preds, vector<string> &verts){
   // set<string>::iterator i;
    string fn = this->caller->db->dic_folder+in_file_name+"_preds.dic";
    FILE * pFile = fopen(fn.c_str(), "wb");
    vector<int> aux(2);
    int counter = 0;
    for(unsigned i = 0; i < preds.size(); i++){
        aux[0] = counter++;
        aux[1] = preds[i].size();
        fwrite(&aux[0], sizeof(int), 2, pFile);
        fwrite(&preds[i][0], sizeof(char), aux[1], pFile);
    }
    fclose(pFile);

    fn = this->caller->db->dic_folder+in_file_name+"_verts.dic";
    pFile = fopen(fn.c_str(), "wb");
    counter = 0;
    for(unsigned i = 0; i < verts.size(); i++){
        aux[0] = counter++;
        aux[1] = verts[i].size();
        fwrite(&aux[0], sizeof(int), 2, pFile);
        fwrite(&verts[i][0], sizeof(char), aux[1], pFile);
    }
    fclose(pFile);
}
