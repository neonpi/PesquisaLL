//
// Created by Rafael on 08/03/2025.
//

#include "Search.h"
#include "Utils.h"

void Search::persist_swap_1_1(int *coordinates, double delta) {
        Route* route_a = this->solution->routes.at(coordinates[0]);
        vector<Sequence>* route_a_sequences = &route_a->sequences;
        Sequence* seq_a = &route_a_sequences->at(coordinates[1]);
        Route* route_b = this->solution->routes.at(coordinates[2]);
        vector<Sequence>* route_b_sequences = &route_b->sequences;
        Sequence* seq_b = &route_b_sequences->at(coordinates[3]);
        double delta_a= calculate_delta_swap_1_1(route_a_sequences,coordinates[1],route_b_sequences,coordinates[3],'a');
        double delta_b= calculate_delta_swap_1_1(route_a_sequences,coordinates[1],route_b_sequences,coordinates[3],'b');

        route_a->traveled_distance += delta_a;
        route_b->traveled_distance += delta_b;
        this->solution->cost += delta;


        //Atualizando a demanda mínima para a rota a, se for necessario
        double min_seq_b_demand = -1.0;
        for(Node* n: seq_b->customers) {
            if(n->load_demand < route_a->minimun_route_load) {
                route_a->minimun_route_load = n->load_demand;
            }
            if(min_seq_b_demand == -1.0 || n->load_demand < min_seq_b_demand) {
                min_seq_b_demand = n->load_demand;
            }
        }

        //Atualizando a demanda mínima para a rota b, se for necessario
        double min_seq_a_demand = -1.0;
        for(Node* n: seq_a->customers) {
            if(n->load_demand < route_b->minimun_route_load) {
                route_b->minimun_route_load = n->load_demand;
            }
            if(min_seq_a_demand == -1.0 || n->load_demand < min_seq_a_demand) {
                min_seq_a_demand = n->load_demand;
            }
        }


        //Reajustando a demanda mínima da rota a, se o cara que saiu detém a menor demanda
        if(route_a->minimun_route_load != min_seq_b_demand && route_a->minimun_route_load == min_seq_a_demand) {
            for(int i=1; i<(int)route_a_sequences->size()-1;i++ ) {
                Sequence* s = &route_a_sequences->at(i);
                if(s != seq_b) {
                    for(Node* n: s->customers) {
                        if(n->load_demand < route_a->minimun_route_load) {
                            route_a->minimun_route_load = n->load_demand;
                        }
                    }
                }
            }
        }
        //Reajustando a demanda mínima da rota b, se o cara que saiu detém a menor demanda
        if(route_b->minimun_route_load != min_seq_a_demand && route_b->minimun_route_load == min_seq_b_demand) {
            for(int i=1; i<(int)route_b_sequences->size()-1;i++ ) {
                Sequence* s = &route_b_sequences->at(i);
                if(s != seq_a) {
                    for(Node* n: s->customers) {
                        if(n->load_demand < route_b->minimun_route_load) {
                            route_b->minimun_route_load = n->load_demand;
                        }
                    }
                }
            }
        }

        //Atualizando contador de lockers por rota
        if(seq_a->node->type == "p") {
            route_a->visited_lockers[seq_a->node] --;
            route_b->visited_lockers[seq_a->node] ++;
        }
        if(seq_b->node->type == "p") {
            route_b->visited_lockers[seq_b->node] --;
            route_a->visited_lockers[seq_b->node] ++;
        }

        route_a_sequences->insert(route_a_sequences->begin()+coordinates[1]+1,1,*seq_b);
        route_b_sequences->insert(route_b_sequences->begin()+coordinates[3]+1,1,*seq_a);

        route_a_sequences->erase(route_a_sequences->begin()+coordinates[1],route_a_sequences->begin()+coordinates[1]+1);
        route_b_sequences->erase(route_b_sequences->begin()+coordinates[3],route_b_sequences->begin()+coordinates[3]+1);

        propagate(coordinates[2],coordinates[3]-1);
        propagate(coordinates[0],coordinates[1]-1);



}

