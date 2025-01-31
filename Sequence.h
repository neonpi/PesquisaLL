//
// Created by Rafael on 11/01/2025.
//

#ifndef SEQUENCE_H
#define SEQUENCE_H
#include "Node.h"


class Sequence {

public:
    Node* node = nullptr;
    Node* customer = nullptr;
    char method = 'u';
    double time_off = 0.0;
    double current_distance = 0.0;
    double current_time = 0.0;
    double current_load = 0.0;
    double minimun_route_load = 0.0;

    void clone_this_to(Sequence* clone) {
        clone->node = this->node;
        clone->customer = this->customer;
        clone->method = this->method;
        clone->time_off = this->time_off;
        clone->current_distance = this->current_distance;
        clone->current_time = this->current_time;
        clone->current_load = this->current_load;
        clone->minimun_route_load = this->minimun_route_load;
    }

    void reset_values() {
        this->method = 'u';
        this->time_off = 0.0;
        this->current_distance = 0.0;
        this->current_time = 0.0;
        this->current_load = 0.0;
        this->minimun_route_load = 0.0;
    }
};



#endif //SEQUENCE_H
