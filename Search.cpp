//
// Created by Rafael on 29/11/2024.
//

#include "Search.h"

#include "Utils.h"

Search::Search(Instance *instance) {
    this->instance = instance;
    this->n_vehicles = instance->minimum_vehicle;
    this->customer_served = 0;
    this->total_cost = 0.0;
    this->best = nullptr;

    this->initialize_routes();

    this->visited = new bool[instance->n_node];
    for (int i=0;i<instance->n_node;i++) {
        this->visited[i] = false;
    }

    this->virtual_sequence = new Sequence();

}

Search::~Search() {

    if (this->best != nullptr) {
        delete this->best;
    }

    delete this->virtual_sequence;
    delete[] this->visited;
}

void Search::initialize_routes() {

    this->routes.reserve(this->n_vehicles);

    for (int i=0; i < this->n_vehicles; i++) {

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

void Search::construct() {
    this->insertion_heuristic();
}

void Search::insertion_heuristic() {

    vector<tuple<int,int,Sequence,double>> cand_list = build_candidate_list(); //(rota,antecessor,cliente destino)
    vector<tuple<int,int,Sequence,double>>::iterator it;
    //double alpha = 0.05;

    while(!cand_list.empty()) {

        tuple<int,int,Sequence,double> candidate = cand_list.at(0); //GULOSO

        //Randomizado
        /*int candidates = int(cand_list.size()*alpha);

        int rand_index = 0;
        if(candidates>0) {
            rand_index = rand()%candidates;
        }
        tuple<int,int,Sequence,double> candidate = cand_list.at(rand_index);*/
        //TODO TESTE
        /*if( this->routes.at(0).size()==2 ) {
            while(get<2>(candidate).node->id!="C35") {
                candidate = cand_list.at(list_index++);
            }
        }*/

        //Evitar colocar um nó entre lockers
        /*int list_index = 0;
        tuple<int,int,Sequence> candidate = cand_list.at(list_index++);
        Sequence *previous_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate));
        Sequence *next_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate)+1);

        while(previous_sequence->node->id == next_sequence->node->id) {
            candidate = cand_list.at(list_index++);
            previous_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate));
            next_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate)+1);
        }*/


        insert_sequency(candidate);
        //Utils::print_output_file(this);


        cand_list.erase(cand_list.begin(),cand_list.end());

        cand_list = build_candidate_list();
    }

    this->total_cost = 0.0;
    for (vector<Sequence> route : this->routes) {
        this->total_cost += (route.end()-1)->current_distance;
    }
}

vector<tuple<int, int, Sequence, double>> Search::build_candidate_list() {

    vector<tuple<int,int,Sequence, double>> cand_list;

    for (int i=this->instance->customer_indexes[0];i<this->instance->locker_indexes[1];i++) {

        Node* cand_node = &this->instance->nodes.at(i);
        if (this->instance->nodes.at(i).type == "p") {
            try_locker_candidate(&cand_list,cand_node);
        }else if (this->instance->nodes.at(i).type != "c2") {
            try_customer_candidate(&cand_list,cand_node);

        }

    }

    //funcao lambda
    //print_candidate_list(&cand_list);
    sort(cand_list.begin(),cand_list.end(),[this](const tuple<int,int,Sequence, double> cus_a, const tuple<int,int,Sequence, double> cus_b){return get<3>(cus_a) < get<3>(cus_b);});

    //print_candidate_list(&cand_list);

    return cand_list;
}

void Search::try_customer_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node *cand_node) {
    if(!this->visited[cand_node->index]) {

        int route_index = 0;
        for (vector<Sequence> route : this->routes) {

            //O Load é viável?
            if (is_load_viable(route_index,cand_node)) {


                int previous_sequence_index = 0;
                for (Sequence previous_sequence : route) {
                    if (previous_sequence.node->id != "Dt") {

                        Sequence cand_sequence;
                        cand_sequence.node = cand_node;
                        cand_sequence.customer = cand_node;

                        bool is_insertion_viable = propagate_virtual(route_index, previous_sequence_index, &cand_sequence);

                        if(is_insertion_viable) {
                            tuple<int, int, Sequence, double> cand_tuple = {route_index,previous_sequence_index,cand_sequence, 0.0};
                            calculate_delta_distance(&cand_tuple);
                            cand_list->push_back(cand_tuple);
                        }

                    }
                    previous_sequence_index++;
                }

            }


            route_index++;
        }

    }
}


void Search::try_locker_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node* cand_node) {

    int route_index = 0;
    for (vector<Sequence> route : this->routes) {

        Sequence* last_sequence = &(*(route.end()-1));
        //Pra cada cliente
        for (Node* customer_node: cand_node->designated_customers) {
            if (!this->visited[customer_node->index]) {

                //O load fica viável??
                if (is_load_viable(route_index,customer_node)) {

                    int previous_sequence_index = 0;
                    for (Sequence previous_sequence : route) {
                        //TODO da pra otimizar não alterando muito a cand_sequence. O que muda é só o load do customer
                        if (previous_sequence.node->id != "Dt") {

                            Sequence cand_sequence;
                            cand_sequence.node = cand_node;
                            cand_sequence.customer = customer_node;

                            bool is_insertion_viable = propagate_virtual(route_index, previous_sequence_index, &cand_sequence);

                            if(is_insertion_viable) {
                                tuple<int, int, Sequence, double> cand_tuple = {route_index,previous_sequence_index,cand_sequence, 0.0};
                                calculate_delta_distance(&cand_tuple);
                                cand_list->push_back(cand_tuple);
                            }

                        }
                        previous_sequence_index++;
                    }


                }
            }
        }
        route_index++;
    }
}

