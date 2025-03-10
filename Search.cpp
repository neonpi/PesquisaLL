//
// Created by Rafael on 29/11/2024.
//

#include "Search.h"

#include "Utils.h"


Search::Search(Instance *instance, Config* config) {
    this->instance = instance;
    this->config = config;
    this->best = nullptr;
    
    this->solution = new Solution(instance);
    this->virtual_sequence = new Sequence();

}

Search::~Search() {

    if (this->best != nullptr) {
        delete this->best;
    }

    delete this->virtual_sequence;
}


void Search::run() {
    /*this->construct();
    this->rvnd_inter();
    this->iterated_greedy();*/

}

void Search::debug_run() {
    this->construct();
    this->rvnd_inter();
    /*Utils::test_cost(this->solution);
    Utils::test_print_viability(this->solution,0);*/

}

void Search::construct() {
    this->insertion_heuristic();
}

void Search::insertion_heuristic() {

    vector<tuple<int,int,Sequence,double>> cand_list = build_candidate_list(); //(rota,antecessor,cliente destino)

    while(!cand_list.empty()) {

        //Randomizado
        int candidates = int((int)cand_list.size()*this->config->alpha);

        int rand_index = 0;
        if(candidates>0) {
            rand_index = rand()%candidates;
        }

        tuple<int,int,Sequence,double> candidate = cand_list.at(rand_index);

        insert_sequency(candidate);

        cand_list.erase(cand_list.begin(),cand_list.end());

        cand_list = build_candidate_list();
    }

    this->solution->calculate_total_cost();
}

// void Search::insertion_heuristic_ig() {

//     vector<vector<tuple<int,int,Sequence,double>>> cand_list = build_candidate_list_ig(); //(rota,antecessor,cliente destino)

//     while(!cand_list.empty()) {
//         //tuple<int,int,Sequence,double> candidate = cand_list.at(0); //GULOSO

//         //Randomizado
//         int qty_groups = int((int)cand_list.size()*this->config->alpha);

//         int rand_index = 0;
//         if(qty_groups>2) {
//             rand_index = rand()%qty_groups;
//         }

//         vector<tuple<int,int,Sequence,double>> group = cand_list.at(rand_index);

//         int candidates = (int)group.size();

//         rand_index = 0;
//         if(candidates>2) {
//             rand_index = rand()%candidates;
//         }

//         tuple<int,int,Sequence,double> candidate = group.at(rand_index);

//         insert_sequency(candidate);


//         cand_list.erase(cand_list.begin(),cand_list.end());

//         cand_list = build_candidate_list_ig();
//     }

//     this->solution->calculate_total_cost();
// }

void Search::rvnd_intra() {

    vector<int> neighb = {0,1,2,3,4};
    int last_improved_neighb = -1;
    random_shuffle(neighb.begin(),neighb.end());
    double cost_backup = this->solution->cost;

    for(int i=0;i<5;i++) {
        if(neighb[i] != last_improved_neighb) {
            switch (neighb[i]) {
                case 0:
                    this->ls_intra_2opt();
                break;
                case 1:
                    this->ls_intra_exchange();
                break;
                case 2:
                    this->ls_intra_or_opt_1();
                break;
                case 3:
                    this->ls_intra_or_opt_k(2);
                break;
                case 4:
                    this->ls_intra_or_opt_k(3);
                break;
                default:
                    cout<<"Unknown LS"<<endl;
            }
            if(this->solution->cost < cost_backup) {
                cost_backup = this->solution->cost;
                last_improved_neighb = neighb[i];
                i=-1;
            }
        }
    }
}



void Search::rvnd_inter() {

    this->rvnd_intra();

    vector<int> neighb = {0,1,2,3,4};
    int last_improved_neighb = -1;
    random_shuffle(neighb.begin(),neighb.end());
    double cost_backup = this->solution->cost;

    for(int i=0;i<5;i++) {
        if(neighb[i] != last_improved_neighb) {
            switch (neighb[i]) {
                case 0:
                    this->ls_inter_swap_1_1();
                    break;
                case 1:
                    this->ls_inter_swap_2_1();
                    break;
                case 2:
                    this->ls_inter_swap_2_2();
                    break;
                case 3:
                    this->ls_inter_shift_1_0();
                    break;
                case 4:
                    this->ls_inter_shift_2_0();
                    break;
                default:
                    cout<<"Unknown LS"<<endl;
            }
            this->ls_locker_reducer();
            /*Utils::test_cost(this->solution);
            Utils::test_print_viability(this->solution,0);*/
            if(this->solution->cost < cost_backup) {
                double cost_before_intra = this->solution->cost;
                this->rvnd_intra();
                cost_backup = this->solution->cost;
                if(this->solution->cost < cost_before_intra) {
                    last_improved_neighb = -1;
                }else {
                    last_improved_neighb = neighb[i];
                }
                i=-1;
            }
        }
    }
}



