//
// Created by Rafael on 29/11/2024.
//

#ifndef SEARCH_H
#define SEARCH_H

#define VIABLE_TW 0
#define EARLY_TW 1
#define INVIABLE_TW 2

#include "Instance.h"
#include <vector>
#include <list>
#include <algorithm>
#include <iosfwd>
#include <iosfwd>
#include <iosfwd>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include <vector>
#include <vector>
#include <vector>
#include <vector>
using namespace std;

class Sequence {
public:
    Node* node = nullptr;
    Node* customer = nullptr;
    char method = 'u';
    double time_off = 0.0;
    double min_time_off = 0.0;
    double max_time_off = 0.0;
    double current_time = 0.0;
    double current_distance = 0.0;
    double current_load = 0.0;

};

class Search {
public:
    Search(Instance* instance, int n_vehicles);
    ~Search();

    void initialize_routes();
    void construct();

    //Métodos Igor
    void construct_2(double alpha, int max_it_greedy);
    void construct_3();
    void update_cost_history(int iter, double* cost_history, double alpha);
    void insertion_heuristic();

    vector<tuple<int, int, Sequence>> build_candidate_list();
    void fill_candidate_sequences(Sequence *previous_sequence, Sequence *next_sequence, Sequence* cand_sequence, Node *cand_node, Node *customer_node);
    void accumulate_virtual_sequence(Sequence *previous_sequence, Sequence *virtual_sequence, Sequence *current_sequence, Sequence *next_sequence);
    void try_customer_candidate(vector<tuple<int, int, Sequence>> *cand_list, Node* cand_node);
    void try_locker_candidate(vector<tuple<int, int, Sequence>> *cand_list, Node* cand_node);
    void insert_sequency(tuple<int, int, Sequence> candidate);
    void update_forward(tuple<int, int, Sequence> candidate);
    bool sort_function(const tuple<int, int, Sequence> cus_a, const tuple<int, int, Sequence> cus_b);
    bool sort_function_tw(tuple<int,int,vector<Sequence>> cus_a,tuple<int,int,vector<Sequence>> cus_b);
    double delta_distance(tuple<int,int,vector<Sequence>> cus);

    void calculate_total_cost();
    void print_is_viable();

    bool is_customer(int node_index);
    bool is_recharger_station(int node_index);
    bool is_locker(int node_index);

    bool is_load_viable(Sequence* sequence, Node* cand_node){return sequence->current_load - cand_node->load_demand >= 0;}
    short is_time_window_viable(Sequence* candidate_sequence);
    bool is_forward_viable(int route_index, int previous_sequence_index, Sequence *candidate_sequence);

    void print();
    string get_delta_to_print(tuple<int,int,vector<Sequence>> cus);

    Instance* instance;
    vector<vector<Sequence>> routes;
    bool* visited;
    int n_vehicles;
    int customer_served;
    double total_cost;
    Search* best;


};



#endif //SEARCH_H
