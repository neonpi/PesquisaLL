//
// Created by Rafael on 24/11/2024.
//

#include "Utils.h"

#include <algorithm>

#include "instance_factory/Evrptwprpl.h"
#include "instance_factory/Vrppl.h"


vector<Instance *> Utils::buildInstances(string problem) {
    ifstream file;
    file.open("instances/instances_" + problem + ".txt");
    string line;
    vector<Instance*> instances;

    if(file.is_open()) {
        while(getline(file,line)) {
            Instance* new_instance = nullptr;
            if(problem == "vrppl") {
                new_instance = Vrppl::buildInstance(line);
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

void Utils::print_route_file(Search *search, bool reset) {
    ofstream file;
    if(reset) {
        file.open("Output/"+search->instance->inst_name+"_routes", ofstream::out);
        file<<"route"<<endl;
    }else {
        file.open("Output/"+search->instance->inst_name+"_routes", ofstream::app);
    }
    if (!file.is_open()) {
        cout<<"Output file not opened"<<endl;
    }

    for (vector<Sequence> route: search->routes) {
        if(route.size()>2) {
            string route_string = "";

            for (Sequence sequence: route) {
                route_string+=sequence.node->id;
                if (sequence.node->id != "Dt") {
                    route_string+=" ";
                }
            }
            file<<route_string<<endl;
        }

    }
    file<<"---"<<endl;

    file.close();
}

void Utils::print_stats_file(Stats *stats) {
    ofstream file;
    file.open("Output/"+stats->instance->inst_name+"_stats", ofstream::out);

    file<<"COST,TIME"<<endl;
    for(int i=0;i<stats->config->runs;i++) {
        file<<to_string(stats->costs.at(i))<<","<<to_string(stats->costs.at(i))<<endl;
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
