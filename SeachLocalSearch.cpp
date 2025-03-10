//
// Created by Rafael on 09/03/2025.
//

#include "Search.h"
//LS INTRA ROUTE
void Search::ls_intra_exchange() {

    for(int i_route=0; i_route<(int)this->solution->routes.size();i_route++) {
        Route* route = this->solution->routes.at(i_route);
        vector<Sequence> *route_sequences = &route->sequences;
        if(route->sequences.size()>4) {

            double best_delta = 0.0;
            int coordinates[2] = {-1,-1}; //i_seq_a,i_seq_b

            for(int i_seq_a=1;i_seq_a<(int)(route_sequences->size()-2);i_seq_a++) {

                Sequence* seq_a = &route_sequences->at(i_seq_a);

                for(int i_seq_b = i_seq_a+1; i_seq_b<(int)(route_sequences->size()-1);i_seq_b++) {
                    Sequence* seq_b = &route_sequences->at(i_seq_b);

                    if(seq_a->node->id != seq_b->node->id) {
                        double delta = calculate_delta_exchange(route_sequences, i_seq_a,i_seq_b);

                        if(Count::improves(0.0,delta) &&
                        Count::improves(best_delta,delta)) {
                            //Testar viabilidade
                            if(propagate_virtual_exchange(i_route,i_seq_a,i_seq_b)) {
                                best_delta = delta;
                                coordinates[0] = i_seq_a;
                                coordinates[1] = i_seq_b;
                            }

                        }
                    }

                }

            }

            if(best_delta<0.0) {
                swap_sequence_intraroute(i_route,coordinates[0],coordinates[1], best_delta);
                propagate(i_route,coordinates[0]-1);
                this->solution->cost+=best_delta;
                i_route--;
            }
        }
    }
}