// void Search::iterated_greedy() {
//     Solution* bestSolution = this->solution->clone();
//     int dec_size = 1;
//     int dec_size_limit = this->instance->customers_qty*0.4;

//     int iter_random_limit = 10;
//     int iter_total_limit = this->solution->used_routes + iter_random_limit;


//     while(dec_size<dec_size_limit) {

//         for(int iter=0; iter<iter_total_limit;iter++) {

//             if(iter<iter_random_limit) {
//                 deconstruct_random(dec_size);
//             }else {
//                 deconstruct_route(iter-iter_random_limit);
//             }

//             insertion_heuristic_ig();

//             this->rvnd_inter();

//             if(this->solution->cost < bestSolution->cost) {
//                 delete bestSolution;
//                 bestSolution = this->solution->clone();
//                 iter_total_limit = this->solution->used_routes + iter_random_limit;
//                 dec_size = 0;
//                 break;
//             }else {
//                 delete this->solution;
//                 this->solution = bestSolution->clone();
//             }

//         }

//         dec_size++;

//     }

// }

// void Search::deconstruct_random(int dec_size) {


//     for(int i=0;i<dec_size;i++) {

//         int i_route = rand()%this->solution->used_routes;
//         vector<Sequence>* route = &this->solution->routes.at(i_route);

//         int i_seq = 1 + rand()%((int)route->size()-2);
//         Sequence* seq = &route->at(i_seq);
//         this->solution->visited.at(seq->customer->index) = false;

//         route->erase(route->begin()+i_seq,route->begin()+i_seq+1);

//         if(route->size() == 2) {
//             (route->end()-1)->current_distance = 0.0;
//             (route->end()-1)->current_time = 0.0;
//             (route->end()-1)->current_load = 0.0;
//             iter_swap(this->solution->routes.begin()+i_route,this->solution->routes.begin()+this->solution->used_routes-1);
//             this->solution->used_routes--;
//         }


//     }

//     for(int i=0;i<this->solution->used_routes;i++) {
//         propagate(i,0);
//     }

//     //this->solution->calculate_total_cost();





// }

// void Search::deconstruct_route(int i_route) {
//     vector<Sequence> *route = &this->solution->routes.at(i_route);

//     for(int i_seq = 1; i_seq<(route->size()-1);i_seq++) {
//         this->solution->visited.at(route->at(i_seq).customer->index) = false;
//     }

//     route->erase(route->begin()+1,route->end()-1);

//     route->at(1).current_distance = 0.0;
//     route->at(1).current_time = 0.0;
//     route->at(1).current_load = 0.0;
//     iter_swap(this->solution->routes.begin()+i_route,this->solution->routes.begin()+this->solution->used_routes-1);
//     this->solution->used_routes--;


// }

// void Search::local_search() {

//     double best_cost = this->solution->cost;


//     for(int i_route_a=0;i_route_a<this->instance->max_vehicle;i_route_a++) {
//         vector<Sequence>* route_a = &this->solution->routes.at(i_route_a);
//         if(route_a->size()>2) {

//             for(int i_seq_a=1;i_seq_a<(route_a->size()-1);i_seq_a++) {
//                 Sequence * seq_a = &route_a->at(i_seq_a);
//                 for(int i_route_b=i_route_a;i_route_b<this->instance->max_vehicle;i_route_b++) {
//                     vector<Sequence>* route_b = &this->solution->routes.at(i_route_b);
//                     if(route_b->size()>2) {
//                         for(int i_seq_b = route_a == route_b? i_seq_a+1: 1; i_seq_b<(route_b->size()-1); i_seq_b++) {
//                             Sequence * seq_b = &route_b->at(i_seq_b);
//                             swap_sequence(i_route_a,i_seq_a,i_route_b,i_seq_b);

