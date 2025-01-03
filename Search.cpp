//
// Created by Rafael on 29/11/2024.
//

#include "Search.h"

#include "Utils.h"

Search::Search(Instance *instance, int n_vehicles) {
    this->instance = instance;
    this->n_vehicles = max(n_vehicles,instance->minimum_vehicle);
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
        depot_0.max_time_off = depot_0.node->time_window[1];
        depot_t.max_time_off = depot_t.node->time_window[1];

        vector<Sequence> route ={depot_0, depot_t};
        this->routes.push_back(route);
        this->routes.at(i).reserve(this->instance->n_node);
    }
}

void Search::construct() {
    this->insertion_heuristic();
}

void Search::insertion_heuristic() {

    vector<tuple<int,int,Sequence>> cand_list = build_candidate_list(); //(rota,antecessor,cliente destino)
    vector<tuple<int,int,Sequence>>::iterator it;


    while(!cand_list.empty()) {

        int list_index = 0;
        tuple<int,int,Sequence> candidate = cand_list.at(list_index++);

        //Evitar colocar um nó entre lockers
        /*Sequence *previous_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate));
        Sequence *next_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate)+1);

        while(previous_sequence->node->id == next_sequence->node->id) {
            candidate = cand_list.at(list_index++);
            previous_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate));
            next_sequence = &this->routes.at(get<0>(candidate)).at(get<1>(candidate)+1);
        }*/


            //tuple<int,int,Node*> cand = cand_list.at(rand()%cand_list.size()); //Só pra testar
        insert_sequency(candidate);
        Utils::print_output(this);


        cand_list.erase(cand_list.begin(),cand_list.end());

        cand_list = build_candidate_list();
    }

    this->total_cost = 0.0;
    for (vector<Sequence> route : this->routes) {
        this->total_cost += (route.end()-1)->current_distance;
    }
}

vector<tuple<int, int, Sequence>> Search::build_candidate_list() {

    vector<tuple<int,int,Sequence>> cand_list;

    for (int i=this->instance->customer_indexes[0];
            i<this->instance->locker_indexes[1];i++) {

        Node* cand_node = &this->instance->nodes.at(i);
        if (this->instance->nodes.at(i).type == "p") {
            try_locker_candidate(&cand_list,cand_node);
        }else if (this->instance->nodes.at(i).type != "c2") {
            try_customer_candidate(&cand_list,cand_node);

        }

            }

    //funcao lambda
    sort(cand_list.begin(),cand_list.end(),[this](const tuple<int,int,Sequence> cus_a, const tuple<int,int,Sequence> cus_b){return sort_function(cus_a,cus_b);});

    return cand_list;
}


void Search::fill_candidate_sequences(Sequence *previous_sequence, Sequence *next_sequence, Sequence *cand_sequence,
    Node *cand_node, Node *customer_node) {
    Node* previous_node = previous_sequence->node;

    double distance_from_previous = this->instance->distances[previous_node->index][cand_node->index];
    double travel_time = this->instance->avg_speed*(distance_from_previous);
    cand_sequence->node = cand_node;
    cand_sequence->customer = customer_node;
    cand_sequence->current_load = previous_sequence->current_load - customer_node->load_demand;
    cand_sequence->current_distance = previous_sequence->current_distance + distance_from_previous;

    cand_sequence->current_time = previous_sequence->current_time + travel_time;
    /*TODO O somatorio de tempo aqui não vai tratar o timeoff do nó atual.
    TODO Ele vai ser tratado SOMENTE quando uma estação for necessária ou um timeoff for obrigadório por conta de time window
    TODO Isso na hora de verificar as viabilidades*/

    //tratando o service time
    if (previous_sequence->node->type =="p") {
        if (previous_sequence->node->id != cand_node->id) {
            cand_sequence->current_time += cand_node->service_time;
        }
    }else {
        cand_sequence->current_time += cand_node->service_time;
    }
    //O time window do proximo no pode diminuir ainda mais o timeoff, se a tw do proximo no terminar bem antes
    if (next_sequence!=nullptr) {
        Node* next_node = next_sequence->node;
        cand_sequence->max_time_off = min(next_node->time_window[1], next_sequence->max_time_off) - cand_sequence->current_time;
    }

    if (cand_node->type == "p") {
        cand_sequence->method = 'p';
    }else {
        cand_sequence->method = 'h';
    }
}

