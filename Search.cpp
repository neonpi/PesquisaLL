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

}

Search::~Search() {

    if (this->best != nullptr) {
        delete this->best;
    }

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
        depot_0.current_load = this->instance->load_capacity;
        depot_t.current_load = this->instance->load_capacity;
        depot_0.max_time_off = depot_0.node->time_window[1];
        depot_t.max_time_off = depot_t.node->time_window[1];

        vector<Sequence> route ={depot_0, depot_t};
        this->routes.push_back(route);
        this->routes.at(i).reserve(this->instance->n_node);
    }
}

void Search::construct_3() {
    this->insertion_heuristic();
}

void Search::insertion_heuristic() {

    vector<tuple<int,int,vector<Sequence>>> cand_list; //(rota,antecessor,cliente destino)
    vector<tuple<int,int,vector<Sequence>>>::iterator it;

    do {

        //buscando aresta p insercao
        if (!cand_list.empty()) {
            //int index = 0;
            tuple<int,int,vector<Sequence>> candidate = cand_list.at(0);
            /*while (delta_distance(candidate) == delta_distance(cand_list.at(index)) && index < cand_list.size()) {
                tuple<int,int,vector<Sequence>> candidate_aux = cand_list.at(index);
                if (get<2>(candidate_aux).at(get<2>(candidate_aux).size()-1).node->time_window[0] <
                    get<2>(candidate).at(get<2>(candidate).size()-1).node->time_window[0]) {
                    candidate = candidate_aux;
                }
                index++;
            }*/

            //tuple<int,int,Node*> cand = cand_list.at(rand()%cand_list.size()); //Só pra testar
            insert_sequency(candidate);
            Utils::print_output(this);
        }
        cand_list.erase(cand_list.begin(),cand_list.end());

        cand_list = build_candidate_list();

        /*for (tuple<int,int,vector<Sequence>>  t: cand_list) {
            cout<<delta_distance(t)<<" ";
        }cout<<endl;

        cout<<"LIST: "<<endl;
        for (tuple<int,int,vector<Sequence>> cand_el : cand_list) {
                cout<<"rota:"<<get<0>(cand_el)<<" "<<"point:"<<get<1>(cand_el)<<" {";
            for (Sequence s: get<2>(cand_el)) {
                cout<<s.node->id<<"("<<s.customer->id<<") - ";
            }
            cout<<"}"<<endl;
        }
        cout<<endl;*/



        /*for (it=cand_list.begin(); it!=cand_list.end(); ++it)
            cout << ' ' << this->instance->distances[(this->routes.at(get<0>(*it)).end()-2)->node->index][get<2>(*it)->index];
        cout<<endl;*/

    }while (!cand_list.empty());

    this->total_cost = 0.0;
    for (vector<Sequence> route : this->routes) {
        this->total_cost += (route.end()-1)->current_distance;
    }
}

vector<tuple<int,int,vector<Sequence>>> Search::build_candidate_list() {

    vector<tuple<int,int,vector<Sequence>>> cand_list;

    for (int i=this->instance->customer_indexes[0];
            i<this->instance->locker_indexes[1];i++) {

        Node* can_node = &this->instance->nodes.at(i);
        if (this->instance->nodes.at(i).type == "p") {

        }else if (this->instance->nodes.at(i).type != "c2") {
        }

            }

    //funcao lambda
    sort(cand_list.begin(),cand_list.end(),[this](const tuple<int,int,vector<Sequence>> cus_a, const tuple<int,int,vector<Sequence>> cus_b){return sort_function(cus_a,cus_b);});
    //sort(cand_list.begin(),cand_list.end(),[this](const tuple<int,int,vector<Sequence>> cus_a, const tuple<int,int,vector<Sequence>> cus_b){return sort_function_tw(cus_a,cus_b);});



    return cand_list;
}


void Search::fill_candidate_sequences(Sequence *previous_sequence, Sequence *cand_sequence, Node *cand_node, Node *customer_node) {

    Node* previous_node = previous_sequence->node;
    double distance = this->instance->distances[previous_node->index][cand_node->index];
    cand_sequence->node = cand_node;
    cand_sequence->customer = customer_node;
    cand_sequence->current_time = previous_sequence->current_time + this->instance->avg_speed*(distance);

    if (previous_sequence->node->type =="p") {
        if (previous_sequence->node->id != cand_node->id) {
            cand_sequence->current_time += cand_node->service_time;
        }
    }else {
        cand_sequence->current_time += cand_node->service_time;
    }
    ;
    cand_sequence->current_distance = previous_sequence->current_distance + distance;
    cand_sequence->current_load = previous_sequence->current_load - customer_node->load_demand;

    if (cand_node->type == "p") {
        cand_sequence->method = 'p';
    }else {
        cand_sequence->method = 'h';
    }
}