//                             if(is_viable() && this->solution->cost<best_cost) {
//                                 best_cost = this->solution->cost;
//                                 i_seq_b = (route_b->size()-1);
//                                 i_route_b = this->instance->max_vehicle;
//                                 i_seq_a = (route_a->size()-1);
//                                 i_route_a = -1;

//                             }else {
//                                 swap_sequence(i_route_a,i_seq_a,i_route_b,i_seq_b);
//                             }

//                         }
//                     }

//                 }

//             }

//         }
//     }

// }

// void Search::swap_sequence(int route_a_index, int seq_a_index, int route_b_index, int seq_b_index) {
//     Sequence* sequence_a = &this->solution->routes.at(route_a_index).at(seq_a_index);
//     Sequence* sequence_b = &this->solution->routes.at(route_b_index).at(seq_b_index);

//     this->solution->cost-= this->solution->routes.at(route_a_index).at((int)this->solution->routes.at(route_a_index).size()-1).current_distance;
//     this->solution->cost-= this->solution->routes.at(route_b_index).at((int)this->solution->routes.at(route_b_index).size()-1).current_distance;
//     Node* node_a = sequence_a->node;
//     Node* customer_a = sequence_a->customer;

//     Node* node_b = sequence_b->node;
//     Node* customer_b = sequence_b->customer;

//     sequence_b->node = node_a;
//     sequence_b->customer = customer_a;
//     propagate(route_b_index,seq_b_index-1);

//     sequence_a->node = node_b;
//     sequence_a->customer = customer_b;
//     propagate(route_a_index,seq_a_index-1);

//     this->solution->cost+= this->solution->routes.at(route_a_index).at((int)this->solution->routes.at(route_a_index).size()-1).current_distance;
//     this->solution->cost+= this->solution->routes.at(route_b_index).at((int)this->solution->routes.at(route_b_index).size()-1).current_distance;

// }

//TODO testar
void Search::swap_sequence_intraroute(int route_index, int seq_a_index, int seq_b_index, double delta_distance) {
    Route* route = this->solution->routes.at(route_index);
    Sequence* sequence_a = &route->sequences.at(seq_a_index);
    Sequence* sequence_b = &route->sequences.at(seq_b_index);

    Node* node_a = sequence_a->node;
    vector<Node*> customers_a = sequence_a->customers;

    Node* node_b = sequence_b->node;
    vector<Node*> customers_b = sequence_b->customers;

    sequence_b->node = node_a;
    sequence_b->customers = customers_a;

    sequence_a->node = node_b;
    sequence_a->customers = customers_b;

    route->traveled_distance += delta_distance;
}

//TODO testar
bool Search::is_viable() {
    for(Route* route: this->solution->routes) {

        //Load
        if(route->load > this->instance->load_capacity) {
            return false;
        }

        //TW
        for(int i_seq=0; i_seq<(int)route->sequences.size();i_seq++) {
            Sequence* s = &route->sequences.at(i_seq);
            //TODO verificar se precisa considerar o Differs aqui
            if(s->current_time < s->node->time_window[0] ||  s->current_time > s->node->time_window[1]) {
                return false;
            }
        }
    }

    return true;
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

    sort(cand_list.begin(),cand_list.end(),[this](const tuple<int,int,Sequence, double> cus_a, const tuple<int,int,Sequence, double> cus_b){return get<3>(cus_a) < get<3>(cus_b);});

    return cand_list;
}

vector<vector<tuple<int, int, Sequence, double>>> Search::build_candidate_list_ig() {

    vector<tuple<int,int,Sequence, double>> cand_list = build_candidate_list();

    vector<vector<tuple<int, int, Sequence, double>>> parsed_list = group_by_delta(&cand_list);
    return parsed_list;
}


