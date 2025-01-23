//
// Created by Rafael on 29/11/2024.
//

#include "Search.h"

#include "Utils.h"

Search::Search(Instance *instance, Config* config) {
    this->instance = instance;
    this->config = config;
    this->n_vehicles = instance->max_vehicle;
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

void Search::run() {
    this->construct();
    this->rvnd_inter();
}

void Search::construct() {
    this->insertion_heuristic();
}

void Search::insertion_heuristic() {

    vector<tuple<int,int,Sequence,double>> cand_list = build_candidate_list(); //(rota,antecessor,cliente destino)
    vector<tuple<int,int,Sequence,double>>::iterator it;

    while(!cand_list.empty()) {

        //tuple<int,int,Sequence,double> candidate = cand_list.at(0); //GULOSO

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

    calculate_total_cost();
}

void Search::rvnd_intra() {

    vector<int> neighb = {0,1};
    int last_improved_neighb = -1;
    random_shuffle(neighb.begin(),neighb.end());
    double cost_backup = this->total_cost;

    for(int i=0;i<2;i++) {
        if(neighb[i] != last_improved_neighb) {
            switch (neighb[i]) {
                case 0:
                    this->ls_intra_2opt();
                break;
                case 1:
                    this->ls_intra_exchange();
                break;
                default:
                    cout<<"Unknown LS"<<endl;
            }
            if(this->total_cost < cost_backup) {
                cost_backup = this->total_cost;
                last_improved_neighb = neighb[i];
                i=-1;
            }
        }
    }
}

void Search::ls_intra_exchange() {
    double best_cost = this->total_cost;

    for(int i_route=0; i_route<(int)this->routes.size();i_route++) {
        vector<Sequence>* route = &this->routes.at(i_route);
        if(route->size()>4) {
            for(int i_seq_a=2;i_seq_a<(route->size()-2);i_seq_a++) {

                Sequence* s_a = &route->at(i_seq_a-1);

                for(int i_seq_b = i_seq_a+1; i_seq_b<(route->size()-2);i_seq_b++) {
                    Sequence* s_b = &route->at(i_seq_b);

                    if(s_a->node->id != s_b->node->id) {
                        swap_sequence_intraroute(i_route,i_seq_a,i_seq_b);
                        //test_cost();
                        if(this->total_cost<best_cost && Utils::differs(this->total_cost,best_cost) && is_viable()) {
                            best_cost = this->total_cost;
                            i_seq_a = 1;

                            break;
                        }

                        swap_sequence_intraroute(i_route,i_seq_a,i_seq_b);
                        //test_cost();

                    }

                }



            }
        }
    }
}

void Search::ls_intra_2opt() {


    for (int i_route = 0;i_route<(int)this->routes.size();i_route++) {

        double best_delta = 0.0;
        int coordinates[2] = {-1,-1}; //i_seq_a,i_seq_b

        vector<Sequence>* route = &this->routes.at(i_route);
           for(int i_seq_a = 0; i_seq_a<((int)(route->size())-3);i_seq_a++) {
               Sequence* seq_a = &route->at(i_seq_a);

               for(int i_seq_b = i_seq_a+3; i_seq_b<(int)route->size();i_seq_b++) {
                   Sequence* seq_b = &route->at(i_seq_b);

                   //Inverter a rota inteira não faz efeito algum
                   if(seq_a->node->id =="D0" && seq_b->node->id == "Dt") {break;}

                   double delta = calculate_delta_2opt(route,i_seq_a, i_seq_b);

                   if(Utils::improves(0.0,delta) &&
                       Utils::improves(best_delta,delta)) {
                        //Testar viabilidade
                       if(propagate_virtual_2opt(i_route,i_seq_a,i_seq_b)) {
                           best_delta = delta;
                           coordinates[0] = i_seq_a;
                           coordinates[1] = i_seq_b;
                       }
                   }

               }


           }

            if(best_delta<0.0) {
                reverse(route->begin()+coordinates[0]+1,route->begin()+coordinates[1]);
                propagate(i_route,coordinates[0]);
                calculate_total_cost();
                i_route--;
            }

    }

}

void Search::rvnd_inter() {
    vector<int> neighb = {0,1,2,3};
    int last_improved_neighb = -1;
    random_shuffle(neighb.begin(),neighb.end());
    double cost_backup = this->total_cost;

    for(int i=0;i<4;i++) {
        if(neighb[i] != last_improved_neighb) {
            switch (neighb[i]) {
                case 0:
                    this->ls_inter_shift_1_0();
                break;
                case 1:
                    this->ls_inter_shift_2_0();
                break;
                case 2:
                    this->ls_inter_swap_1_1();
                break;
                case 3:
                    this->ls_inter_swap_2_1();
                break;
                default:
                    cout<<"Unknown LS"<<endl;
            }
            if(this->total_cost < cost_backup) {
                this->rvnd_intra();
                cost_backup = this->total_cost;
                last_improved_neighb = neighb[i];
                i=-1;
            }
        }
    }
}

void Search::ls_inter_shift_1_0() {
    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    vector<Sequence> * route_a = nullptr;
    vector<Sequence> * route_b = nullptr;
    Sequence * seq_a = nullptr;
    Sequence * seq_b = nullptr;
    for (int i_route_a = 0;i_route_a<(int)this->routes.size();i_route_a++) {
        route_a = &this->routes.at(i_route_a);

        if((int)route_a->size() > 2) {
            for (int i_route_b = 0;i_route_b<(int)this->routes.size();i_route_b++) {

                if(i_route_a!=i_route_b) {
                    route_b = &this->routes.at(i_route_b);

                    //Verificando se pelo menos um cliente da rota A cabe na rota B
                    if((int)route_b->size() > 2 && ((route_b->end()-1)->current_load + (route_a->end()-1)->minimun_route_load) <= this->instance->load_capacity) {
                        //Aponta pro nó de A que vai
                        for(int i_seq_a = 1; i_seq_a<((int)route_a->size()-1);i_seq_a++) {
                            seq_a = &route_a->at(i_seq_a);

                            //Verificando se o nó da seq A cabe na rota B
                            if((route_b->end()-1)->current_load + seq_a->customer->load_demand <= this->instance->load_capacity) {

                                //Aponta pro nó de B que vai vir antes
                                for(int i_seq_b = 0; i_seq_b<((int)route_b->size()-1);i_seq_b++) {
                                    //this->print();
                                    seq_b = &route_b->at(i_seq_b); //TODO depois remover
                                    double delta = calculate_delta_shift_1_0(route_a,i_seq_a,route_b,i_seq_b);
                                    if(Utils::improves(0.0,delta) &&
                                        Utils::improves(best_delta,delta)) {

                                        if(propagate_virtual(i_route_b,i_seq_b,seq_a)) {
                                            best_delta = delta;
                                            coordinates[0] = i_route_a;
                                            coordinates[1] = i_seq_a;
                                            coordinates[2] = i_route_b;
                                            coordinates[3] = i_seq_b;
                                        }

                                    }
                                }


                            }


                        }

                    }

                }
            }
        }
    }

    if(best_delta<0.0) {
        route_a = &this->routes.at(coordinates[0]);
        seq_a = &route_a->at(coordinates[1]);
        route_b = &this->routes.at(coordinates[2]);
        seq_b = &route_b->at(coordinates[3]);

        route_b->insert(route_b->begin()+coordinates[3]+1,1,*seq_a);
        propagate(coordinates[2],coordinates[3]);
        //Verificando se o load minimo da rota B vai ser atualizado
        if(seq_a->customer->load_demand < (route_a->end()-1)->minimun_route_load) {
            (route_a->end()-1)->minimun_route_load = seq_a->customer->load_demand;
        }

        route_a->erase(route_a->begin()+coordinates[1],route_a->begin()+coordinates[1]+1);
        propagate(coordinates[0],coordinates[1]-1);

        //Reajustando a demanda mínima da rota que foi reduzida
        if(seq_a->customer->load_demand == (route_a->end()-1)->minimun_route_load) {
            (route_a->end()-1)->minimun_route_load = route_a->at(1).customer->load_demand;
            for(int i=2;i<((int)route_a->size()-1);i++) {
                if(route_a->at(0).customer->load_demand < (route_a->end()-1)->minimun_route_load) {
                    (route_a->end()-1)->minimun_route_load = route_a->at(0).customer->load_demand;
                }
            }
        }

        this->calculate_total_cost();
    }
}

void Search::ls_inter_shift_2_0() {
    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    vector<Sequence> * route_a = nullptr;
    vector<Sequence> * route_b = nullptr;
    Sequence * seq_a_1 = nullptr;
    Sequence * seq_a_2 = nullptr;
    Sequence * seq_b = nullptr;
    for (int i_route_a = 0;i_route_a<(int)this->routes.size();i_route_a++) {
        route_a = &this->routes.at(i_route_a);

        if((int)route_a->size() > 3) {
            for (int i_route_b = 0;i_route_b<(int)this->routes.size();i_route_b++) {

                if(i_route_a!=i_route_b) {
                    route_b = &this->routes.at(i_route_b);

                    //Verificando se pelo menos um cliente da rota A cabe na rota B
                    if((int)route_b->size() > 2) {
                        //Aponta pro nó de A que vai
                        for(int i_seq_a = 1; i_seq_a<((int)route_a->size()-2);i_seq_a++) {
                            seq_a_1 = &route_a->at(i_seq_a);
                            seq_a_2 = &route_a->at(i_seq_a+1);

                            //Verificando se os nós da seq A cabe na rota B
                            if((route_b->end()-1)->current_load + seq_a_1->customer->load_demand + seq_a_2->customer->load_demand <= this->instance->load_capacity) {

                                //Aponta pro nó de B que vai vir antes
                                for(int i_seq_b = 0; i_seq_b<((int)route_b->size()-1);i_seq_b++) {
                                    //this->print();
                                    seq_b = &route_b->at(i_seq_b); //TODO depois remover
                                    double delta = calculate_delta_shift_2_0(route_a,i_seq_a,route_b,i_seq_b);
                                    if(Utils::improves(0.0,delta) &&
                                        Utils::improves(best_delta,delta)) {

                                        if(propagate_virtual_segment(i_route_b,i_seq_b,seq_a_1,seq_a_2)) {
                                            best_delta = delta;
                                            coordinates[0] = i_route_a;
                                            coordinates[1] = i_seq_a;
                                            coordinates[2] = i_route_b;
                                            coordinates[3] = i_seq_b;
                                        }

                                    }
                                }


                            }


                        }

                    }

                }
            }
        }
    }

    if(best_delta<0.0) {
        route_a = &this->routes.at(coordinates[0]);
        seq_a_1 = &route_a->at(coordinates[1]);
        seq_a_2 = &route_a->at(coordinates[1]+1);
        route_b = &this->routes.at(coordinates[2]);
        seq_b = &route_b->at(coordinates[3]);


        route_b->insert(route_b->begin()+coordinates[3]+1,route_a->begin()+coordinates[1],route_a->begin()+coordinates[1]+2);
        propagate(coordinates[2],coordinates[3]);

        //Verificando se o load minimo da rota B vai ser atualizado
        Sequence* sequence_min_load = seq_a_1->customer->load_demand < seq_a_2->customer->load_demand? seq_a_1: seq_a_2;
        if(sequence_min_load->customer->load_demand < (route_a->end()-1)->minimun_route_load) {
            (route_a->end()-1)->minimun_route_load = sequence_min_load->customer->load_demand;
        }

        route_a->erase(route_a->begin()+coordinates[1],route_a->begin()+coordinates[1]+2);
        propagate(coordinates[0],coordinates[1]-1);

        //Reajustando a demanda mínima da rota que foi reduzida
        if(seq_a_1->customer->load_demand == (route_a->end()-1)->minimun_route_load || seq_a_2->customer->load_demand == (route_a->end()-1)->minimun_route_load) {
            (route_a->end()-1)->minimun_route_load = route_a->at(1).customer->load_demand;
            for(int i=2;i<((int)route_a->size()-1);i++) {
                if(route_a->at(0).customer->load_demand < (route_a->end()-1)->minimun_route_load) {
                    (route_a->end()-1)->minimun_route_load = route_a->at(0).customer->load_demand;
                }
            }
        }

        this->calculate_total_cost();
    }
}
bool Search::swap_1_1_broke_load(vector<Sequence>* route_a, Sequence* seq_a , vector<Sequence>* route_b, Sequence* seq_b) {

    bool broke_a = (route_a->end()-1)->current_load  - seq_a->customer->load_demand + seq_b->customer->load_demand > this->instance->load_capacity;
    bool broke_b = (route_b->end()-1)->current_load  - seq_b->customer->load_demand + seq_a->customer->load_demand > this->instance->load_capacity;
    return broke_a || broke_b;

}
void Search::ls_inter_swap_1_1() {
    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    vector<Sequence> * route_a = nullptr;
    vector<Sequence> * route_b = nullptr;
    Sequence * seq_a = nullptr;
    Sequence * seq_b = nullptr;

    for (int i_route_a = 0;i_route_a<((int)this->routes.size()-1);i_route_a++) {
        route_a = &this->routes.at(i_route_a);
        if(route_a->size()>2) {
            for (int i_route_b = i_route_a+1;i_route_b<(int)this->routes.size();i_route_b++) {
                route_b = &this->routes.at(i_route_b);

                if(route_b->size()>2) {

                    for(int i_seq_a=1; i_seq_a<((int)route_a->size()-1);i_seq_a++) {
                        seq_a = &route_a->at(i_seq_a);

                        for(int i_seq_b = 1; i_seq_b<((int)route_b->size()-1);i_seq_b++) {
                            seq_b = &route_b->at(i_seq_b);

                            //Trocar locker com locker não faz efeito
                            if(seq_a->node->id != seq_b->node->id) {

                                if(!swap_1_1_broke_load(route_a,seq_a,route_b,seq_b)) {
                                    double delta = calculate_delta_swap_1_1(route_a,i_seq_a,route_b,i_seq_b);
                                    if(Utils::improves(0.0,delta) &&
                                        Utils::improves(best_delta,delta)) {
                                        if(propagate_virtual_swap_1_1(i_route_b,i_seq_b-1,seq_a) &&
                                            propagate_virtual_swap_1_1(i_route_a,i_seq_a-1,seq_b)) {
                                            best_delta = delta;
                                            coordinates[0] = i_route_a;
                                            coordinates[1] = i_seq_a;
                                            coordinates[2] = i_route_b;
                                            coordinates[3] = i_seq_b;
                                        }
                                    }

                                }

                            }

                        }
                    }
                }
            }
        }
    }

    if(best_delta<0.0) {
        route_a = &this->routes.at(coordinates[0]);
        seq_a = &route_a->at(coordinates[1]);
        route_b = &this->routes.at(coordinates[2]);
        seq_b = &route_b->at(coordinates[3]);

        route_b->insert(route_b->begin()+coordinates[3]+1,1,*seq_a);


        //Verificando se o load minimo da rota B vai ser atualizado
        if(seq_b->customer->load_demand < (route_a->end()-1)->minimun_route_load) {
            (route_a->end()-1)->minimun_route_load = seq_b->customer->load_demand;
        }

        route_a->insert(route_a->begin()+coordinates[1]+1,1,*seq_b);


        //Verificando se o load minimo da rota B vai ser atualizado
        if(seq_a->customer->load_demand < (route_b->end()-1)->minimun_route_load) {
            (route_b->end()-1)->minimun_route_load = seq_a->customer->load_demand;
        }

        route_a->erase(route_a->begin()+coordinates[1],route_a->begin()+coordinates[1]+1);
        propagate(coordinates[0],coordinates[1]-1);


        //Reajustando a demanda mínima da rota que foi reduzida
        if(seq_a->customer->load_demand == (route_a->end()-1)->minimun_route_load && seq_b->customer->load_demand != (route_a->end()-1)->minimun_route_load) {
            (route_a->end()-1)->minimun_route_load = route_a->at(1).customer->load_demand;
            for(int i=2;i<((int)route_a->size()-1);i++) {
                if(route_a->at(0).customer->load_demand < (route_a->end()-1)->minimun_route_load) {
                    (route_a->end()-1)->minimun_route_load = route_a->at(0).customer->load_demand;
                }
            }
        }

        route_b->erase(route_b->begin()+coordinates[3],route_b->begin()+coordinates[3]+1);
        propagate(coordinates[2],coordinates[3]-1);

        //Reajustando a demanda mínima da rota que foi reduzida
        if(seq_b->customer->load_demand == (route_a->end()-1)->minimun_route_load && seq_a->customer->load_demand != (route_b->end()-1)->minimun_route_load) {
            (route_b->end()-1)->minimun_route_load = route_b->at(1).customer->load_demand;
            for(int i=2;i<((int)route_b->size()-1);i++) {
                if(route_b->at(0).customer->load_demand < (route_b->end()-1)->minimun_route_load) {
                    (route_b->end()-1)->minimun_route_load = route_b->at(0).customer->load_demand;
                }
            }
        }

        this->calculate_total_cost();
    }

}
bool Search::swap_2_1_broke_load(vector<Sequence>* route_a, Sequence* seq_a_1, Sequence *seq_a_2, vector<Sequence>* route_b, Sequence* seq_b) {

    bool broke_a = (route_a->end()-1)->current_load  - seq_a_1->customer->load_demand - seq_a_2->customer->load_demand + seq_b->customer->load_demand > this->instance->load_capacity;
    bool broke_b = (route_b->end()-1)->current_load  - seq_b->customer->load_demand + seq_a_1->customer->load_demand + seq_a_2->customer->load_demand > this->instance->load_capacity;
    return broke_a || broke_b;

}
void Search::ls_inter_swap_2_1() {
    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    vector<Sequence> * route_a = nullptr;
    vector<Sequence> * route_b = nullptr;
    Sequence * seq_a_1 = nullptr;
    Sequence * seq_a_2 = nullptr;
    Sequence * seq_b = nullptr;

    for (int i_route_a = 0;i_route_a<(int)this->routes.size();i_route_a++) {
        route_a = &this->routes.at(i_route_a);
        if(route_a->size()>3) {
            for (int i_route_b = 0;i_route_b<(int)this->routes.size();i_route_b++) {

                if(i_route_b != i_route_a) {
                    route_b = &this->routes.at(i_route_b);
                    if(route_b->size()>2) {
                        for(int i_seq_a=1; i_seq_a<((int)route_a->size()-2);i_seq_a++) {
                            seq_a_1 = &route_a->at(i_seq_a);
                            seq_a_2 = &route_a->at(i_seq_a+1);

                            for(int i_seq_b = 1; i_seq_b<((int)route_b->size()-1);i_seq_b++) {
                                seq_b = &route_b->at(i_seq_b);

                                //Trocar locker com locker não faz efeito TODO testar depois
                                    if(!swap_2_1_broke_load(route_a,seq_a_1,seq_a_2,route_b,seq_b)) {
                                        double delta = calculate_delta_swap_2_1(route_a,i_seq_a,route_b,i_seq_b);
                                        if(Utils::improves(0.0,delta) &&
                                            Utils::improves(best_delta,delta)) {
                                            if(propagate_virtual_swap_1_2(i_route_a,i_seq_a-1,seq_b) &&
                                                propagate_virtual_swap_2_1(i_route_b,i_seq_b-1,seq_a_1,seq_a_2)) {
                                                best_delta = delta;
                                                coordinates[0] = i_route_a;
                                                coordinates[1] = i_seq_a;
                                                coordinates[2] = i_route_b;
                                                coordinates[3] = i_seq_b;
                                                }
                                            }

                                    }

                            }
                        }
                    }
                }
            }
        }
    }

    if(best_delta<0.0) {
        route_a = &this->routes.at(coordinates[0]);
        seq_a_1 = &route_a->at(coordinates[1]);
        seq_a_2 = &route_a->at(coordinates[1]+1);
        route_b = &this->routes.at(coordinates[2]);
        seq_b = &route_b->at(coordinates[3]);

        route_b->insert(route_b->begin()+coordinates[3]+1,route_a->begin()+coordinates[1],route_a->begin()+coordinates[1]+2);

        //Verificando se o load minimo da rota B vai ser atualizado
        if(seq_a_1->customer->load_demand < (route_b->end()-1)->minimun_route_load || seq_a_2->customer->load_demand < (route_b->end()-1)->minimun_route_load) {
            (route_b->end()-1)->minimun_route_load = min(seq_a_1->customer->load_demand,seq_a_2->customer->load_demand);
        }

        route_a->insert(route_a->begin()+coordinates[1]+2,1,*seq_b);

        //Verificando se o load minimo da rota B vai ser atualizado
        if(seq_b->customer->load_demand < (route_a->end()-1)->minimun_route_load) {
            (route_b->end()-1)->minimun_route_load = seq_b->customer->load_demand;
        }

        route_a->erase(route_a->begin()+coordinates[1],route_a->begin()+coordinates[1]+2);
        propagate(coordinates[0],coordinates[1]-1);

        //Reajustando a demanda mínima da rota que foi reduzida
        if((seq_a_1->customer->load_demand == (route_a->end()-1)->minimun_route_load || seq_a_2->customer->load_demand == (route_a->end()-1)->minimun_route_load) && seq_b->customer->load_demand != (route_a->end()-1)->minimun_route_load) {
            (route_a->end()-1)->minimun_route_load = route_a->at(1).customer->load_demand;
            for(int i=2;i<((int)route_a->size()-1);i++) {
                if(route_a->at(0).customer->load_demand < (route_a->end()-1)->minimun_route_load) {
                    (route_a->end()-1)->minimun_route_load = route_a->at(0).customer->load_demand;
                }
            }
        }

        route_b->erase(route_b->begin()+coordinates[3],route_b->begin()+coordinates[3]+1);
        propagate(coordinates[2],coordinates[3]-1);

        //Reajustando a demanda mínima da rota que foi reduzida
        if(seq_b->customer->load_demand == (route_a->end()-1)->minimun_route_load && (seq_a_1->customer->load_demand != (route_b->end()-1)->minimun_route_load || seq_a_1->customer->load_demand != (route_b->end()-1)->minimun_route_load)) {
            (route_b->end()-1)->minimun_route_load = route_b->at(1).customer->load_demand;
            for(int i=2;i<((int)route_b->size()-1);i++) {
                if(route_b->at(0).customer->load_demand < (route_b->end()-1)->minimun_route_load) {
                    (route_b->end()-1)->minimun_route_load = route_b->at(0).customer->load_demand;
                }
            }
        }

        this->calculate_total_cost();
    }
}

double Search::calculate_delta_2opt(vector<Sequence>* route, int i_seq_a, int i_seq_b) {
    Sequence* seq_a = &route->at(i_seq_a);
    Sequence* slice_first = &route->at(i_seq_a+1);
    Sequence* seq_b = &route->at(i_seq_b);
    Sequence* slice_last = &route->at(i_seq_b-1);
    double delta = 0.0;
    delta += this->instance->distances[seq_a->node->index][slice_last->node->index];
    delta += this->instance->distances[slice_first->node->index][seq_b->node->index];
    delta -= this->instance->distances[seq_a->node->index][slice_first->node->index];
    delta -= this->instance->distances[slice_last->node->index][seq_b->node->index];

    return delta;
}

double Search::calculate_delta_shift_1_0(vector<Sequence> *route_a, int i_seq_a, vector<Sequence> *route_b, int i_seq_b) {
    Sequence* seq_a_previous = &route_a->at(i_seq_a-1);
    Sequence* seq_a = &route_a->at(i_seq_a);
    Sequence* seq_a_next = &route_a->at(i_seq_a+1);

    Sequence* seq_b = &route_b->at(i_seq_b);
    Sequence* seq_b_next = &route_b->at(i_seq_b+1);

    double delta = 0.0;

    //Retirando a seq a da rota a
    delta+= this->instance->distances[seq_a_previous->node->index][seq_a_next->node->index];
    delta-= this->instance->distances[seq_a_previous->node->index][seq_a->node->index];
    delta-= this->instance->distances[seq_a->node->index][seq_a_next->node->index];

    //Retirando a seq a na rota b
    delta+= this->instance->distances[seq_b->node->index][seq_a->node->index];
    delta+= this->instance->distances[seq_a->node->index][seq_b_next->node->index];
    delta-= this->instance->distances[seq_b->node->index][seq_b_next->node->index];


    return delta;

}

double Search::calculate_delta_shift_2_0(vector<Sequence> *route_a, int i_seq_a, vector<Sequence> *route_b,
    int i_seq_b) {

    Sequence* seq_a_previous = &route_a->at(i_seq_a-1);
    Sequence* seq_a_1 = &route_a->at(i_seq_a);
    Sequence* seq_a_2 = &route_a->at(i_seq_a+1);
    Sequence* seq_a_next = &route_a->at(i_seq_a+2);

    Sequence* seq_b = &route_b->at(i_seq_b);
    Sequence* seq_b_next = &route_b->at(i_seq_b+1);

    double delta = 0.0;

    delta+= this->instance->distances[seq_a_previous->node->index][seq_a_next->node->index];
    delta-= this->instance->distances[seq_a_previous->node->index][seq_a_1->node->index];
    delta-= this->instance->distances[seq_a_2->node->index][seq_a_next->node->index];

    delta+= this->instance->distances[seq_b->node->index][seq_a_1->node->index];
    delta+= this->instance->distances[seq_a_2->node->index][seq_b_next->node->index];
    delta-= this->instance->distances[seq_b->node->index][seq_b_next->node->index];

    return delta;
}

double Search::calculate_delta_swap_1_1(vector<Sequence> *route_a, int i_seq_a, vector<Sequence> *route_b,
    int i_seq_b) {
    Sequence* seq_a_previous = &route_a->at(i_seq_a-1);
    Sequence* seq_a = &route_a->at(i_seq_a);
    Sequence* seq_a_next = &route_a->at(i_seq_a+1);

    Sequence* seq_b_previous = &route_b->at(i_seq_b-1);
    Sequence* seq_b = &route_b->at(i_seq_b);
    Sequence* seq_b_next = &route_b->at(i_seq_b+1);

    double delta = 0.0;

    //Contabilizando alteração da rota_a
    delta+=this->instance->distances[seq_a_previous->node->index][seq_b->node->index];
    delta+=this->instance->distances[seq_b->node->index][seq_a_next->node->index];
    delta-=this->instance->distances[seq_a_previous->node->index][seq_a->node->index];
    delta-=this->instance->distances[seq_a->node->index][seq_a_next->node->index];

    //Contabilizando alteração da rota_b
    delta+=this->instance->distances[seq_b_previous->node->index][seq_a->node->index];
    delta+=this->instance->distances[seq_a->node->index][seq_b_next->node->index];
    delta-=this->instance->distances[seq_b_previous->node->index][seq_b->node->index];
    delta-=this->instance->distances[seq_b->node->index][seq_b_next->node->index];

    return delta;

}

double Search::calculate_delta_swap_2_1(vector<Sequence> *route_a, int i_seq_a, vector<Sequence> *route_b,
    int i_seq_b) {
    Sequence* seq_a_previous = &route_a->at(i_seq_a-1);
    Sequence* seq_a_1 = &route_a->at(i_seq_a);
    Sequence* seq_a_2 = &route_a->at(i_seq_a+1);
    Sequence* seq_a_next = &route_a->at(i_seq_a+2);

    Sequence* seq_b_previous = &route_b->at(i_seq_b-1);
    Sequence* seq_b = &route_b->at(i_seq_b);
    Sequence* seq_b_next = &route_b->at(i_seq_b+1);

    double delta = 0.0;

    //Contabilizando alteração da rota_a
    delta+=this->instance->distances[seq_a_previous->node->index][seq_b->node->index];
    delta+=this->instance->distances[seq_b->node->index][seq_a_next->node->index];
    delta-=this->instance->distances[seq_a_previous->node->index][seq_a_1->node->index];
    delta-=this->instance->distances[seq_a_2->node->index][seq_a_next->node->index];

    //Contabilizando alteração da rota_b
    delta+=this->instance->distances[seq_b_previous->node->index][seq_a_1->node->index];
    delta+=this->instance->distances[seq_a_2->node->index][seq_b_next->node->index];
    delta-=this->instance->distances[seq_b_previous->node->index][seq_b->node->index];
    delta-=this->instance->distances[seq_b->node->index][seq_b_next->node->index];

    return delta;

}

void Search::local_search() {
    //this->print();
    double best_cost = this->total_cost;


    for(int i_route_a=0;i_route_a<this->n_vehicles;i_route_a++) {
        vector<Sequence>* route_a = &this->routes.at(i_route_a);
        if(route_a->size()>2) {
            for(int i_seq_a=1;i_seq_a<(route_a->size()-1);i_seq_a++) {
                Sequence * seq_a = &route_a->at(i_seq_a);
                for(int i_route_b=i_route_a;i_route_b<this->n_vehicles;i_route_b++) {
                    vector<Sequence>* route_b = &this->routes.at(i_route_b);
                    if(route_b->size()>2) {
                        for(int i_seq_b = route_a == route_b? i_seq_a+1: 1; i_seq_b<(route_b->size()-1); i_seq_b++) {
                            Sequence * seq_b = &route_b->at(i_seq_b);
                            swap_sequence(i_route_a,i_seq_a,i_route_b,i_seq_b);

                            if(is_viable() && this->total_cost<best_cost) {
                                best_cost = this->total_cost;
                                i_seq_b = (route_b->size()-1);
                                i_route_b = this->n_vehicles;
                                i_seq_a = (route_a->size()-1);
                                i_route_a = -1;

                            }else {
                                swap_sequence(i_route_a,i_seq_a,i_route_b,i_seq_b);
                            }

                        }
                    }

                }

            }

        }
    }

}

void Search::swap_sequence(int route_a_index, int seq_a_index, int route_b_index, int seq_b_index) {
    Sequence* sequence_a = &this->routes.at(route_a_index).at(seq_a_index);
    Sequence* sequence_b = &this->routes.at(route_b_index).at(seq_b_index);

    this->total_cost-= this->routes.at(route_a_index).at((int)this->routes.at(route_a_index).size()-1).current_distance;
    this->total_cost-= this->routes.at(route_b_index).at((int)this->routes.at(route_b_index).size()-1).current_distance;
    Node* node_a = sequence_a->node;
    Node* customer_a = sequence_a->customer;

    Node* node_b = sequence_b->node;
    Node* customer_b = sequence_b->customer;

    sequence_b->node = node_a;
    sequence_b->customer = customer_a;
    propagate(route_b_index,seq_b_index-1);

    sequence_a->node = node_b;
    sequence_a->customer = customer_b;
    propagate(route_a_index,seq_a_index-1);

    this->total_cost+= this->routes.at(route_a_index).at((int)this->routes.at(route_a_index).size()-1).current_distance;
    this->total_cost+= this->routes.at(route_b_index).at((int)this->routes.at(route_b_index).size()-1).current_distance;

}

void Search::swap_sequence_intraroute(int route_index, int seq_a_index, int seq_b_index) {
    Sequence* sequence_a = &this->routes.at(route_index).at(seq_a_index);
    Sequence* sequence_b = &this->routes.at(route_index).at(seq_b_index);

    this->total_cost-= this->routes.at(route_index).at((int)this->routes.at(route_index).size()-1).current_distance;

    Node* node_a = sequence_a->node;
    Node* customer_a = sequence_a->customer;

    Node* node_b = sequence_b->node;
    Node* customer_b = sequence_b->customer;

    sequence_b->node = node_a;
    sequence_b->customer = customer_a;

    sequence_a->node = node_b;
    sequence_a->customer = customer_b;

    propagate(route_index,seq_a_index<seq_b_index?(seq_a_index-1):(seq_b_index-1));

    this->total_cost+= this->routes.at(route_index).at((int)this->routes.at(route_index).size()-1).current_distance;
}

bool Search::is_viable() {
    for(vector<Sequence> seq: this->routes) {
        //Load
        if(seq.at((int)seq.size()-1).current_load > this->instance->load_capacity) {
            return false;
        }
        //TW
        for(Sequence s: seq) {
            if(s.current_time<s.node->time_window[0] ||  s.current_time>s.node->time_window[1]) {
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
    this->virtual_sequence->customer = current_sequence->customer;

}

void Search::fill_forward(Sequence *previous_sequence, Sequence *current_sequence) {

    current_sequence->current_load = previous_sequence->current_load + current_sequence->customer->load_demand;

    double distance = this->instance->distances[previous_sequence->node->index][current_sequence->node->index];
    current_sequence->current_distance = previous_sequence->current_distance + distance;

    current_sequence->current_time = previous_sequence->current_time + previous_sequence->node->service_time + distance * this->instance->avg_speed;

    if(current_sequence->current_time < current_sequence->node->time_window[0]) {
        previous_sequence->time_off = current_sequence->node->time_window[0] - current_sequence->current_time;
        current_sequence->current_time += previous_sequence->time_off;
    }


}

void Search::propagate(int route_index, int previous_sequence_index) {
    vector<Sequence>* route = &this->routes.at(route_index);

    Sequence* previous_sequence = nullptr;
    Sequence* current_sequence = &route->at(previous_sequence_index);

    for(int i=previous_sequence_index+1; i<route->size(); i++) {

        previous_sequence = current_sequence;
        current_sequence = &route->at(i);
        fill_forward(previous_sequence,current_sequence);

    }

}
//Retorna false se não for viável
bool Search::propagate_virtual(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    vector<Sequence>* route = &this->routes.at(route_index);

    route->at(previous_sequence_index).clone(this->virtual_sequence);

    Sequence* previous_sequence = &route->at(previous_sequence_index);
    Sequence* current_sequence = cand_sequence;
    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }

    for(int i=previous_sequence_index+1; i<(int)route->size(); i++) {

        previous_sequence = current_sequence;
        current_sequence = &route->at(i);

        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }

    }

    return true;
}

bool Search::propagate_virtual_segment(int route_index, int previous_sequence_index, Sequence* cand_sequence_1, Sequence* cand_sequence_2) {
    vector<Sequence>* route = &this->routes.at(route_index);

    route->at(previous_sequence_index).clone(this->virtual_sequence);
    //Propagando primeiro nó
    Sequence* previous_sequence = &route->at(previous_sequence_index);
    Sequence* current_sequence = cand_sequence_1;

    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }

    //Propagando segundo nó
    previous_sequence = current_sequence;
    current_sequence = cand_sequence_2;

    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }

    for(int i=previous_sequence_index+1; i<(int)route->size(); i++) {

        previous_sequence = current_sequence;
        current_sequence = &route->at(i);

        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }

    }

    return true;
}