void Search::fill_candidate_sequences_2(Sequence *previous_sequence, Sequence *next_sequence, Sequence *cand_sequence,
    Node *cand_node, Node *customer_node) {
    //Se next_sequence == nullptr ==> cand_sequence eh estação
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
    //Se o nó passado a entrar foi estação, preencher o timeoff da estação considerando o q foi feito para o cliente de agora
    if (previous_node->type == "f") {
        previous_sequence->max_time_off = min(cand_node->time_window[1], cand_sequence->max_time_off) - previous_sequence->current_time;
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
bool Search::sort_function(const tuple<int,int,vector<Sequence>> cus_a, const tuple<int,int,vector<Sequence>> cus_b) {
    Sequence a_previous_sequence = this->routes.at(get<0>(cus_a)).at(get<1>(cus_a));
    Sequence a_next_sequence = this->routes.at(get<0>(cus_a)).at(get<1>(cus_a));

    double distance_prev_next = this->instance->distances[a_previous_sequence.node->index][a_next_sequence.node->index];
    double distance_prev_cus = this->instance->distances[a_previous_sequence.node->index][get<2>(cus_a).begin()->node->index];
    double distance_cus_next = this->instance->distances[(get<2>(cus_a).end()-1)->node->index][a_next_sequence.node->index];
    double distance_between = 0.0;

    if (get<2>(cus_a).size() > 1) {
        distance_between += this->instance->distances[get<2>(cus_a).begin()->node->index][(get<2>(cus_a).end()-1)->node->index];
    }

    double delta_cus_a = distance_prev_cus + distance_cus_next + distance_between -distance_prev_next;


    Sequence b_previous_sequence = this->routes.at(get<0>(cus_b)).at(get<1>(cus_b));
    Sequence b_next_sequence = this->routes.at(get<0>(cus_b)).at(get<1>(cus_b));

    distance_prev_next = this->instance->distances[b_previous_sequence.node->index][b_next_sequence.node->index];
    distance_prev_cus = this->instance->distances[b_previous_sequence.node->index][get<2>(cus_b).begin()->node->index];
    distance_cus_next = this->instance->distances[(get<2>(cus_b).end()-1)->node->index][b_next_sequence.node->index];
    distance_between = 0.0;

    if (get<2>(cus_b).size() > 1) {
        distance_between += this->instance->distances[get<2>(cus_b).begin()->node->index][(get<2>(cus_b).end()-1)->node->index];
    }

    double delta_cus_b = distance_prev_cus + distance_cus_next + distance_between -distance_prev_next;

   return delta_cus_a < delta_cus_b;
}
bool Search::sort_function_tw(const tuple<int,int,vector<Sequence>> cus_a, const tuple<int,int,vector<Sequence>> cus_b) {
    Node* a = get<2>(cus_a).at(get<2>(cus_a).size()-1).node;
    Node* b = get<2>(cus_b).at(get<2>(cus_b).size()-1).node;

   return a->time_window[1] < b->time_window[1];
}

void Search::insert_sequency(tuple<int,int,vector<Sequence>> candidate) {
    int route_index = get<0>(candidate);
    int previous_sequence_index = get<1>(candidate);
    vector<Sequence> candidate_sequences = get<2>(candidate);

    this->update_forward(candidate);

    this->routes.at(route_index).insert(this->routes.at(route_index).begin()+previous_sequence_index+1,
        candidate_sequences.begin(),
        candidate_sequences.end());
    this->visited[(candidate_sequences.end()-1)->customer->index] = true;

}

void Search::update_forward(tuple<int,int,vector<Sequence>> candidate) {
    int route_index = get<0>(candidate);
    int next_sequence_index = get<1>(candidate)+1;
    vector<Sequence> *route = &this->routes.at(route_index);

    Sequence* previous_sequence = &get<2>(candidate).at(get<2>(candidate).size()-1);
    for (int i=next_sequence_index;i< route->size();i++) {
        Sequence* sequence = &route->at(i);
        double distance = this->instance->distances[previous_sequence->node->index][sequence->node->index];
        sequence->current_load = previous_sequence->current_load - sequence->node->load_demand;
        sequence->current_distance = previous_sequence->current_distance + distance;
        sequence->current_time = previous_sequence->current_time + this->instance->avg_speed*distance + sequence->node->service_time;

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

    int customers = 0;
    for (int i = this->instance->customer_indexes[0];
        i < this->instance->customer_indexes[1] ;i++) {
        if (!this->visited[i]) {
            customer_viable = false;
            customers++;

        }
    }

    int time_window=0;
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
                time_window ++;
            }
        }
    }



    if (!load_viable) {
        cout<<"Load inviability"<<endl;
    }
    if (!customer_viable) {
        cout<<"Customer inviability ("<<customers<<")"<<endl;
    }
    if (!sp_visit_viable) {
        cout<<"Customer inviability ("<<sp_visit<<")"<<endl;
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
bool Search::is_forward_viable(int route_index, int previous_sequence_index, vector<Sequence> *cand_sequence) {
    vector<Sequence> route = this->routes.at(route_index);
    Sequence* candidate_sequence = &(*(cand_sequence->end()-1));

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