void Search::try_customer_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node *cand_node) {
    if(!this->solution->served.at(cand_node->index)) {

        Sequence cand_sequence;
        cand_sequence.node = cand_node;
        cand_sequence.customers.push_back(cand_node);
        tuple<int, int, Sequence, double> cand_tuple = {-1,-1,cand_sequence, -1.0};
        double delta = -1.0;


        for (int i_route = 0; i_route<(int)this->solution->routes.size();i_route++) {
            Route* route = this->solution->routes.at(i_route);
            //O Load é viável?
            if (is_load_viable(i_route,cand_node)) {

                for (int i_prev_seq = 0; i_prev_seq<(int)route->sequences.size();i_prev_seq++) {
                    Sequence* previous_sequence = &route->sequences.at(i_prev_seq);
                    if (previous_sequence->node->id != "Dt") {
                        delta = calculate_delta_distance(i_route, i_prev_seq, &cand_sequence);

                        if(get<3>(cand_tuple) == -1.0 || delta < get<3>(cand_tuple)) {

                            bool is_insertion_viable = propagate_virtual(i_route, i_prev_seq, &cand_sequence);

                            if(is_insertion_viable) {
                                get<0>(cand_tuple) = i_route;
                                get<1>(cand_tuple) = i_prev_seq;
                                get<3>(cand_tuple) = delta;
                            }
                        }

                    }
                }

            }

            if((int)route->sequences.size() == 2) {
                break;
            }


        }

        if(get<0>(cand_tuple) != -1.0) {
            cand_list->push_back(cand_tuple);
        }

    }
}


//TODO testar
void Search::try_locker_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node* cand_node) {

    Sequence cand_sequence;
    cand_sequence.node = cand_node;
    tuple<int, int, Sequence, double> cand_tuple = {-1,-1,cand_sequence,-1};
    vector<Node*> cand_customers;
    double delta = -1.0;

    int route_index = 0;
    for (Route* route : this->solution->routes) {

        tuple<int, double> route_cand_tuple = {-1,-1.0};
        vector<Node*> route_cand_customers;

        //Inserindo todos os clientes que podem caber na rota
        double accum_demand = 0;
        for(Node* customer: cand_sequence.node->designated_customers) {
            if(!this->solution->served[customer->index]) {
                if((accum_demand + customer->load_demand + route->load) <= this->instance->load_capacity) {
                    route_cand_customers.push_back(customer);
                    accum_demand += customer->load_demand;
                }else {
                    break;
                }
            }
        }

        // Só prossegue se couber pelo menos um cliente
        if((int)route_cand_customers.size() > 0) {

            for (int i_prev_seq = 0; i_prev_seq < (int)route->sequences.size();i_prev_seq++) {
                Sequence* previous_sequence = &route->sequences.at(i_prev_seq);
                if(previous_sequence->node->id != "Dt") {
                    //Se o locker ja estiver nessa rota, cai fora
                    if(previous_sequence->node->id == cand_node->id) {
                        //Se uma inserção tambem tiver sido considerada, desiste
                        if(get<0>(route_cand_tuple) != 1) {
                            get<0>(route_cand_tuple) = -1;
                        }
                        break;
                    }else {
                        bool is_insertion_viable = propagate_virtual(route_index,i_prev_seq,&cand_sequence);

                        if(is_insertion_viable) {
                            delta = calculate_delta_distance(route_index,i_prev_seq,&cand_sequence);
                            //O delta é melhor que o melhor?
                            if (get<0>(cand_tuple) == -1 || delta < get<3>(cand_tuple)) {

                                //A inserção do route_cand_tuple pode estar ainda melhor
                                if(get<0>(route_cand_tuple) == -1 || delta < get<1>(route_cand_tuple)) {

                                    get<0>(route_cand_tuple) = i_prev_seq;
                                    get<1>(route_cand_tuple) = delta;

                                }

                            }
                        }

                    }

                    i_prev_seq++;
                }

            }

            //Se != de -1 ele achou um candidato melhor, atribuir na tupla mãe
            if(get<0>(route_cand_tuple) != -1) {
                get<0>(cand_tuple) = route_index;
                get<1>(cand_tuple) = get<0>(route_cand_tuple);
                get<3>(cand_tuple) = get<1>(route_cand_tuple);
                cand_customers = route_cand_customers;
            }
        }

        if((int)route->sequences.size() == 2) {
            break;
        }

        route_index++;
    }

    //Se uma boa inserção viável de locker foi encontrada, incluir na lista
    if(get<0>(cand_tuple) != -1) {

        for(Node* customer : cand_customers) {
            get<2>(cand_tuple).customers.push_back(customer);
        }
        cand_list->push_back(cand_tuple);
    }


}