bool Search::propagate_virtual_2opt(int route_index, int i_seq_a, int i_seq_b) {
    vector<Sequence>* route = &this->routes.at(route_index);

    route->at(i_seq_a).clone(this->virtual_sequence);

    //Trocando o proximo cliente de i_seq_a pelo último do intervalo (i_seq_b)
    Sequence* previous_sequence = &route->at(i_seq_a);
    Sequence* current_sequence = &route->at(i_seq_b-1);
    fill_forward_virtual(previous_sequence,current_sequence);
    if(broke_time_window()) {
        return false;
    }

    //Calculando o propagate para o intervalo de forma inversa
    for(int i = i_seq_b-2;i>i_seq_a;i--) {
        previous_sequence = current_sequence;
        current_sequence = &route->at(i);
        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }
    }

    //Calculando de i_seq_b em diante
    for(int i=i_seq_b;i<(int)route->size();i++) {
        previous_sequence = current_sequence;
        current_sequence = &route->at(i);
        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }

    }

    return true;
}

bool Search::propagate_virtual_swap_1_1(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    vector<Sequence>* route = &this->routes.at(route_index);

    route->at(previous_sequence_index).clone(this->virtual_sequence);

    Sequence* previous_sequence = &route->at(previous_sequence_index);
    Sequence* current_sequence = cand_sequence;
    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }

    for(int i=previous_sequence_index+2; i<(int)route->size(); i++) {

        previous_sequence = current_sequence;
        current_sequence = &route->at(i);

        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }

    }

    return true;
}

