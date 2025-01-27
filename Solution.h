//
// Created by Rafael on 27/01/2025.
//

#ifndef SOLUTION_H
#define SOLUTION_H
#include "Instance.h"
#include "Sequence.h"


class Solution {
public:
    Solution(Instance* instance);
    ~Solution();

    void initialize_routes();
    void calculate_total_cost();
    void print();
    void print_is_viable(long seed);
    Solution* clone();

    Instance* instance;
    vector<vector<Sequence>> routes;
    vector<bool> visited;
    double total_cost;
};



#endif //SOLUTION_H
