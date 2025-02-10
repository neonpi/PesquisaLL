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


void Utils::print_result_file(Search *search, Instance *instance, int run, double time, long seed) {
    ofstream file;

    if(search == nullptr) {
        file.open("Output/"+instance->inst_name+"_stats", ofstream::out);
        if (!file.is_open()) {
            cout<<"Output file not opened"<<endl;
        }
        file<<"run,cost,#_vehicles,time,seed"<<endl;
    }else{
        file.open("Output/"+search->instance->inst_name+"_stats", ofstream::app);
        if (!file.is_open()) {
            cout<<"Output file not opened"<<endl;
        }
        file<<"--- "<<to_string(run)<<","<<to_string(search->solution->cost)<<","<<to_string(search->solution->used_routes)<<","<<to_string(time)<<","<<to_string(seed)<<endl;

        for (vector<Sequence> route: search->solution->routes) {
            if(route.size()>2) {
                string route_string = "";

                for (Sequence sequence: route) {
                    route_string+=sequence.node->id;

                    if (sequence.node->id != "Dt") {
                        if(sequence.node->type == "p") {
                            route_string+= "("+sequence.customer->id+")";
                        }
                        route_string+=" ";
                    }
                }
                file<<route_string<<endl;
            }

        }
        file<<endl;

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

bool Utils::differs(double a, double b, double epsilon) {
    return abs(a-b) > epsilon;
}

bool Utils::improves(double best, double current) {
    return current < best && differs(best,current);
}

void Utils::print_final_stats(Stats *stats) {
    ofstream file;

    if(stats->instance->inst_name == "C101_co_25.txt") {
        file.open("Output/0_final",ios::out);
        file<<"instance,avg_time,avg_cost,#_vehicles,best_cost"<<endl;
        file.close();
    }

    file.open("Output/0_final",ios::app);

    file<<stats->instance->inst_name<<","<<to_string(stats->avg_time)<<","<<to_string(stats->avg_cost)<<","<<to_string(stats->best_solution->used_routes)<<","<<to_string(stats->best_solution->cost)<<endl;

    file.close();
}

void Utils::print_screen_run(Stats *stats) {
    cout<<"AVG_COST: "<<stats->avg_cost<<" - AVG_TIME: "<<stats->avg_time<<" - BEST_COST: "<<stats->best_solution->cost<<" - BEST_TIME: "<<stats->best_time<<endl;
}

