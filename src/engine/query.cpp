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

#include "engine/query.h"

Query::Query(){

}
Query::Query(SPARQLParser parser){
    supported = true;
    if(parser.getPatterns().filters.size() > 0 || parser.getPatterns().unions.size() > 0 || parser.getPatterns().optional.size() > 0){
        this->supported = false;
        return;
    }
    for (SPARQLParser::projection_iterator iter=parser.projectionBegin(),limit=parser.projectionEnd();iter!=limit;++iter){
        this->projections.push_back("?"+parser.getVariableName(*iter));
    }

    vector<string> triple;
    Node node;
    for (std::vector<SPARQLParser::Pattern>::const_iterator iter=parser.getPatterns().patterns.begin(),limit=parser.getPatterns().patterns.end();iter!=limit;++iter) {
        triple.clear();
        if((*iter).subject.type == 0){
            triple.push_back("?"+parser.getVariableName((*iter).subject.id));
        }
        else{
            triple.push_back((*iter).subject.value);
        }

        triple.push_back((*iter).predicate.value);

        if((*iter).object.type == 0){
            triple.push_back("?"+parser.getVariableName((*iter).object.id));
        }
        else if((*iter).object.type == 1){
            triple.push_back("\""+(*iter).object.value+"\"");
        }
        else{
            triple.push_back((*iter).object.value);
        }
        node = Node(triple);
        this->string_nodes.push_back(node);

        for (unsigned i = 0; i < triple.size(); i++) {
            if (triple[i][0] == '?')
                if (find(variables.begin(), variables.end(), triple[i]) == variables.end())
                    variables.push_back(triple[i]);
        }
    }
}

string Query::print(){
    string result = "";
    for(unsigned i = 0 ; i < this->nodes.size(); i++){
        result += nodes[i].encode_query()+"\n";
    }
    return result;
}
