//
// Created by Rafael on 27/02/2025.
//
#include "Search.h"

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

double Search::calculate_delta_or_opt_1(vector<Sequence> *route, int i_seq_a, int i_seq_b) {
    Sequence* seq_a_previous = &route->at(i_seq_a-1);
    Sequence* seq_a = &route->at(i_seq_a);
    Sequence* seq_a_next = &route->at(i_seq_a+1);

    Sequence* seq_b_previous = &route->at(i_seq_b-1);
    Sequence* seq_b = &route->at(i_seq_b);

    double delta = 0.0;

    delta+=this->instance->distances[seq_a_previous->node->index][seq_a_next->node->index];
    delta-=this->instance->distances[seq_a_previous->node->index][seq_a->node->index];
    delta-=this->instance->distances[seq_a->node->index][seq_a_next->node->index];

    delta+=this->instance->distances[seq_b_previous->node->index][seq_a->node->index];
    delta+=this->instance->distances[seq_a->node->index][seq_b->node->index];
    delta-=this->instance->distances[seq_b_previous->node->index][seq_b->node->index];


    return delta;
}

double Search::calculate_delta_or_opt_k(int k, vector<Sequence> *route, int i_seq_a, int i_seq_b) {
    Sequence* seq_a_previous = &route->at(i_seq_a-1);
    Sequence* seq_a_begin = &route->at(i_seq_a);
    Sequence* seq_a_end = &route->at(i_seq_a+k-1);
    Sequence* seq_a_next = &route->at(i_seq_a+k);

    Sequence* seq_b_previous = &route->at(i_seq_b-1);
    Sequence* seq_b = &route->at(i_seq_b);

    double delta = 0.0;

    delta+=this->instance->distances[seq_a_previous->node->index][seq_a_next->node->index];
    delta-=this->instance->distances[seq_a_previous->node->index][seq_a_begin->node->index];
    delta-=this->instance->distances[seq_a_end->node->index][seq_a_next->node->index];

    delta+=this->instance->distances[seq_b_previous->node->index][seq_a_begin->node->index];
    delta+=this->instance->distances[seq_a_end->node->index][seq_b->node->index];
    delta-=this->instance->distances[seq_b_previous->node->index][seq_b->node->index];


    return delta;
}

double Search::calculate_delta_exchange(vector<Sequence> *route, int i_seq_a, int i_seq_b) {

    double delta = 0.0;

    if (i_seq_b == i_seq_a+1) {
        Sequence* seq_a_previous = &route->at(i_seq_a-1);
        Sequence* seq_a = &route->at(i_seq_a);

        Sequence* seq_b = &route->at(i_seq_b);
        Sequence* seq_b_next = &route->at(i_seq_b+1);

        delta+= this->instance->distances[seq_a_previous->node->index][seq_b->node->index];
        delta-= this->instance->distances[seq_a_previous->node->index][seq_a->node->index];

        delta+= this->instance->distances[seq_a->node->index][seq_b_next->node->index];
        delta-= this->instance->distances[seq_b->node->index][seq_b_next->node->index];

    }else {

        Sequence* seq_a_previous = &route->at(i_seq_a-1);
        Sequence* seq_a = &route->at(i_seq_a);
        Sequence* seq_a_next = &route->at(i_seq_a+1);

        Sequence* seq_b_previous = &route->at(i_seq_b-1);
        Sequence* seq_b = &route->at(i_seq_b);
        Sequence* seq_b_next = &route->at(i_seq_b+1);


        delta+= this->instance->distances[seq_a_previous->node->index][seq_b->node->index];
        delta+= this->instance->distances[seq_b->node->index][seq_a_next->node->index];
        delta-= this->instance->distances[seq_a_previous->node->index][seq_a->node->index];
        delta-= this->instance->distances[seq_a->node->index][seq_a_next->node->index];

        delta+= this->instance->distances[seq_b_previous->node->index][seq_a->node->index];
        delta+= this->instance->distances[seq_a->node->index][seq_b_next->node->index];
        delta-= this->instance->distances[seq_b_previous->node->index][seq_b->node->index];
        delta-= this->instance->distances[seq_b->node->index][seq_b_next->node->index];

    }

    return delta;

}
//TODO testar
double Search::calculate_delta_swap_1_1(vector<Sequence> *route_a, int i_seq_a, vector<Sequence> *route_b,
                                        int i_seq_b, char for_route) {
    Sequence* seq_a_previous = &route_a->at(i_seq_a-1);
    Sequence* seq_a = &route_a->at(i_seq_a);
    Sequence* seq_a_next = &route_a->at(i_seq_a+1);

    Sequence* seq_b_previous = &route_b->at(i_seq_b-1);
    Sequence* seq_b = &route_b->at(i_seq_b);
    Sequence* seq_b_next = &route_b->at(i_seq_b+1);

    double delta = 0.0;

    if(for_route == 'a') {

        //Contabilizando delta da rota_a
        delta+=this->instance->distances[seq_a_previous->node->index][seq_b->node->index];
        delta+=this->instance->distances[seq_b->node->index][seq_a_next->node->index];
        delta-=this->instance->distances[seq_a_previous->node->index][seq_a->node->index];
        delta-=this->instance->distances[seq_a->node->index][seq_a_next->node->index];

    }else if(for_route == 'b') {

        //Contabilizando delta da rota_b
        delta+=this->instance->distances[seq_b_previous->node->index][seq_a->node->index];
        delta+=this->instance->distances[seq_a->node->index][seq_b_next->node->index];
        delta-=this->instance->distances[seq_b_previous->node->index][seq_b->node->index];
        delta-=this->instance->distances[seq_b->node->index][seq_b_next->node->index];

    }else{ //BOTH

        //Contabilizando delta da rota_a
        delta+=this->instance->distances[seq_a_previous->node->index][seq_b->node->index];
        delta+=this->instance->distances[seq_b->node->index][seq_a_next->node->index];
        delta-=this->instance->distances[seq_a_previous->node->index][seq_a->node->index];
        delta-=this->instance->distances[seq_a->node->index][seq_a_next->node->index];

        //Contabilizando delta da rota_b
        delta+=this->instance->distances[seq_b_previous->node->index][seq_a->node->index];
        delta+=this->instance->distances[seq_a->node->index][seq_b_next->node->index];
        delta-=this->instance->distances[seq_b_previous->node->index][seq_b->node->index];
        delta-=this->instance->distances[seq_b->node->index][seq_b_next->node->index];

    }

    return delta;

}

