//
// Created by Rafael on 27/01/2025.
//

#include "Solution.h"

Solution::Solution(Instance* instance) {
    this->total_cost = 0.0;
    this->visited = new bool[instance->n_node];
    for (int i=0;i<instance->n_node;i++) {
        this->visited[i] = false;
    }
    this->instance = instance;
    this->initialize_routes();

}

Solution::~Solution() {
    delete[] this->visited;
}


void Solution::initialize_routes() {

    this->routes.reserve(this->instance->max_vehicle);

    for (int i=0; i < this->instance->max_vehicle; i++) {

        Sequence depot_0, depot_t;
        depot_0.node = &this->instance->nodes.at(0);
        depot_0.customer = &this->instance->nodes.at(0);
        depot_t.node = &this->instance->nodes.at(1);
        depot_t.customer = &this->instance->nodes.at(1);
        depot_0.current_load = 0;
        depot_t.current_load = 0;
        vector<Sequence> route ={depot_0, depot_t};
        this->routes.push_back(route);
        this->routes.at(i).reserve(this->instance->n_node);
    }
}

void Solution::calculate_total_cost() {
    this->total_cost = 0.0;
    for (vector<Sequence> route : this->routes) {
        this->total_cost += (route.end()-1)->current_distance;
    }
}


void Solution::print() {
    int used_vehicles = 0;
    vector<string> routes_string;

    for (int i=0; i<this->instance->max_vehicle; i++) {
        if((int)this->routes.at(i).size()>2) {
            string s= "V_"+to_string(i)+": ";
            for (Sequence sequence: this->routes.at(i)) {
                s+=sequence.customer->id;
                if (sequence.node->type == "p") {
                    s+="("+sequence.node->id+")";
                }
                s+=" -> ";
            }
            routes_string.push_back(s);
            used_vehicles++;
        }
    }

    cout<<"TOTAL COST: "<<this->total_cost<<endl;
    cout<<"MAX_V: "<<this->instance->max_vehicle<<endl;
    cout<<"USED_V: "<<used_vehicles<<endl;
    cout<<"PATHS: "<<endl;

    for(string s: routes_string) {
        cout<<s<<endl;
    }
}


void Solution::print_is_viable(long seed) {
    bool customer_viable = true;

    vector<string> customers = {};
    for (int i = this->instance->customer_indexes[0];
        i < this->instance->customer_indexes[1] ;i++) {
        if (!this->visited[i]) {
            customer_viable = false;
            customers.push_back(this->instance->nodes.at(i).id);

        }
    }

    int sp_visit=0;
    bool sp_visit_viable = true;

    bool time_window_viable = true;
    vector<string> time_window={};

    bool load_viable = true;

    for (vector<Sequence> route: this->routes) {
        Sequence* seq = &*(route.end()-1);
        if (seq->node->type == "c2") {
            sp_visit_viable = false;
            sp_visit++;
        }

        if (seq->current_load > this->instance->load_capacity) {
            load_viable = false;
        }

        for (Sequence sequence: route) {
            if (sequence.current_time < sequence.node->time_window[0] || sequence.current_time > sequence.node->time_window[1]) {
                time_window_viable = false;
                time_window.push_back(sequence.node->id);
            }
        }
    }



    if (!load_viable) {
        cout<<"Load inviability, seed="<<to_string(seed)<<endl;
        exit(10);
    }

    if (!sp_visit_viable) {
        cout<<"Self pickup inviability, seed="<<to_string(seed)<<endl;
        exit(10);
    }

    if (!customer_viable) {
        cout<<"Customer inviability ("<<(int)customers.size()<<"): (";
        for(string customer: customers) {
            cout<< customer<<", ";
        }
        cout<<"), seed="<<to_string(seed)<<endl;
        exit(10);
    }

    if(!time_window_viable) {
        cout<<"Time window inviability ("<<(int)time_window.size()<<"): (";
        for(string time: time_window) {
            cout<< time;
            if(time != *(time_window.end()-1)) {
                cout<<", ";
            }
        }
        cout<<"), seed="<<to_string(seed)<<endl;
        cout<<this->instance->inst_name<<endl;
        print();
        exit(10);

    }


}
