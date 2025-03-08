//
// Created by Rafael on 08/03/2025.
//
#include "Search.h"


bool Search::swap_1_1_broke_load(Route *route_a, Sequence* seq_a, Route *route_b, Sequence* seq_b) {

    double seq_a_demand = 0.0;
    double seq_b_demand = 0.0;

    for(Node* n: seq_a->customers) {
        seq_a_demand += n->load_demand;
    }

    for(Node* n: seq_b->customers) {
        seq_b_demand += n->load_demand;
    }

    bool broke_a = (route_a->load  - seq_a_demand + seq_b_demand) > this->instance->load_capacity;
    bool broke_b = (route_b->load  - seq_b_demand + seq_a_demand) > this->instance->load_capacity;
    return broke_a || broke_b;

}