void Search::fill_forward_virtual(Sequence *next_sequence) {


    double distance = this->instance->distances[this->virtual_sequence->node->index][next_sequence->node->index];
    this->virtual_sequence->current_distance += distance;

    this->virtual_sequence->current_time += distance * this->instance->avg_speed + this->virtual_sequence->node->service_time;

    //Verificando se precisa de timeoff no depósito
    if(this->virtual_sequence->node->id == "D0" && this->virtual_sequence->current_time < next_sequence->node->time_window[0]) {
        this->virtual_sequence->current_time += next_sequence->node->time_window[0] - this->virtual_sequence->current_time;
    }

    this->virtual_sequence->current_load += next_sequence->customer->load_demand;
    this->virtual_sequence->node = next_sequence->node;
    this->virtual_sequence->customer = next_sequence->customer;

}

void Search::fill_forward(Sequence *current_sequence, Sequence *next_sequence) {

    double distance = this->instance->distances[current_sequence->node->index][next_sequence->node->index];

    next_sequence->current_distance = current_sequence->current_distance + distance;

    next_sequence->current_time = current_sequence->current_time + current_sequence->node->service_time + distance * this->instance->avg_speed;

    if(current_sequence->node->id == "D0" && next_sequence->current_time < next_sequence->node->time_window[0]) {
        current_sequence->time_off = next_sequence->node->time_window[0] - next_sequence->current_time;
        next_sequence->current_time += current_sequence->time_off;
    }

    next_sequence->current_load = current_sequence->current_load + next_sequence->customer->load_demand;

}

void Search::propagate(int route_index) {
    vector<Sequence>* route = &this->routes.at(route_index);

    Sequence* current_sequence = nullptr;
    Sequence* next_sequence = nullptr;

    for(int i=0; i<(route->size() - 1); i++) {

        current_sequence = &route->at(i);
        next_sequence = &route->at(i+1);
        fill_forward(current_sequence,next_sequence);

    }

}

bool Search::propagate_virtual(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    vector<Sequence>* route = &this->routes.at(route_index);

    //TODO depois otimizar, talvez nao precise começar do começo
    route->at(0).clone(this->virtual_sequence);
    this->virtual_sequence->reset_values();

    for(int i=1; i<route->size(); i++) {

        Sequence* current_sequence = &route->at(i);

        if(i-1 == previous_sequence_index) {
            fill_forward_virtual(cand_sequence);
            if(broke_time_window()) {
                return false;
            }
        }

        fill_forward_virtual(current_sequence);
        if(broke_time_window()) {
            return false;
        }

    }

    return true;
}

bool Search::broke_time_window() {

    return this->virtual_sequence->current_time < this->virtual_sequence->node->time_window[0] ||
        this->virtual_sequence->current_time > this->virtual_sequence->node->time_window[1];
}