void Search::fill_forward_virtual(Sequence *previous_sequence, Sequence *current_sequence) {

    double distance = this->instance->distances[previous_sequence->node->index][current_sequence->node->index];
    this->virtual_sequence->current_time += distance * this->instance->avg_speed + previous_sequence->node->service_time;

    //Verificando se precisa de timeoff no
    if(this->virtual_sequence->current_time < current_sequence->node->time_window[0]) {
        this->virtual_sequence->time_off = current_sequence->node->time_window[0] - this->virtual_sequence->current_time;
        this->virtual_sequence->current_time += this->virtual_sequence->time_off;
    }

    //TODO depois tirar essas atribuições, porque nao precisa.
    this->virtual_sequence->node = current_sequence->node;
    //TODO this->virtual_sequence->customer = current_sequence->customer;

}

//TODO testar Agora o fill forward só trata o tw
void Search::fill_forward(Sequence *previous_sequence, Sequence *current_sequence) {


    double distance = this->instance->distances[previous_sequence->node->index][current_sequence->node->index];

    current_sequence->current_time = previous_sequence->current_time + previous_sequence->node->service_time + distance * this->instance->avg_speed;

    if(current_sequence->current_time < current_sequence->node->time_window[0]) {
        previous_sequence->time_off = current_sequence->node->time_window[0] - current_sequence->current_time;
        current_sequence->current_time += previous_sequence->time_off;
    }else {
        previous_sequence->time_off = 0.0;
    }


}


void Search::propagate(int route_index, int previous_sequence_index) {
    Route* route = this->solution->routes.at(route_index);
    vector<Sequence> *route_sequences = &route->sequences;

    Sequence* previous_sequence = nullptr;
    Sequence* current_sequence = &route_sequences->at(previous_sequence_index);

    for(int i=previous_sequence_index+1; i<(int)route_sequences->size(); i++) {
        previous_sequence = current_sequence;
        current_sequence = &route_sequences->at(i);
        fill_forward(previous_sequence,current_sequence);

    }

}

//Retorna false se não for viável
bool Search::propagate_virtual(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    Route* route = this->solution->routes.at(route_index);
    vector<Sequence> *route_sequences = &route->sequences;

    route_sequences->at(previous_sequence_index).clone_this_to(this->virtual_sequence);

    Sequence* previous_sequence = &route_sequences->at(previous_sequence_index);
    Sequence* current_sequence = cand_sequence;
    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }

    for(int i=previous_sequence_index+1; i<(int)route_sequences->size(); i++) {

        previous_sequence = current_sequence;
        current_sequence = &route_sequences->at(i);

        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }

    }

    return true;
}

//TODO testar
bool Search::propagate_virtual_exchange(int route_index, int i_seq_a, int i_seq_b) {
    Route* route = this->solution->routes.at(route_index);
    vector<Sequence> *route_sequences = &route->sequences;

    route_sequences->at(i_seq_a-1).clone_this_to(this->virtual_sequence);

    Sequence* previous_sequence = &route_sequences->at(i_seq_a-1);
    Sequence* current_sequence = &route_sequences->at(i_seq_b);
    fill_forward_virtual(previous_sequence,current_sequence);
    if(broke_time_window()) {
        return false;
    }

    //Propagando até o i_seq_b
    for (int i = i_seq_a + 1; i < i_seq_b; i++) {
        previous_sequence = current_sequence;
        current_sequence = &route_sequences->at(i);
        fill_forward_virtual(previous_sequence,current_sequence);
        if(broke_time_window()) {
            return false;
        }
    }

    //propagando de seq_b_previous pra seq_a
    previous_sequence = current_sequence;
    current_sequence = &route_sequences->at(i_seq_a);
    fill_forward_virtual(previous_sequence,current_sequence);
    if(broke_time_window()) {
        return false;
    }
    //TODO Da pra inferir que: Se o tw aqui no virtual, for menor que o real, não vai quebrar a janela de tempo da solução. Não precisa verificar novamente cada tw

    //Propagando de i_seq_a até o resto
    for (int i = i_seq_b + 1; i < (int)route_sequences->size(); i++) {
        previous_sequence = current_sequence;
        current_sequence = &route_sequences->at(i);
        fill_forward_virtual(previous_sequence,current_sequence);
        if(broke_time_window()) {
            return false;
        }
    }

    return true;
}

