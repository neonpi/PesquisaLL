//
// Created by Rafael on 12/01/2025.
//

#include "Stats.h"

Stats::Stats(Instance* instance, Config* config) {
    this->instance = instance;
    this->config = config;
}

void Stats::set_result(double cost, double time) {
    this->costs.push_back(cost);
    this->times.push_back(time);

    if(this->best_cost == -1.0 || cost < this->best_cost) {
        this->best_cost = cost;
    }

    if(this->best_time == -1.0 || cost < this->best_time) {
        this->best_time = cost;
    }

    this->avg_cost+=cost;
    this->avg_time+=time;
}

void Stats::finish_stats() {
    this->avg_cost/=(double)this->config->runs;
    this->avg_time/=(double)this->config->runs;
}
