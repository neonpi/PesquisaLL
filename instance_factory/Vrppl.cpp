//
// Created by Rafael on 06/01/2025.
//

#include "Vrppl.h"

Instance * Vrppl::buildInstance(string fileName) {

    Instance* instance = new Instance();

    ifstream file;
    file.open(fileName);
    string line;

    vector<Node> nodes;
    vector<string> splited_line;

    //Numero de clientes e número de lockers
    getline(file,line);
    splited_line = Utils::tookenize(line," ");
    instance->customers_qty = stoi(splited_line.at(0));
    instance->locker_qty = stoi(splited_line.at(1));
    instance->nodes.reserve(instance->n_node);

    //Numero de veíclos e capacidade de carga
    getline(file,line);
    splited_line = Utils::tookenize(line," ");
    instance->minimum_vehicle = stoi(splited_line.at(0));
    instance->load_capacity = stod(splited_line.at(1));

    Node node_d0,node_dt;
    node_d0.id = "D0";
    node_d0.index = 0;
    node_dt.id = "Dt";
    node_dt.index = 1;
    nodes.push_back(node_d0);
    nodes.push_back(node_dt);

    //Calculando load, index e ids
    for(int i=0; i<instance->customers_qty; i++) {
        getline(file,line);

        Node node;
        node.id = "C" + to_string(i-2);
        node.load_demand = stoi(line);
        node.index = nodes.size();
        nodes.push_back(node);
    }

    for(int i=0; i<instance->locker_qty; i++) {
        Node node;
        node.id = "P" + to_string(i);
        node.load_demand = 0;
        node.index = nodes.size();
        nodes.push_back(node);
    }


    instance->nodes = nodes;
    instance->n_node = nodes.size();

    //Definindo coordenadas, TW, ST e Customer type
    for(int i=0;i<instance->n_node;i++) {
        if(i!=1) {
            getline(file,line);
        }
        splited_line = Utils::tookenize(line," ");
        nodes.at(i).coord[0] = stod(splited_line.at(0));
        nodes.at(i).coord[1] = stod(splited_line.at(1));
        nodes.at(i).time_window[0] = stod(splited_line.at(2));
        nodes.at(i).time_window[1] = stod(splited_line.at(3));
        nodes.at(i).service_time = stod(splited_line.at(4));
        switch (stoi(splited_line.at(5))) {
            case 0:
                nodes.at(i).type = "d";
                break;
            case 1:
                nodes.at(i).type = "c1";
                break;
            case 2:
                nodes.at(i).type = "c2";
                break;
            case 3:
                nodes.at(i).type = "c3";
                break;
            default:
                nodes.at(i).type = "p";
                break;
        }

    }



    for(string s:splited_line) {
        cout<<s<<endl;
    }
}