void Search::accumulate_virtual_sequence(Sequence *previous_sequence, Sequence *virtual_sequence, Sequence *current_sequence, Sequence *next_sequence) {

    Node* previous_node = previous_sequence->node;
    Node* new_node = virtual_sequence->node;
    Node* next_node = next_sequence->node;

    double distance_previous_to_new = this->instance->distances[previous_node->index][new_node->index];
    double travel_time = this->instance->avg_speed*(distance_previous_to_new);
    virtual_sequence->current_load -= virtual_sequence->node->load_demand;
    virtual_sequence->current_distance += distance_previous_to_new;

    virtual_sequence->current_time += travel_time + current_sequence->time_off;

    //tratando o service time
    if (previous_sequence->node->type =="p") {
        if (previous_sequence->node->id != new_node->id) {
            virtual_sequence->current_time += new_node->service_time;
        }
    }else {
        virtual_sequence->current_time += new_node->service_time;
    }
    //O time window do proximo no pode diminuir ainda mais o timeoff, se a tw do proximo no terminar bem antes
    virtual_sequence->max_time_off = current_sequence->max_time_off;

    if (new_node->type == "p") {
        virtual_sequence->method = 'p';
    }else {
        virtual_sequence->method = 'h';
    }
}


void Search::try_customer_candidate(vector<tuple<int, int, Sequence>> *cand_list, Node *cand_node) {

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
                            cand_list->push_back({route_index,previous_sequence_index,cand_sequence});
                        }

                    }
                    previous_sequence_index++;
                }

            }


            route_index++;
        }

    }
}


