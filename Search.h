//
// Created by Rafael on 29/11/2024.
//

#ifndef SEARCH_H
#define SEARCH_H

#define VIABLE_TW 0
#define EARLY_TW 1
#define INVIABLE_TW 2

#include "Instance.h"
#include "Sequence.h"
#include <algorithm>
#include <tuple>
#include <vector>

#include "Config.h"
using namespace std;

class Search {
public:
    Search(Instance* instance, Config* config);
    ~Search();

    void initialize_routes();

    //Construtivo
    void construct();
    void insertion_heuristic();

    vector<tuple<int, int, Sequence, double>> build_candidate_list();
    void insert_sequency(tuple<int, int, Sequence, double> candidate);

    void try_customer_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node* cand_node);
    void try_locker_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node* cand_node);

    void propagate(int route_index, int previous_sequence_index);
    bool propagate_virtual(int route_index, int previous_sequence_index, Sequence *cand_sequence);

    void fill_forward_virtual(Sequence *previous_sequence, Sequence* current_sequence);
    void fill_forward(Sequence *previous_sequence, Sequence* current_sequence);

    bool broke_time_window();
    bool sort_function(const tuple<int, int, Sequence> cus_a, const tuple<int, int, Sequence> cus_b);


    void calculate_total_cost();
    void print_is_viable();

    bool is_customer(int node_index);
    bool is_locker(int node_index);

    bool is_load_viable(int route_index, Node* cand_node){return this->instance->load_capacity > (this->routes.at(route_index).end()-1)->current_load + cand_node->load_demand;}

    void calculate_delta_distance(tuple<int, int, Sequence, double> *cus);
    void print();
    void print_candidate_list(vector<tuple<int, int, Sequence, double>> *cand_list);

    Instance* instance;
    vector<vector<Sequence>> routes;
    Sequence* virtual_sequence;
    bool* visited;
    int n_vehicles;
    int customer_served;
    double total_cost;
    Search* best;

    Config* config;


};



#endif //SEARCH_H
