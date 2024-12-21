//
// Created by Rafael on 24/11/2024.
//

#include "Instance.h"

#include <algorithm>

Instance::Instance() {
    this->inst_name="";
    this->n_node = 0;
    this->nodes = vector<Node>();
    this->customer_indexes[0] = 0;
    this->customer_indexes[1] = 0;
    this->charger_st_indexes[0] = 2;
    this->charger_st_indexes[1] = 0;
    this->locker_indexes[0] = 0;
    this->locker_indexes[1] = 0;

    this->qty_customers = 0;
    this->qty_parcel_locker = 0;
    this->qty_charger_st = 0;

    this->batt_capacity=0;
    this->load_capacity=0;
    this->consumption_rate=0;
    this->charging_rate=0;
    this->avg_speed=0;
}

Instance::~Instance() {

    if(this->nodes.empty()) {
        for(int i=0;i<this->n_node;i++) {
            delete this->distances[i];
        }
        delete [] this->distances;
    }

}
void Instance::calculate_distances() {
    this->distances = new double*[this->n_node];
    for(int i=0; i<this->n_node; i++) {
        this->distances[i] = new double[this->n_node];
    }

    for(int i=0; i<this->n_node; i++) {
        for(int j=i; j<this->n_node;j++) {
            if(i==j) {
                this->distances[i][j] = 0.0;
            }else {
                this->distances[i][j] =
                    sqrt(
                        pow(this->nodes.at(i).coord[0]-this->nodes.at(j).coord[0],2)
                        +
                        pow(this->nodes.at(i).coord[1]-this->nodes.at(j).coord[1],2)
                        );
                this->distances[j][i] = this->distances[i][j];
            }
        }
    }
}

void Instance::calcula_nearest_stations() {

    //Estaçoes mais proximas de pares
    for(int i=0; i<this->n_node; i++) {
        this->nearest_stations.emplace_back();
        for (int j=0;j<this->n_node;j++) {
            Node* nearest_station = nullptr;
            double min_distance = -1.0;
            double distance;
            for (int k=this->charger_st_indexes[0];k<this->charger_st_indexes[1];k++) {
                distance = this->distances[i][k] + this->distances[k][j];
                if(nearest_station == nullptr || distance < min_distance) {
                    nearest_station = &this->nodes.at(k);
                    min_distance = distance;
                }
            }
            this->nearest_stations.at(i).push_back(nearest_station);


        }
    }
    //Estações mais proximas de nos
    for (int i=0; i<this->n_node; i++) {
        Node* node = &this->nodes.at(i);
        node->nearest_station = &this->nodes.at(charger_st_indexes[0]);
        for (int j=charger_st_indexes[0]+1; j<this->charger_st_indexes[1]; j++) {
            if (this->distances[node->index][this->nodes.at(j).index] < this->distances[node->index][node->nearest_station->index]) {
                node->nearest_station = &this->nodes.at(j);
            }
        }
    }
}


void Instance::print() {
    cout<<"Params: "<<endl;
    cout<<"- Q: "+to_string(this->batt_capacity)<<endl;
    cout<<"- CV: "+to_string(this->load_capacity)<<endl;
    cout<<"- r: "+to_string(this->consumption_rate)<<endl;
    cout<<"- g: "+to_string(this->charging_rate)<<endl;
    cout<<"- V: "+to_string(this->avg_speed)<<endl;

    cout<<endl<<"Nodes:"<<endl;

    for(int i=0;i<this->n_node;i++) {
        this->nodes.at(i).print();
    }

    cout<<endl<<"Distances:"<<endl;

    for(int i=0;i<this->n_node;i++) {
        for(int j=0;j<this->n_node;j++) {
            cout<<this->distances[i][j]<<" ";
        }
        cout<<endl;
    }
}