//TODO testar
bool Search::propagate_virtual_2opt(int route_index, int i_seq_a, int i_seq_b) {
    Route* route = this->solution->routes.at(route_index);
    vector<Sequence> *route_sequences = &route->sequences;

    route_sequences->at(i_seq_a).clone_this_to(this->virtual_sequence);

    //Trocando o proximo cliente de i_seq_a pelo último do intervalo (i_seq_b)
    Sequence* previous_sequence = &route_sequences->at(i_seq_a);
    Sequence* current_sequence = &route_sequences->at(i_seq_b-1);
    fill_forward_virtual(previous_sequence,current_sequence);
    if(broke_time_window()) {
        return false;
    }

    //Calculando o propagate para o intervalo de forma inversa
    for(int i = i_seq_b-2;i>i_seq_a;i--) {
        previous_sequence = current_sequence;
        current_sequence = &route_sequences->at(i);
        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }
    }

    //Calculando de i_seq_b em diante
    for(int i=i_seq_b;i<(int)route_sequences->size();i++) {
        previous_sequence = current_sequence;
        current_sequence = &route_sequences->at(i);
        fill_forward_virtual(previous_sequence, current_sequence);
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

vector<vector<tuple<int, int, Sequence, double>>> Search::group_by_delta(
    vector<tuple<int, int, Sequence, double>> *cand_list) {

    vector<vector<tuple<int, int, Sequence, double>>> all_group_list;

    for(int i_cand_list = 0;i_cand_list<(int)cand_list->size();i_cand_list++) {
        vector<tuple<int, int, Sequence, double>> group_list;

        group_list.push_back(cand_list->at(i_cand_list++));

        while(i_cand_list < (int) cand_list->size() &&
            get<3>(*(group_list.end()-1)) == get<3>(cand_list->at(i_cand_list))) {
                group_list.push_back(cand_list->at(i_cand_list++));
        }

        all_group_list.push_back(group_list);

        i_cand_list--;

    }


    return all_group_list;
}

//TODO testar
double Search::calculate_delta_distance(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    Route* route = this->solution->routes.at(route_index);
    vector<Sequence> *route_sequences = &route->sequences;

    Sequence a_previous_sequence = route_sequences->at(previous_sequence_index);
    Sequence a_next_sequence = route_sequences->at(previous_sequence_index+1);

    double distance_prev_next = this->instance->distances[a_previous_sequence.node->index][a_next_sequence.node->index];
    double distance_prev_cus = this->instance->distances[a_previous_sequence.node->index][cand_sequence->node->index];
    double distance_cus_next = this->instance->distances[cand_sequence->node->index][a_next_sequence.node->index];

    double delta_distance = distance_prev_cus + distance_cus_next - distance_prev_next;
    return delta_distance;
}


void Search::insert_sequency(tuple<int, int, Sequence, double> candidate) {
    Route* route = this->solution->routes.at(get<0>(candidate));
    vector<Sequence> *route_sequences = &route->sequences;

    int previous_sequence_index = get<1>(candidate);
    Sequence* previous_sequence = &route_sequences->at(previous_sequence_index);
    Sequence* candidate_sequence = &get<2>(candidate);

    //Atualizando o load minimo da rota
    double lower_demmand = -1.0;
    //TODO testar para locker
    for(Node* customer: candidate_sequence->customers) {
        if(customer->load_demand < lower_demmand || lower_demmand == -1.0) {
            lower_demmand = customer->load_demand;
        }
    }

    if(route_sequences->size() == 2 || lower_demmand < route->minimun_route_load) {
        route->minimun_route_load = lower_demmand;
    }

    if((int)route_sequences->size() == 2){ this->solution->used_routes++; }

    if(previous_sequence->node->id == candidate_sequence->node->id) { //TODO isso aqui provavelmente nunca vai acontecer.
        previous_sequence->customers.push_back(candidate_sequence->customers.at(0));
        propagate(get<0>(candidate), previous_sequence_index-1);
    }else {
        route_sequences->insert(route_sequences->begin()+previous_sequence_index+1,1, *candidate_sequence);
        propagate(get<0>(candidate), previous_sequence_index);
    }

    //TODO Atualizando aqui agora o load da rota e a distancia. Verificar se vai funcionar
    for(Node* customer : candidate_sequence->customers) {
        route->load += customer->load_demand;
        this->solution->served.at(customer->index) = true;
    }

    double delta = get<3>(candidate);
    route->traveled_distance += delta;

    if(candidate_sequence->node->type == "p") {
        route->visited_lockers[candidate_sequence->node] += 1;
    }

}

bool Search::is_customer(int node_index) {
    return this->instance->nodes.at(node_index).type[0] == 'c';
}
bool Search::is_locker(int node_index) {
    return this->instance->nodes.at(node_index).type[0] == 'p';
}

// void Search::print_ig_candidate_list(vector<vector<tuple<int, int, Sequence, double>>> *cand_list) {
//     for(vector<tuple<int,int,Sequence, double>> cand_group: *cand_list) {
//         for(tuple<int,int,Sequence, double> cand: cand_group) {
//             int cand_route = get<0>(cand);
//             int cand_index = get<1>(cand);
//             Sequence *cand_sequence = &get<2>(cand);

//             if(cand_sequence->node!=cand_sequence->customer) {
//                 cout<<cand_route<<" - "<<cand_index<<" - ("<<cand_sequence->customer->id<<")"<<cand_sequence->node->id<<" - "<<get<3>(cand)<<" | ";
//             }else {
//                 cout<<cand_route<<" - "<<cand_index<<" - "<<cand_sequence->node->id<<" - "<<get<3>(cand)<<" | ";
//             }
//         }
//         cout<<endl;
//     }
//     cout<<endl;
// }

void Search::shift(vector<Sequence> *route_sequences, int i_seq_a, int i_seq_b) {

    if (i_seq_a<i_seq_b) {
        route_sequences->insert(route_sequences->begin()+i_seq_b,route_sequences->begin()+i_seq_a,route_sequences->begin()+i_seq_a+1);
        route_sequences->erase(route_sequences->begin()+i_seq_a,route_sequences->begin()+i_seq_a+1);
    }else {
        route_sequences->insert(route_sequences->begin()+i_seq_b,route_sequences->begin()+i_seq_a+1,route_sequences->begin()+i_seq_a+2);
        route_sequences->erase(route_sequences->begin()+i_seq_a+1,route_sequences->begin()+i_seq_a+2);
    }
}

void Search::shift_k(int k, vector<Sequence> *route_sequences, int i_seq_a, int i_seq_b) {

    if (i_seq_a<i_seq_b) {
        rotate(route_sequences->begin() + i_seq_a, route_sequences->begin() + i_seq_a + k, route_sequences->begin() + i_seq_b );
    }else {
        rotate(route_sequences->begin() + i_seq_b, route_sequences->begin() + i_seq_a, route_sequences->begin() + i_seq_a + k );    }

}

void Search::build_predefined_solution(vector<vector<string>> solution) {

    /*for(int i_solution_route = 0; i_solution_route < (int)solution.size(); i_solution_route++) {
        vector<string> solution_route = solution.at(i_solution_route);
        for(int i_solution_node = 0, i_insertion = 0; i_solution_node < (int)solution_route.size(); i_solution_node++) {
            string route_el = solution_route.at(i_solution_node);
            vector<string> parsed_route_el = Utils::tookenize(route_el,"-");

            Node* node = this->instance->find_node_per_id(parsed_route_el.at(0));
            Node* customer = nullptr;
            if((int) parsed_route_el.size() > 1) {
                customer = this->instance->find_node_per_id(parsed_route_el.at(1));
            }else {
                customer = node;
            }

            this->solution->served[customer->index] = true;
            Route* route = this->solution->routes.at(i_solution_route);
            vector<Sequence>* route_sequences = &route->sequences;

            if((route_sequences->end()-2)->node->id == node->id) {
                (route_sequences->end()-2)->customers.push_back(customer);
            }else {
                Sequence s;
                s.node = node;
                s.customers.push_back(customer);
                route_sequences->insert(route_sequences->begin() + i_insertion++ + 1,1,s);
            }

        }

        propagate(i_solution_route,0);
    }
    this->solution->used_routes = (int)solution.size();
    this->solution->calculate_total_cost();
    this->solution->print();
    cout<<"\n-----------TESTING SOLUTION:--------"<<endl<<endl;
    //this->solution->print_is_viable(2);
    cout<<endl;*/

}

