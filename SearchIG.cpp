//
// Created by Rafael on 10/03/2025.
//
#include "Search.h"
#include "Utils.h"


void Search::iterated_greedy() {

    backup_solution();


    int dec_size = 1;
    int dec_size_limit = this->instance->customers_qty*0.4;

    int iter_random_limit = 10;
    int iter_total_limit = this->solution->used_routes + iter_random_limit;


    while(dec_size<dec_size_limit) {

        for(int iter=0; iter<iter_total_limit;iter++) {

            this->solution->print();
            while(this->solution->used_routes > 0) {
                deconstruct_random(1);
                this->solution->print();
                Utils::test_cost(this->solution);
                Utils::test_print_viability(this->solution,0);
            }

            if(iter<iter_random_limit) {
                deconstruct_random(dec_size);
            }else {
                //deconstruct_route(iter-iter_random_limit);
            }

            //insertion_heuristic_ig();

            this->rvnd_inter();

            if(this->solution->cost < this->best_solution->cost) {
                backup_solution();
                iter_total_limit = this->solution->used_routes + iter_random_limit;
                dec_size = 0;
                break;
            }else {
                restore_solution();
            }

        }
        dec_size++;
    }
}


void Search::deconstruct_random(int dec_size) {

    for(int i=0;i<dec_size;i++) {

        int i_route = rand()%this->solution->used_routes;
        Route* route = this->solution->routes.at(i_route);
        vector<Sequence>* route_sequences = &route->sequences;

        int i_seq = 1 + rand()%((int)route_sequences->size()-2);
        Sequence* seq = &route_sequences->at(i_seq);

        int i_customer = rand()%((int)seq->customers.size());
        Node* customer = seq->customers.at(i_customer);

        this->solution->served.at(customer->index) = false;

        if((int)seq->customers.size() == 1) {

            double delta = calculate_delta_destruction(route_sequences,i_seq);
            this->solution->cost += delta;

            if(seq->node->type == "p") {
                route->visited_lockers[seq->node] --;
            }

            route_sequences->erase(route_sequences->begin()+i_seq,route_sequences->begin()+i_seq+1);

            if(route_sequences->size() == 2) {
                route->traveled_distance = 0.0;
                route->load = 0.0;
                route->minimun_route_load = 0.0;

                iter_swap(this->solution->routes.begin()+i_route,this->solution->routes.begin()+this->solution->used_routes-1);

                this->solution->used_routes--;
            }else {
                route->load -= customer->load_demand;
            }


        }else {
            seq->customers.erase(seq->customers.begin() + i_customer , seq->customers.begin() + i_customer + 1);
            route->load -= customer->load_demand;
        }
    }

    for(int i=0;i<this->solution->used_routes;i++) {
        propagate(i,0);
    }

}