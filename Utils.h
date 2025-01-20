//
// Created by Rafael on 24/11/2024.
//

#ifndef UTILS_H
#define UTILS_H
#include "Instance.h"
#include <fstream>
#include <vector>
#include "Search.h"
#include "Stats.h"


class Utils {
public:
    static vector<Instance*> buildInstances(string problem);
    static void print_route_file(Search* search, bool reset, long seed);
    static void print_stats_file(Stats* stats);
    static vector<string> tookenize(string str, string symbol);

    static bool differs(double a, double b, double epsilon = 0.001);
    static bool improves(double best, double current, double epsilon = 0.001);
    static void print_final_stats(Stats* stats);

};



#endif //UTILS_H
