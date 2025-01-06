//
// Created by Rafael on 06/01/2025.
//

#include "Factory.h"

void Factory::defineNodeIndexes(Instance *instance) {

    instance->customer_indexes[0] = 2;

    for(int i=2; i<instance->n_node; i++) {

        if(instance->nodes.at(i).type == "p") {
            instance->customer_indexes[1]=i;
            instance->locker_indexes[0]=i;
            instance->locker_indexes[1]=instance->n_node;
            break;
        }

    }

    instance->qty_customers = instance->customer_indexes[1] - 2;
    instance->qty_parcel_locker = instance->locker_indexes[1] - instance->qty_customers - 2;
}