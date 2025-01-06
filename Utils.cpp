//
// Created by Rafael on 24/11/2024.
//

#include "Utils.h"

#include <algorithm>

#include "instance_factory/Evrptwprpl.h"


vector<Instance *> Utils::buildInstances(string problem) {
    ifstream file;
    file.open("instances/instances_" + problem + ".txt");
    string line;
    vector<Instance*> instances;

    if(file.is_open()) {
        while(getline(file,line)) {
            Instance* new_instance = nullptr;
            if(problem == "vrppl") {

            }else {
                new_instance = Evrptwprpl::buildInstance(line);
            }
            instances.push_back(new_instance);
        }
    }else {
        cout<<"File not opened"<<endl;
    }

    file.close();
    return instances;
}

void Utils::print_output_file(Search *search) {
    ofstream file;
    file.open("Output/"+search->instance->inst_name, ofstream::out);
    if (!file.is_open()) {
        cout<<"File not opened"<<endl;
    }
    file<<"route"<<endl;

    for (vector<Sequence> route: search->routes) {
        string route_string = "";

        for (Sequence sequence: route) {
            route_string+=sequence.node->id;
            if (sequence.node->id != "Dt") {
                route_string+=" ";
            }
        }
        file<<route_string<<endl;

    }

    file.close();
}



vector<string> Utils::tookenize(string str, string symbol) {

    stringstream ss(str);

    vector<string> tokens;
    string temp_str;

    while(getline(ss,temp_str,symbol[0])) {
        tokens.push_back(temp_str);
    }

    return tokens;

}