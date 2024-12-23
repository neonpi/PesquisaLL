//
// Created by Rafael on 24/11/2024.
//

#include "Utils.h"

#include <algorithm>


vector<Instance *> Utils::buildInstances() {
    ifstream file;
    file.open("instances/instances.txt");

    string line;
    vector<Instance*> instances;

    if(file.is_open()) {
        while(getline(file,line)) {
            Instance* new_instance = buildInstance(line);
            instances.push_back(new_instance);
        }
    }else {
        cout<<"File not opened"<<endl;
    }

    file.close();
    return instances;
}

void Utils::print_output(Search *search) {
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

Instance * Utils::buildInstance(string fileName) {
    ifstream file;
    file.open(fileName);
    string line;

    vector<Node> nodes;
    getline(file,line);
    vector<string> splited_line;
    while(getline(file,line)) {
        splited_line = tookenize(line," ");

        if(splited_line.at(0) == "param" || splited_line.at(0) == "0" || splited_line.at(0) == "1"){ break; }

        Node node;
        node.id = splited_line.at(0);
        node.type = splited_line.at(1);
        node.coord[0] = stod(splited_line.at(2));
        node.coord[1] = stod(splited_line.at(3));
        node.load_demand = stod(splited_line.at(4));
        node.time_window[0] = stod(splited_line.at(5));
        node.time_window[1] = stod(splited_line.at(6));
        node.service_time = stod(splited_line.at(7));
        node.index = static_cast<int>(nodes.size());

        nodes.push_back(node);

    }

    Instance* instance = new Instance();

    vector<string> instanceName = tookenize(fileName,"\\");
    instance->inst_name = instanceName.at(instanceName.size()-1);

    instance->n_node = nodes.size();
    instance->nodes = nodes;

    defineNodeIndexes(instance);

    //Definindo lockers - cliente
    int cus_index = instance->customer_indexes[0];
    if(splited_line.at(0) == "0" || splited_line.at(0) == "1") { //Caso em que tem varios lockers

        while(splited_line.at(0) == "0" || splited_line.at(0) == "1") {
            for(int i=0;i<splited_line.size();i++) {
                if(splited_line.at(i) == "1") {
                    instance->nodes.at(cus_index).designated_locker = &instance->nodes.at(instance->locker_indexes[0]+i);
                    break;
                }
            }
            cus_index++;

            getline(file,line);
            splited_line = tookenize(line," ");

        }

    }else { //Casos em que tem apenas um locker

        while(cus_index<instance->customer_indexes[1]) {
            if (instance->nodes.at(cus_index).type == "c2" || instance->nodes.at(cus_index).type == "c3") {
                instance->nodes.at(cus_index).designated_locker=&instance->nodes.at(instance->locker_indexes[0]);
            }
            cus_index++;
        }
    }

    //preenchendo listas de clientes por locker
    for (int i = instance->customer_indexes[0]; i < instance->customer_indexes[1]; i++) {
        if (instance->nodes.at(i).designated_locker != nullptr) {
            Node* aux_cus = &instance->nodes.at(i);
            Node* aux_loc = aux_cus->designated_locker;
            aux_loc->designated_customers.push_back(aux_cus);
            aux_loc->customers_qty++;
        }
    }

    //Ordenando clientes do locker por load
    for (int i=instance->locker_indexes[0]; i<instance->locker_indexes[1]; i++) {
        vector<Node*> customers = instance->nodes.at(i).designated_customers;
        sort(customers.begin(),customers.end(), [](Node* a, Node* b){return a->load_demand < b->load_demand;});

    }


    splited_line = tookenize(line," ");
    instance->load_capacity = stod(splited_line.at(3));

    getline(file,line);
    splited_line = tookenize(line," ");
    instance->avg_speed = stod(splited_line.at(3));

    instance->calculate_distances();

    instance->minimum_vehicle = 0;
    double load = 0;
    for (Node node: instance->nodes) {
        load += node.load_demand;

        if (load>=instance->load_capacity) {
            instance->minimum_vehicle++;
            load-=instance->load_capacity;
        }
    }

    if (load>0) {
        instance->minimum_vehicle++;
    }

    file.close();
    return instance;
}

void Utils::defineNodeIndexes(Instance *instance) {

    instance->customer_indexes[0] = 2;

    for(int i=2; i<instance->n_node; i++) {

        if(instance->nodes.at(i).type == "p") {
            instance->customer_indexes[1]=i;
            instance->locker_indexes[0]=i;
            instance->locker_indexes[1]=instance->n_node;
            break;
        }

    }

    instance->qty_customers = instance->customer_indexes[1] - 2;
    instance->qty_parcel_locker = instance->locker_indexes[1] - instance->qty_customers - 2;
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