bool Search::propagate_virtual_swap_1_2(int route_index, int previous_sequence_index, Sequence *cand_sequence) {
    vector<Sequence>* route = &this->routes.at(route_index);

    route->at(previous_sequence_index).clone(this->virtual_sequence);

    //Propagando nó de a
    Sequence* previous_sequence = &route->at(previous_sequence_index);
    Sequence* current_sequence = cand_sequence;
    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }

    //TODO testar
    for(int i=previous_sequence_index+3; i<(int)route->size(); i++) {

        previous_sequence = current_sequence;
        current_sequence = &route->at(i);

        fill_forward_virtual(previous_sequence, current_sequence);
        if(broke_time_window()) {
            return false;
        }

    }

    return true;
}

bool Search::propagate_virtual_swap_2_1(int route_index, int previous_sequence_index, Sequence *cand_sequence_1,
                                        Sequence *cand_sequence_2) {
    vector<Sequence>* route = &this->routes.at(route_index);

    route->at(previous_sequence_index).clone(this->virtual_sequence);

    //Propagando primeiro nó
    Sequence* previous_sequence = &route->at(previous_sequence_index);
    Sequence* current_sequence = cand_sequence_1;
    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }

    //Propagando segundo nó
    previous_sequence = current_sequence;
    current_sequence = cand_sequence_2;
    fill_forward_virtual(previous_sequence, current_sequence);
    if(broke_time_window()) {
        return false;
    }
    //TODO testar
    for(int i=previous_sequence_index+2; i<(int)route->size(); i++) {

        previous_sequence = current_sequence;
        current_sequence = &route->at(i);

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
    vector<Sequence>* route = &this->routes.at(get<0>(candidate));
    int previous_sequence_index = get<1>(candidate);
    Sequence* candidate_sequence = &get<2>(candidate);

    if(route->size() == 2 || candidate_sequence->customer->load_demand < (route->end()-1)->minimun_route_load) {
        (route->end()-1)->minimun_route_load = candidate_sequence->customer->load_demand;
    }

    route->insert(route->begin()+previous_sequence_index+1,1, *candidate_sequence);

    propagate(get<0>(candidate), previous_sequence_index);

    this->visited[candidate_sequence->customer->index] = true;

}