void Search::try_locker_candidate(vector<tuple<int, int, Sequence>>* cand_list, Node* cand_node) {

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
                                cand_list->push_back({route_index,previous_sequence_index,cand_sequence});
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

void Search::fill_next_virtual(Sequence *next_sequence, int scan_i, double *delta_time, bool is_candidate) {


    double distance = this->instance->distances[this->virtual_sequence->node->index][next_sequence->node->index];
    this->virtual_sequence->current_distance += distance;

    this->virtual_sequence->current_time += distance * this->instance->avg_speed;
    //Considerando apenas um service time do locker
    if(next_sequence->node->id != this->virtual_sequence->node->id) {
        this->virtual_sequence->current_time += next_sequence->node->service_time;
    }

    this->virtual_sequence->current_load += next_sequence->customer->load_demand;

    if(scan_i == 1) {

        //virtual_sequence->current_time += delta_time == nullptr? 0 :  *delta_time;

        double time_off = max(0.0,min(this->virtual_sequence->max_time_off,next_sequence->node->time_window[0] - this->virtual_sequence->current_time));
        //Não faz sentido atribuir o timeoff aqui, ele pertence ao nó anterior

        if(delta_time!=nullptr) {
            *delta_time+=time_off;
        }

        virtual_sequence->current_time += time_off;
        virtual_sequence->max_time_off = next_sequence->max_time_off - *delta_time;
        //TODO rever essa atribuição quando está no deposito final
    }

    this->virtual_sequence->node = next_sequence->node;
    this->virtual_sequence->customer = next_sequence->customer;

    if(is_candidate) {
        next_sequence->current_time = this->virtual_sequence->current_time;
    }

}

void Search::fill_previous_virtual(Sequence *previous_sequence, bool is_candidate_sequence) {
    double distance = this->instance->distances[this->virtual_sequence->node->index][previous_sequence->node->index];
    this->virtual_sequence->current_distance -= distance;

    this->virtual_sequence->max_time_off =
        min(this->virtual_sequence->max_time_off,
            this->virtual_sequence->node->time_window[1] - this->virtual_sequence->current_time);

    //TODO tratar arredondamentos
    this->virtual_sequence->current_time -= distance * this->instance->avg_speed;
    //Considerando apenas um service time do locker
    if(previous_sequence->node->id != this->virtual_sequence->node->id) {
        this->virtual_sequence->current_time -= this->virtual_sequence->node->service_time;
    }

    this->virtual_sequence->current_load -= this->virtual_sequence->customer->load_demand;


    this->virtual_sequence->node = previous_sequence->node;
    this->virtual_sequence->customer = previous_sequence->customer;


    if(is_candidate_sequence) {
        previous_sequence->max_time_off = this->virtual_sequence->max_time_off;
    }
}

void Search::fill_next(Sequence *current_sequence, Sequence *next_sequence, int scan_i, double *delta_time) {

    double distance = this->instance->distances[current_sequence->node->index][next_sequence->node->index];

    if(scan_i == 1) {

        double time_off = max(0.0,next_sequence->node->time_window[0] - this->virtual_sequence->current_time);
        current_sequence->time_off = time_off;
        current_sequence->current_time += time_off;

        if(delta_time!=nullptr) {
            *delta_time+=time_off;
        }

        next_sequence->max_time_off -= *delta_time;

    }else {

        next_sequence->current_distance = current_sequence->current_distance + distance;
        next_sequence->current_time = current_sequence->current_time + distance * this->instance->avg_speed;

        //Considerando apenas um service time do locker
        if(next_sequence->node->id != current_sequence->node->id) {
            next_sequence->current_time += next_sequence->node->service_time;

        }
    }

}

void Search::fill_previous(Sequence *current_sequence, Sequence *next_sequence) {

    double distance = this->instance->distances[current_sequence->node->index][next_sequence->node->index];


    //TODO tratar arredondamentos

    current_sequence->max_time_off = min(next_sequence->max_time_off,
        next_sequence->node->time_window[1] - current_sequence->current_time);
    if(current_sequence->node->id != next_sequence->node->id) {
        current_sequence->max_time_off -= current_sequence->node->service_time;
    }

}

void Search::propagate(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    vector<Sequence>* route = &this->routes.at(route_index);

    Sequence* current_sequence = nullptr;
    Sequence* next_sequence = &route->at(0);
    next_sequence->max_time_off = 0.0;
    next_sequence->time_off = 0.0;

    for(int i=1; i<route->size(); i++) {

        current_sequence = next_sequence;
        next_sequence = &route->at(i);

        fill_next(current_sequence,next_sequence,0,nullptr);

    }

    //Segundo scan, resolvendo max-timeoff
    Sequence* last_sequence = &route->at(route->size()-1);
    last_sequence->max_time_off = last_sequence->node->time_window[1] - last_sequence->current_time;
    current_sequence = nullptr;
    for(int i=route->size()-2;i>=0; i--) {
        current_sequence = &route->at(i);
        next_sequence = &route->at(i+1);

        fill_previous(current_sequence, next_sequence);

    }

    //Terceiro scan, tratando time-off
    double delta_time = 0.0;
    for(int i=1; i<route->size();i++) {
        current_sequence = &route->at(i) - 1;
        next_sequence = &route->at(i);

        fill_next(current_sequence,next_sequence,1, &delta_time);
        delta_time += current_sequence->time_off;

    }

}

bool Search::propagate_virtual(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    vector<Sequence>* route = &this->routes.at(route_index);

    //Primeiro scan, resolvendo distancias e time
    //TODO depois otimizar, talvez nao precise começar do começo
    route->at(0).clone(this->virtual_sequence);
    this->virtual_sequence->time_off = 0.0;
    this->virtual_sequence->max_time_off = 0.0;

    for(int i=1; i<route->size(); i++) {

        Sequence* current_sequence = &route->at(i);

        if(i-1 == previous_sequence_index) {
            fill_next_virtual(cand_sequence,0, nullptr, true);
            if(broke_upper_time_window()) {
                return false;
            }
        }

        fill_next_virtual(current_sequence,0, nullptr, false);
        if(broke_upper_time_window()) {
            return false;
        }

    }

    //Segundo scan, resolvendo max-timeoff. Aqui a virtual_sequence eh sempre o Dt
    virtual_sequence->max_time_off = this->virtual_sequence->node->time_window[1] - this->virtual_sequence->current_time;
    if (broke_timeoff()) {
        return false;
    }

    for(int i=route->size()-2;i>=0; i--) {
        Sequence* previous_sequence = &route->at(i);

        if(i == previous_sequence_index) {

            fill_previous_virtual(cand_sequence, true);
            if(broke_timeoff()) {
                return false;
            }

        }

        fill_previous_virtual(previous_sequence, false);
        if(broke_timeoff()) {
            return false;
        }

    }

    //Terceiro scan, tratando time-off
    double delta_time = 0.0;
    for(int i=1; i<route->size();i++) {
        Sequence* next_sequence = &route->at(i);

        if(i-1 == previous_sequence_index) {
            fill_next_virtual(cand_sequence,1, &delta_time, true);
            if(broke_upper_time_window()) {
                return false;
            }
        }

        fill_next_virtual(next_sequence,1, &delta_time, false);

        if(broke_upper_time_window()) {
            return false;
        }



    }

    return true;
}

bool Search::broke_upper_time_window() {

    return this->virtual_sequence->current_time > this->virtual_sequence->node->time_window[1];
}

bool Search::broke_timeoff() {
    return this->virtual_sequence->current_time > this->virtual_sequence->max_time_off;
}


double Search::delta_distance(tuple<int,int,vector<Sequence>> cus) {
    Sequence a_previous_sequence = this->routes.at(get<0>(cus)).at(get<1>(cus));
    Sequence a_next_sequence = this->routes.at(get<0>(cus)).at(get<1>(cus));
    double distance_prev_next = this->instance->distances[a_previous_sequence.node->index][a_next_sequence.node->index];
    double distance_prev_cus = this->instance->distances[a_previous_sequence.node->index][get<2>(cus).begin()->node->index];
    double distance_cus_next = this->instance->distances[(get<2>(cus).end()-1)->node->index][a_next_sequence.node->index];
    double distance_between = 0.0;


    if (get<2>(cus).size() > 1) {
        distance_between += this->instance->distances[get<2>(cus).begin()->node->index][(get<2>(cus).end()-1)->node->index];
    }

    return distance_prev_cus + distance_cus_next + distance_between -distance_prev_next;
}
bool Search::sort_function(const tuple<int, int, Sequence> cus_a, const tuple<int, int, Sequence> cus_b) {
    Sequence a_previous_sequence = this->routes.at(get<0>(cus_a)).at(get<1>(cus_a));
    Sequence a_next_sequence = this->routes.at(get<0>(cus_a)).at(get<1>(cus_a));

    double distance_prev_next = this->instance->distances[a_previous_sequence.node->index][a_next_sequence.node->index];
    double distance_prev_cus = this->instance->distances[a_previous_sequence.node->index][get<2>(cus_a).node->index];
    double distance_cus_next = this->instance->distances[get<2>(cus_a).node->index][a_next_sequence.node->index];
    double distance_between = 0.0;


    double delta_cus_a = distance_prev_cus + distance_cus_next + distance_between -distance_prev_next;


    Sequence b_previous_sequence = this->routes.at(get<0>(cus_b)).at(get<1>(cus_b));
    Sequence b_next_sequence = this->routes.at(get<0>(cus_b)).at(get<1>(cus_b));

    distance_prev_next = this->instance->distances[b_previous_sequence.node->index][b_next_sequence.node->index];
    distance_prev_cus = this->instance->distances[b_previous_sequence.node->index][get<2>(cus_b).node->index];
    distance_cus_next = this->instance->distances[get<2>(cus_b).node->index][b_next_sequence.node->index];
    distance_between = 0.0;

    double delta_cus_b = distance_prev_cus + distance_cus_next + distance_between -distance_prev_next;

   return delta_cus_a < delta_cus_b;
}
bool Search::sort_function_tw(const tuple<int,int,vector<Sequence>> cus_a, const tuple<int,int,vector<Sequence>> cus_b) {
    Node* a = get<2>(cus_a).at(get<2>(cus_a).size()-1).node;
    Node* b = get<2>(cus_b).at(get<2>(cus_b).size()-1).node;

   return a->time_window[1] < b->time_window[1];
}

void Search::insert_sequency(tuple<int, int, Sequence> candidate) {
    int route_index = get<0>(candidate);
    int previous_sequence_index = get<1>(candidate);
    Sequence* candidate_sequence = &get<2>(candidate);


    this->routes.at(route_index).insert(this->routes.at(route_index).begin()+previous_sequence_index+1,
        1,
        *candidate_sequence);

    propagate(route_index,previous_sequence_index,candidate_sequence);

    this->visited[candidate_sequence->customer->index] = true;

}

void Search::update_forward(tuple<int, int, Sequence> candidate) {
    int route_index = get<0>(candidate);
    int next_sequence_index = get<1>(candidate)+1;
    vector<Sequence> *route = &this->routes.at(route_index);

    Sequence* previous_sequence = &get<2>(candidate);
    for (int i=next_sequence_index;i< route->size();i++) {
        Sequence* sequence = &route->at(i);
        double distance = this->instance->distances[previous_sequence->node->index][sequence->node->index];
        sequence->current_load = previous_sequence->current_load - sequence->node->load_demand;
        sequence->current_distance = previous_sequence->current_distance + distance;

        //Tratando timeoff
        double time_off = max(0.0,(sequence->node->time_window[0] - sequence->current_time));
        sequence->time_off = time_off;
        sequence->current_time = previous_sequence->current_time + this->instance->avg_speed*distance + sequence->node->service_time + time_off;

        previous_sequence = sequence;
    }

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

    vector<string> time_window={};
    int sp_visit=0;
    bool sp_visit_viable = true;
    bool time_window_viable = true;
    bool load_viable = true;
    for (vector<Sequence> route: this->routes) {
        Sequence* seq = &*(route.end()-1);
        if (seq->node->type == "c2") {
            sp_visit_viable = false;
            sp_visit++;
        }
        if (seq->current_load < 0) {
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
    }
    if (!sp_visit_viable) {
        cout<<"Self pickup inviability"<<endl;
    }
    if (!customer_viable) {
        cout<<"Customer inviability ("<<customers.size()<<"): (";
        for(string customer: customers) {
            cout<< customer<<", ";
        }
        cout<<")"<<endl;
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
    }


}

bool Search::is_customer(int node_index) {
    return this->instance->nodes.at(node_index).type[0] == 'c';
}
bool Search::is_recharger_station(int node_index) {
    return this->instance->nodes.at(node_index).type[0] == 'f';
}
bool Search::is_locker(int node_index) {
    return this->instance->nodes.at(node_index).type[0] == 'p';
}

short Search::is_time_window_viable(Sequence* candidate_sequence) {
    Node* cand_node = candidate_sequence->node;

    if (candidate_sequence->max_time_off < 0 || candidate_sequence->current_time > cand_node->time_window[1]) {
        return INVIABLE_TW;
    }

    if (candidate_sequence->current_time < cand_node->time_window[0]) {
        if (candidate_sequence->current_time + candidate_sequence->max_time_off >= cand_node->time_window[0]) {
            return EARLY_TW;
        }
        return INVIABLE_TW;
    }

    return VIABLE_TW;
}
bool Search::is_forward_viable(int route_index, int previous_sequence_index, Sequence *candidate_sequence) {
    vector<Sequence> route = this->routes.at(route_index);

    Sequence virtual_sequence;
    virtual_sequence.node = candidate_sequence->node;
    virtual_sequence.customer = candidate_sequence->customer;
    virtual_sequence.current_time = candidate_sequence->current_time;
    virtual_sequence.max_time_off = candidate_sequence->max_time_off;
    virtual_sequence.time_off = candidate_sequence->time_off;

    Sequence* previous_sequence = candidate_sequence;
    int current_sequence_index = previous_sequence_index+1;
    while (current_sequence_index < (route.size()-1)) { //TODO testar
        Sequence* current_sequence = &route.at(current_sequence_index);
        Sequence* next_sequence = &route.at(current_sequence_index+1);
        virtual_sequence.node = current_sequence->node;
        virtual_sequence.customer = current_sequence->customer;
        accumulate_virtual_sequence(previous_sequence, &virtual_sequence, current_sequence, next_sequence);

        short time_window_viability = is_time_window_viable(&virtual_sequence);

        if (time_window_viability == INVIABLE_TW) {
            return false;
        }

        previous_sequence = current_sequence;
        current_sequence_index++;
    }

    return true;

}


void Search::print() {
    cout<<"TOTAL COST: "<<this->total_cost<<endl;
    cout<<"MIN_V: "<<this->instance->minimum_vehicle<<endl;
    cout<<"PATHS: "<<endl;
    for (int i=0; i<this->n_vehicles; i++) {
        cout<<"V_"<<i<<": ";
        int j=0;
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

string Search::get_delta_to_print(tuple<int, int, vector<Sequence>> cus) {
    Sequence previous_sequence = this->routes.at(get<0>(cus)).at(get<1>(cus));
    Sequence next_sequence = this->routes.at(get<0>(cus)).at(get<1>(cus));

    double distance_prev_next = this->instance->distances[previous_sequence.node->index][next_sequence.node->index];
    double distance_prev_cus = this->instance->distances[previous_sequence.node->index][get<2>(cus).begin()->node->index];
    double distance_cus_next = this->instance->distances[(get<2>(cus).end()-1)->node->index][next_sequence.node->index];
    double distance_between = 0.0;

    if (get<2>(cus).size() > 1) {
        distance_between += this->instance->distances[get<2>(cus).begin()->node->index][(get<2>(cus).end()-1)->node->index];
    }

    double delta_cus_a = distance_prev_cus + distance_cus_next + distance_between -distance_prev_next;

    return to_string(delta_cus_a);

}