void Search::ls_intra_2opt() {

    for (int i_route = 0;i_route<(int)this->solution->routes.size();i_route++) {

        double best_delta = 0.0;
        int coordinates[2] = {-1,-1}; //i_seq_a,i_seq_b

        Route* route = this->solution->routes.at(i_route);
        vector<Sequence>* route_sequences = &route->sequences;
           for(int i_seq_a = 0; i_seq_a<((int)(route_sequences->size())-3);i_seq_a++) {
               Sequence* seq_a = &route_sequences->at(i_seq_a);

               for(int i_seq_b = i_seq_a+5; i_seq_b<(int)route_sequences->size();i_seq_b++) {
                   Sequence* seq_b = &route_sequences->at(i_seq_b);

                   //Inverter a rota inteira não faz efeito algum
                   if(seq_a->node->id =="D0" && seq_b->node->id == "Dt") {break;}

                   double delta = calculate_delta_2opt(route_sequences,i_seq_a, i_seq_b);

                   if(Count::improves(0.0,delta) &&
                       Count::improves(best_delta,delta)) {
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
                reverse(route_sequences->begin()+coordinates[0]+1,route_sequences->begin()+coordinates[1]);
                propagate(i_route,coordinates[0]);
                route->traveled_distance += best_delta;
                this->solution->cost += best_delta;
                i_route--;
            }

    }

}

void Search::ls_intra_or_opt_1() {
    for (int i_route = 0;i_route<(int)this->solution->routes.size();i_route++) {

        double best_delta = 0.0;
        int coordinates[2] = {-1,-1}; //i_seq_a,i_seq_b

        Route* route = this->solution->routes.at(i_route);
        vector<Sequence>* route_sequences = &route->sequences;
        for(int i_seq_a = 1; i_seq_a<((int)(route_sequences->size())-2);i_seq_a++) {
            Sequence* seq_a = &route_sequences->at(i_seq_a);
            Sequence* seq_b = nullptr;
            //Seq_b é o indice do cara que vem depois do novo destino do seq_a. Seq_b eh empurrado

            for(int i_seq_b = 1; i_seq_b<(int)(route_sequences->size());i_seq_b++) {
                //Não faz sentido testar com i=j ou com o vizinho mais proximo (Vizinhança ja eh contemplada pelo exchange)
                if (abs(i_seq_a-i_seq_b)>1) {
                    seq_b = &route_sequences->at(i_seq_b);
                    double delta = calculate_delta_or_opt_1(route_sequences,i_seq_a,i_seq_b);
                    if(Count::improves(0.0,delta) &&
                        Count::improves(best_delta,delta)) {

                        if (i_seq_a<i_seq_b) {
                            if (propagate_virtual_or_opt_1_up(i_route,i_seq_a,i_seq_b)) {
                                best_delta = delta;
                                coordinates[0] = i_seq_a;
                                coordinates[1] = i_seq_b;
                            }
                        }else {
                            if (propagate_virtual_or_opt_1_down(i_route,i_seq_a,i_seq_b)) {
                                best_delta = delta;
                                coordinates[0] = i_seq_a;
                                coordinates[1] = i_seq_b;
                            }
                        }

                    }
                }
            }

        }


        if (best_delta < 0.0) {
            shift(route_sequences,coordinates[0],coordinates[1]);
            propagate(i_route,min(coordinates[0],coordinates[1])-1);
            route->traveled_distance += best_delta;
            this->solution->cost += best_delta;
            i_route--;
        }

    }
}

void Search::ls_intra_or_opt_k(int k) {

    for (int i_route = 0;i_route<(int)this->solution->routes.size();i_route++) {

        Route* route = this->solution->routes.at(i_route);
        vector<Sequence>* route_sequences = &route->sequences;

        if (route_sequences->size()>(k+2)) {

            double best_delta = 0.0;
            int coordinates[2] = {-1,-1}; //i_seq_a,i_seq_b

            for(int i_seq_a = 1; i_seq_a<((int)(route_sequences->size())-k);i_seq_a++) {
                Sequence* seq_a = &route_sequences->at(i_seq_a);
                Sequence* seq_b = nullptr;
                //Pra traz
                for (int i_seq_b = i_seq_a-1; i_seq_b>0;i_seq_b--) {
                    seq_b = &route_sequences->at(i_seq_b);

                    double delta = calculate_delta_or_opt_k(k,route_sequences,i_seq_a,i_seq_b);

                    if(Count::improves(0.0,delta) &&
                        Count::improves(best_delta,delta)) {
                        if (propagate_virtual_or_opt_k_down(k,i_route,i_seq_a,i_seq_b)) {
                            best_delta = delta;
                            coordinates[0] = i_seq_a;
                            coordinates[1] = i_seq_b;
                        }

                    }
                }

                //Pra frente
                for(int i_seq_b = i_seq_a+k+1; i_seq_b<(int)route_sequences->size();i_seq_b++) {
                    seq_b = &route_sequences->at(i_seq_b);

                    double delta = calculate_delta_or_opt_k(k,route_sequences,i_seq_a,i_seq_b);

                    if(Count::improves(0.0,delta) &&
                        Count::improves(best_delta,delta)) {
                        if (propagate_virtual_or_opt_k_up(k,i_route,i_seq_a,i_seq_b)) {
                            best_delta = delta;
                            coordinates[0] = i_seq_a;
                            coordinates[1] = i_seq_b;
                        }
                    }

                }
            }
            if (best_delta < 0.0) {
                shift_k(k,route_sequences,coordinates[0],coordinates[1]);
                propagate(i_route,min(coordinates[0],coordinates[1])-1);
                route->traveled_distance += best_delta;
                this->solution->cost+= best_delta;
                i_route--;
            }
        }


    }
}
//LS INTER ROUTE

void Search::ls_inter_swap_1_1() {
    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    Route* route_a = nullptr;
    Route* route_b = nullptr;
    vector<Sequence> * route_a_sequences = nullptr;
    vector<Sequence> * route_b_sequences = nullptr;
    Sequence * seq_a = nullptr;
    Sequence * seq_b = nullptr;
    for (int i_route_a = 0;i_route_a<((int)this->solution->routes.size()-1);i_route_a++) {
        route_a = this->solution->routes.at(i_route_a);
        route_a_sequences = &route_a->sequences;
        if(route_a_sequences->size()>2) {
            for (int i_route_b = i_route_a+1;i_route_b<(int)this->solution->routes.size();i_route_b++) {
                route_b = this->solution->routes.at(i_route_b);
                route_b_sequences = &route_b->sequences;

                if(route_b_sequences->size()>2) {

                    for(int i_seq_a=1; i_seq_a<((int)route_a_sequences->size()-1);i_seq_a++) {
                        seq_a = &route_a_sequences->at(i_seq_a);

                        for(int i_seq_b = 1; i_seq_b<((int)route_b_sequences->size()-1);i_seq_b++) {
                            seq_b = &route_b_sequences->at(i_seq_b);

                            //Trocar locker com locker não faz efeito
                            if(seq_a->node->id != seq_b->node->id) {

                                if(!swap_1_1_broke_load(route_a,seq_a,route_b,seq_b)) {
                                    double delta = calculate_delta_swap_1_1(route_a_sequences,i_seq_a,route_b_sequences,i_seq_b,BOTH);
                                    if(Count::improves(0.0,delta) &&
                                        Count::improves(best_delta,delta)) {
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
                }else {
                    break;
                }
            }
        }else {
            break;
        }
    }

    if(best_delta<0.0) {
        persist_swap_1_1(coordinates,best_delta);
    }

}

void Search::ls_inter_swap_2_2() {
    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    Route * route_a = nullptr;
    Route * route_b = nullptr;
    vector<Sequence> * route_a_sequences = nullptr;
    vector<Sequence> * route_b_sequences = nullptr;
    Sequence * seq_a_1 = nullptr;
    Sequence * seq_a_2 = nullptr;
    Sequence * seq_b_1 = nullptr;
    Sequence * seq_b_2 = nullptr;

    for (int i_route_a = 0;i_route_a<(int)this->solution->routes.size();i_route_a++) {
        route_a = this->solution->routes.at(i_route_a);
        route_a_sequences = &route_a->sequences;
        if(route_a_sequences->size()>3) {
            for (int i_route_b = i_route_a + 1;i_route_b<(int)this->solution->routes.size();i_route_b++) {

                if(i_route_b != i_route_a) {
                    route_b = this->solution->routes.at(i_route_b);
                    route_b_sequences = &route_b->sequences;
                    if(route_b_sequences->size()>3) {
                        for(int i_seq_a=1; i_seq_a<((int)route_a_sequences->size()-2);i_seq_a++) {
                            seq_a_1 = &route_a_sequences->at(i_seq_a);
                            seq_a_2 = &route_a_sequences->at(i_seq_a+1);

                            for(int i_seq_b = 1; i_seq_b<((int)route_b_sequences->size()-2);i_seq_b++) {
                                seq_b_1 = &route_b_sequences->at(i_seq_b);
                                seq_b_2 = &route_b_sequences->at(i_seq_b+1);

                                //Trocar locker com locker não faz efeito TODO testar depois
                                    if(!swap_2_2_broke_load(route_a,seq_a_1,seq_a_2,route_b,seq_b_1,seq_b_2)) {
                                        double delta = calculate_delta_swap_2_2(route_a_sequences,i_seq_a,route_b_sequences,i_seq_b,BOTH);
                                        if(Count::improves(0.0,delta) &&
                                            Count::improves(best_delta,delta)) {
                                            if(propagate_virtual_swap_2_2(i_route_a,i_seq_a-1,seq_b_1,seq_b_2) &&
                                                propagate_virtual_swap_2_2(i_route_b,i_seq_b-1,seq_a_1,seq_a_2)) {
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
        this->persist_swap_2_2(coordinates,best_delta);

    }

}

void Search::ls_inter_shift_1_0() {

    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    Route* route_a = nullptr;
    Route* route_b = nullptr;
    vector<Sequence> * route_a_sequences = nullptr;
    vector<Sequence> * route_b_sequences = nullptr;
    Sequence * seq_a = nullptr;
    Sequence * seq_b = nullptr;

    for (int i_route_a = 0;i_route_a<(int)this->solution->routes.size();i_route_a++) {
        route_a = this->solution->routes.at(i_route_a);
        route_a_sequences = &route_a->sequences;

        if((int)route_a_sequences->size() > 2) {
            for (int i_route_b = 0;i_route_b<(int)this->solution->routes.size();i_route_b++) {

                if(i_route_a!=i_route_b) {
                    route_b = this->solution->routes.at(i_route_b);
                    route_b_sequences = &route_b->sequences;

                    //Verificando se pelo menos um cliente da rota A cabe na rota B
                    if((route_b->load + route_a->minimun_route_load) <= this->instance->load_capacity) {
                        //Aponta pro nó de A que vai
                        for(int i_seq_a = 1; i_seq_a<((int)route_a_sequences->size()-1);i_seq_a++) {
                            seq_a = &route_a_sequences->at(i_seq_a);

                            //Verificando se o nó da seq A cabe na rota B
                            if(!this->swap_1_0_broke_load(seq_a,route_b)) {

                                //Aponta pro nó de B que vai vir antes
                                for(int i_seq_b = 0; i_seq_b<((int)route_b_sequences->size()-1);i_seq_b++) {
                                    seq_b = &route_b_sequences->at(i_seq_b); //TODO depois remover

                                    double delta = calculate_delta_shift_1_0(route_a_sequences,i_seq_a,route_b_sequences,i_seq_b,BOTH);
                                    if(Count::improves(0.0,delta) &&
                                        Count::improves(best_delta,delta)) {
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
                    if((int)route_b_sequences->size() == 2) {
                        break;
                    }
                }
            }
        }
    }

    if(best_delta<0.0) {
        this->persist_shift_1_0(coordinates,best_delta);
    }
}


void Search::ls_inter_shift_2_0() {

    double best_delta = 0.0;
    int coordinates[4] = {-1,-1,-1,-1}; //i_route_a,i_seq_a,i_route_b,i_seq_b
    Route* route_a = nullptr;
    Route* route_b = nullptr;
    vector<Sequence> * route_a_sequences = nullptr;
    vector<Sequence> * route_b_sequences = nullptr;
    Sequence * seq_a_1 = nullptr;
    Sequence * seq_a_2 = nullptr;
    Sequence * seq_b = nullptr;
    for (int i_route_a = 0;i_route_a<(int)this->solution->routes.size();i_route_a++) {
        route_a = this->solution->routes.at(i_route_a);
        route_a_sequences = &route_a->sequences;
        if((int)route_a_sequences->size() > 3) {
            for (int i_route_b = 0;i_route_b<(int)this->solution->routes.size();i_route_b++) {

                if(i_route_a!=i_route_b) {
                    route_b = this->solution->routes.at(i_route_b);
                    route_b_sequences = &route_b->sequences;

                    //Verificando se pelo menos um cliente da rota A cabe na rota B
                    if((route_b->load + route_a->minimun_route_load) <= this->instance->load_capacity) {
                        //Aponta pro nó de A que vai
                        for(int i_seq_a = 1; i_seq_a<((int)route_a_sequences->size()-2);i_seq_a++) {
                            seq_a_1 = &route_a_sequences->at(i_seq_a);
                            seq_a_2 = &route_a_sequences->at(i_seq_a+1);

                            //Verificando se os nós da seq A cabe na rota B
                            if(!this->swap_2_0_broke_load(seq_a_1,seq_a_2,route_b)) {

                                //Aponta pro nó de B que vai vir antes
                                for(int i_seq_b = 0; i_seq_b<((int)route_b_sequences->size()-1);i_seq_b++) {
                                    seq_b = &route_b_sequences->at(i_seq_b); //TODO depois remover

                                    double delta = calculate_delta_shift_2_0(route_a_sequences,i_seq_a,route_b_sequences,i_seq_b, BOTH);
                                    if(Count::improves(0.0,delta) &&
                                        Count::improves(best_delta,delta)) {

                                        if(propagate_virtual_shift_2_0(i_route_b,i_seq_b,seq_a_1,seq_a_2)) {
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
                    if((int)route_b_sequences->size() == 2) {
                        break;
                    }
                }
            }
        }
    }

    if(best_delta<0.0) {
        this->persist_shift_2_0(coordinates,best_delta);
    }
}
