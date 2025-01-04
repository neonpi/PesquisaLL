//
// Created by Rafael on 29/11/2024.
//

#ifndef SEARCH_H
#define SEARCH_H

#define VIABLE_TW 0
#define EARLY_TW 1
#define INVIABLE_TW 2

#include "Instance.h"
#include <algorithm>
#include <iosfwd>
#include <tuple>
#include <vector>
using namespace std;

class Sequence {
public:
    Node* node = nullptr;
    Node* customer = nullptr;
    char method = 'u';
    double time_off = 0.0;
    double max_time_off = 0.0;
    double current_distance = 0.0;
    double current_time = 0.0;
    double current_load = 0.0;

    void clone(Sequence* clone) {
        clone->node = this->node;
        clone->customer = this->customer;
        clone->method = this->method;
        clone->time_off = this->time_off;
        clone->max_time_off = this->max_time_off;
        clone->current_distance = this->current_distance;
        clone->current_time = this->current_time;
        clone->current_load = this->current_load;
    }

};

class Search {
public:
    Search(Instance* instance, int n_vehicles);
    ~Search();

    void initialize_routes();

    //Métodos Igor
    void construct();
    void insertion_heuristic();

    vector<tuple<int, int, Sequence>> build_candidate_list();
    void fill_candidate_sequences(Sequence *previous_sequence, Sequence *next_sequence, Sequence* cand_sequence, Node *cand_node, Node *customer_node);
    void try_customer_candidate(vector<tuple<int, int, Sequence>> *cand_list, Node* cand_node);
    void try_locker_candidate(vector<tuple<int, int, Sequence>> *cand_list, Node* cand_node);

    void fill_time_dist_forward_virtual(Sequence* next_sequence, bool is_candidate);
    void fill_max_toff_reverse_virtual(Sequence* previous_sequence, bool is_candidate_sequence);
    void fill_toff_forward_virtual(Sequence* next_sequence, double *delta_time, bool is_candidate, bool *inviable, double next_sequence_timeoff);

    void fill_time_dist_forward(Sequence *previous_sequence, Sequence* current_sequence);
    void fill_max_toff_referse(Sequence* current_sequence, Sequence *next_sequence);

    void propagate(int route_index, int previous_sequence_index, Sequence *cand_sequence);
    bool propagate_virtual(int route_index, int previous_sequence_index, Sequence *cand_sequence);
    bool broke_upper_time_window();
    bool broke_timeoff_upper();
    void insert_sequency(tuple<int, int, Sequence> candidate);
    bool sort_function(const tuple<int, int, Sequence> cus_a, const tuple<int, int, Sequence> cus_b);
    bool sort_function_tw(tuple<int,int,vector<Sequence>> cus_a,tuple<int,int,vector<Sequence>> cus_b);
    double delta_distance(tuple<int,int,vector<Sequence>> cus);

    void calculate_total_cost();
    void print_is_viable();

    bool is_customer(int node_index);
    bool is_locker(int node_index);

    bool is_load_viable(int route_index, Node* cand_node){return this->instance->load_capacity > (this->routes.at(route_index).end()-1)->current_load + cand_node->load_demand;}
    short is_time_window_viable(Sequence* candidate_sequence);

    void print();
    string get_delta_to_print(tuple<int,int,vector<Sequence>> cus);

    Instance* instance;
    vector<vector<Sequence>> routes;
    Sequence* virtual_sequence;
    bool* visited;
    int n_vehicles;
    int customer_served;
    double total_cost;
    Search* best;


};



#endif //SEARCH_H