void Search::calculate_total_cost() {
    this->total_cost = 0.0;
    for (vector<Sequence> route : this->routes) {
        this->total_cost += (route.end()-1)->current_distance;
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
        cout<<"Customer inviability ("<<(int)customers.size()<<"): (";
        for(string customer: customers) {
            cout<< customer<<", ";
        }
        cout<<")"<<endl;
        //exit(2);
    }

    if(!time_window_viable) {
        cout<<"Time window inviability ("<<(int)time_window.size()<<"): (";
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
    int used_vehicles = 0;
    vector<string> routes_string;

    for (int i=0; i<this->n_vehicles; i++) {
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

void Search::print_candidate_list(vector<tuple<int, int, Sequence, double>> *cand_list) {
    for(tuple<int,int,Sequence, double> cand: *cand_list) {
        int cand_route = get<0>(cand);
        int cand_index = get<1>(cand);
        Sequence *cand_sequence = &get<2>(cand);
        cout<<cand_route<<" - "<<cand_index<<" - "<<cand_sequence->node->id<<" - "<<get<3>(cand)<<endl;;
    }
    cout<<endl;
}

void Search::test_cost() {
    double cost = 0.0;
    for(vector<Sequence> vs: this->routes) {
        cost+= vs.at((int)vs.size()-1).current_distance;
    }

    if( Utils::differs(cost,this->total_cost)) {
        cout<<"Erro de custo (E:"<<cost<<" G:"<<this->total_cost<<")"<< endl;
        exit(8);
    }
}

