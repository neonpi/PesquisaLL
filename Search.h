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
#include "Solution.h"
using namespace std;

class Search {
public:
    Search(Instance* instance, Config* config);
    ~Search();

    void run();
    void debug_run();
    //Construtivo
    void construct();
    void insertion_heuristic();
    void insertion_heuristic_ig();

    //Busca local
    void rvnd_intra();
    void ls_intra_exchange();
    void ls_intra_2opt();
    void ls_intra_or_opt_1();
    void ls_intra_or_opt_k(int k);


    void rvnd_inter();
    void ls_inter_shift_1_0();
    void ls_inter_shift_2_0();
    void ls_inter_swap_1_1();
    void ls_inter_swap_2_1();
    void ls_inter_swap_2_2();

    //IG
    void iterated_greedy();
    void deconstruct_random(int dec_size);
    void deconstruct_route(int i_route);


    bool swap_1_1_broke_load(vector<Sequence>* route_a, Sequence* seq_a , vector<Sequence>* route_b, Sequence* seq_b);
    bool swap_2_1_broke_load(vector<Sequence>* route_a, Sequence* seq_a_1, Sequence *seq_a_2, vector<Sequence>* route_b, Sequence* seq_b);
    bool swap_2_2_broke_load(vector<Sequence>* route_a, Sequence* seq_a_1, Sequence *seq_a_2, vector<Sequence>* route_b, Sequence* seq_b_1, Sequence* seq_b_2);
    double calculate_delta_2opt(vector<Sequence>* route, int i_seq_a, int i_seq_b);
    double calculate_delta_or_opt_1(vector<Sequence>* route, int i_seq_a, int i_seq_b);
    double calculate_delta_or_opt_k(int k,vector<Sequence>* route, int i_seq_a, int i_seq_b);
    double calculate_delta_exchange(vector<Sequence>* route, int i_seq_a, int i_seq_b);
    double calculate_delta_shift_1_0(vector<Sequence>* route_a,int i_seq_a,vector<Sequence>* route_b,int i_seq_b);
    double calculate_delta_shift_2_0(vector<Sequence>* route_a,int i_seq_a,vector<Sequence>* route_b,int i_seq_b);
    double calculate_delta_swap_1_1(vector<Sequence>* route_a,int i_seq_a,vector<Sequence>* route_b,int i_seq_b);
    double calculate_delta_swap_2_1(vector<Sequence>* route_a,int i_seq_a,vector<Sequence>* route_b,int i_seq_b);
    double calculate_delta_swap_2_2(vector<Sequence>* route_a,int i_seq_a,vector<Sequence>* route_b,int i_seq_b);

    void local_search();
    void swap_sequence(int route_a_index, int seq_a_index, int route_b_index, int seq_b_index);
    void swap_sequence_intraroute(int route_index, int seq_a_index, int seq_b_index);
    bool is_viable();

    vector<tuple<int, int, Sequence, double>> build_candidate_list();
    vector<vector<tuple<int, int, Sequence, double>>> build_candidate_list_ig();
    void insert_sequency(tuple<int, int, Sequence, double> candidate);

    void try_customer_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node* cand_node);
    void try_locker_candidate(vector<tuple<int, int, Sequence, double>> *cand_list, Node* cand_node);

    void propagate(int route_index, int previous_sequence_index);
    bool propagate_virtual(int route_index, int previous_sequence_index, Sequence *cand_sequence);
    bool propagate_virtual_exchange(int route_index, int i_seq_a, int i_seq_b);
    bool propagate_virtual_segment(int route_index, int previous_sequence_index, Sequence *cand_sequence_1, Sequence *cand_sequence_2);
    bool propagate_virtual_2opt(int route_index, int i_seq_a, int i_seq_b);
    bool propagate_virtual_or_opt_1_up(int route_index, int i_seq_a, int i_seq_b);
    bool propagate_virtual_or_opt_k_up(int k,int route_index, int i_seq_a, int i_seq_b);
    bool propagate_virtual_or_opt_1_down(int route_index, int i_seq_a, int i_seq_b);
    bool propagate_virtual_or_opt_k_down(int k,int route_index, int i_seq_a, int i_seq_b);
    bool propagate_virtual_swap_1_1(int route_index, int previous_sequence_index, Sequence *cand_sequence);
    bool propagate_virtual_swap_1_2(int route_index, int previous_sequence_index, Sequence *cand_sequence);
    bool propagate_virtual_swap_2_1(int route_index, int previous_sequence_index, Sequence *cand_sequence_1,Sequence *cand_sequence_2);
    bool propagate_virtual_swap_2_2(int route_index, int previous_sequence_index, Sequence *cand_sequence_1,Sequence *cand_sequence_2);

    void fill_forward_virtual(Sequence *previous_sequence, Sequence* current_sequence);
    void fill_forward(Sequence *previous_sequence, Sequence* current_sequence);

    bool broke_time_window();
    vector<vector<tuple<int, int, Sequence, double>>> group_by_delta(vector<tuple<int, int, Sequence, double>> *cand_list);


    bool is_customer(int node_index);
    bool is_locker(int node_index);

    bool is_load_viable(int route_index, Node* cand_node){return this->instance->load_capacity > (this->solution->routes.at(route_index).end()-1)->current_load + cand_node->load_demand;}

    double calculate_delta_distance(int route_index, int previous_sequence_index, Sequence *cand_sequence);
    void print_candidate_list(vector<tuple<int, int, Sequence, double>> *cand_list);
    void print_ig_candidate_list(vector<vector<tuple<int, int, Sequence, double>>> *cand_list);

    void shift(vector<Sequence> *route, int i_seq_a, int i_seq_b);
    void shift_k(int k,vector<Sequence> *route, int i_seq_a, int i_seq_b);
    void test_cost();
    void build_predefined_solution(vector<vector<string>> solution);
    Instance* instance;
    Solution* solution;
    Sequence* virtual_sequence;
    int customer_served;
    Search* best;

    Config* config;


};

#endif //SEARCH_H
