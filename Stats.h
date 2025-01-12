//
// Created by Rafael on 12/01/2025.
//

#ifndef STATS_H
#define STATS_H
#include "Config.h"
#include "Instance.h"


class Stats {
public:
    Stats(Instance* instance, Config* config);
    void set_result(double cost, double time);
    void finish_stats();

    Instance* instance;
    Config* config;
    vector<double> costs;
    vector<double> times;
    double best_cost = -1.0;
    double best_time = -1.0;
    double avg_cost = 0;
    double avg_time = 0;


};



#endif //STATS_H
