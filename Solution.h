//
// Created by Rafael on 27/01/2025.
//

#ifndef SOLUTION_H
#define SOLUTION_H
#include "Instance.h"
#include "Sequence.h"
//#include "Utils.h"


class Solution {
public:
    Solution(Instance* instance);
    ~Solution();

    void initialize_routes();
    void calculate_total_cost();
    void print_detailed_total_cost();
    void print();
    void print_is_viable(long seed);
    //void sort_routes();
    Solution* clone();

    Instance* instance;
    vector<vector<Sequence>> routes;
    vector<bool> visited;
    double cost;
    int used_routes;
};



#endif //SOLUTION_H