bool Search::sort_function(const tuple<int, int, Sequence> cus_a, const tuple<int, int, Sequence> cus_b) {
    Sequence a_previous_sequence = this->routes.at(get<0>(cus_a)).at(get<1>(cus_a));
    Sequence a_next_sequence = this->routes.at(get<0>(cus_a)).at(get<1>(cus_a)+1);

    double distance_prev_next = this->instance->distances[a_previous_sequence.node->index][a_next_sequence.node->index];
    double distance_prev_cus = this->instance->distances[a_previous_sequence.node->index][get<2>(cus_a).node->index];
    double distance_cus_next = this->instance->distances[get<2>(cus_a).node->index][a_next_sequence.node->index];


    double delta_cus_a = distance_prev_cus + distance_cus_next - distance_prev_next;


    Sequence b_previous_sequence = this->routes.at(get<0>(cus_b)).at(get<1>(cus_b));
    Sequence b_next_sequence = this->routes.at(get<0>(cus_b)).at(get<1>(cus_b)+1);

    distance_prev_next = this->instance->distances[b_previous_sequence.node->index][b_next_sequence.node->index];
    distance_prev_cus = this->instance->distances[b_previous_sequence.node->index][get<2>(cus_b).node->index];
    distance_cus_next = this->instance->distances[get<2>(cus_b).node->index][b_next_sequence.node->index];

    double delta_cus_b = distance_prev_cus + distance_cus_next - distance_prev_next;

   return delta_cus_a < delta_cus_b;
}

void Search::calculate_delta_distance(tuple<int, int, Sequence, double> *cus) {
    Sequence a_previous_sequence = this->routes.at(get<0>(*cus)).at(get<1>(*cus));
    Sequence a_next_sequence = this->routes.at(get<0>(*cus)).at(get<1>(*cus)+1);

    double distance_prev_next = this->instance->distances[a_previous_sequence.node->index][a_next_sequence.node->index];
    double distance_prev_cus = this->instance->distances[a_previous_sequence.node->index][get<2>(*cus).node->index];
    double distance_cus_next = this->instance->distances[get<2>(*cus).node->index][a_next_sequence.node->index];

    double delta_distance = distance_prev_cus + distance_cus_next - distance_prev_next;
    get<3>(*cus) = delta_distance;
}

void Search::insert_sequency(tuple<int, int, Sequence, double> candidate) {
    int route_index = get<0>(candidate);
    int previous_sequence_index = get<1>(candidate);
    Sequence* candidate_sequence = &get<2>(candidate);


    this->routes.at(route_index).insert(this->routes.at(route_index).begin()+previous_sequence_index+1,
        1,
        *candidate_sequence);

    propagate(route_index);

    this->visited[candidate_sequence->customer->index] = true;

}


void Search::calculate_total_cost() {
    this->total_cost = 0.0;
    for (vector<Sequence> route: this->routes) {
        this->total_cost+=route.end()->current_distance;
    }
}

void Search::print_is_viable() {
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
        cout<<"Load inviability"<<endl;
        //exit(10);
    }

    if (!sp_visit_viable) {
        cout<<"Self pickup inviability"<<endl;
        //exit(10);
    }

    if (!customer_viable) {
        cout<<"Customer inviability ("<<customers.size()<<"): (";
        for(string customer: customers) {
            cout<< customer<<", ";
        }
        cout<<")"<<endl;
        //exit(2);
    }

    if(!time_window_viable) {
        cout<<"Time window inviability ("<<time_window.size()<<"): (";
        for(string time: time_window) {
            cout<< time;
            if(time != *(time_window.end()-1)) {
                cout<<", ";
            }
        }
        cout<<")"<<endl;
        cout<<this->instance->inst_name<<endl;
        //exit(10);

    }


}

bool Search::is_customer(int node_index) {
    return this->instance->nodes.at(node_index).type[0] == 'c';
}
bool Search::is_locker(int node_index) {
    return this->instance->nodes.at(node_index).type[0] == 'p';
}

void Search::print() {
    cout<<"TOTAL COST: "<<this->total_cost<<endl;
    cout<<"MIN_V: "<<this->instance->minimum_vehicle<<endl;
    cout<<"USED_V: "<<this->n_vehicles<<endl;
    cout<<"PATHS: "<<endl;
    for (int i=0; i<this->n_vehicles; i++) {
        cout<<"V_"<<i<<": ";
        for (Sequence sequence: this->routes.at(i)) {
            cout<<sequence.customer->id;
            if (sequence.node->type == "p") {
                cout<<"("<<sequence.node->id<<")";
            }
            cout<<" -> ";
        }
        cout<<endl;
    }
}

void Search::print_candidate_list(vector<tuple<int, int, Sequence, double>> *cand_list) {
    for(tuple<int,int,Sequence, double> cand: *cand_list) {
        int cand_route = get<0>(cand);
        int cand_index = get<1>(cand);
        Sequence *cand_sequence = &get<2>(cand);
        cout<<cand_route<<" - "<<cand_index<<" - "<<cand_sequence->node->id<<" - "<<get<3>(cand)<<endl;;
    }
    cout<<endl;
}

