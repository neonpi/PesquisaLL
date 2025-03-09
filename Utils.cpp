//
// Created by Rafael on 24/11/2024.
//

#include "Utils.h"


vector<Instance *> Utils::buildInstances(string problem) {
    ifstream file;
    file.open("instances/instances_" + problem + ".txt");
    string line;
    vector<Instance*> instances;

    if(file.is_open()) {
        while(getline(file,line)) {
            Instance* new_instance = nullptr;
            if(problem == "vrppl") {
                new_instance = Vrppl::buildInstance(line);
            }else {
                new_instance = Evrptwprpl::buildInstance(line);
            }
            instances.push_back(new_instance);
        }
    }else {
        cout<<"File not opened"<<endl;
    }

    file.close();
    return instances;
}


void Utils::print_result_file(Search *search, Instance *instance, int run, double time, long seed) {
    ofstream file;

    if(search == nullptr) {
        file.open("Output/"+instance->inst_name+"_stats", ofstream::out);
        if (!file.is_open()) {
            cout<<"Output file not opened"<<endl;
        }
        file<<"run,cost,#_vehicles,time,seed"<<endl;
    }else{
        file.open("Output/"+search->instance->inst_name+"_stats", ofstream::app);
        if (!file.is_open()) {
            cout<<"Output file not opened"<<endl;
        }
        file<<"--- "<<to_string(run)<<","<<to_string(search->solution->cost)<<","<<to_string(search->solution->used_routes)<<","<<to_string(time)<<","<<to_string(seed)<<endl;

        for (Route* route: search->solution->routes) {
            vector<Sequence>* route_sequences = &route->sequences;
            if(route_sequences->size()>2) {
                string route_string = "";

                for (Sequence sequence: *route_sequences) {
                    route_string+=sequence.node->id;

                    if (sequence.node->id != "Dt") {
                        if(sequence.node->type == "p") {
                            route_string+="(";

                            for(int i_customer=0; i_customer < (int)sequence.customers.size();i_customer++) {
                                Node* customer = sequence.customers.at(i_customer);
                                route_string+= customer->id;
                                if(i_customer < (int)sequence.customers.size() -1) {
                                    route_string+=", ";
                                }
                            }

                            route_string+=")";
                        }
                        route_string+=" ";
                    }
                }
                file<<route_string<<endl;
            }

        }
        file<<endl;

    }


    file.close();

}


vector<string> Utils::tookenize(string str, string symbol) {

    stringstream ss(str);

    vector<string> tokens;
    string temp_str;

    while(getline(ss,temp_str,symbol[0])) {
        tokens.push_back(temp_str);
    }

    return tokens;

}


void Utils::print_final_stats(Stats *stats) {
    ofstream file;

    if(stats->instance->inst_name == "C101_co_25.txt") {
        file.open("Output/0_final",ios::out);
        file<<"instance,avg_time,avg_cost,#_vehicles,best_cost"<<endl;
        file.close();
    }

    file.open("Output/0_final",ios::app);

    file<<stats->instance->inst_name<<","<<to_string(stats->avg_time)<<","<<to_string(stats->avg_cost)<<","<<to_string(stats->best_solution->used_routes)<<","<<to_string(stats->best_solution->cost)<<endl;

    file.close();
}

void Utils::print_screen_run(Stats *stats) {
    cout<<"AVG_COST: "<<stats->avg_cost<<" - AVG_TIME: "<<stats->avg_time<<" - BEST_COST: "<<stats->best_solution->cost<<" - BEST_TIME: "<<stats->best_time<<endl;
}
//TODO testar
void Utils::test_print_viability(Solution *solution, long seed) {
    int inviabilities = 0;
    bool customer_viable = true;
    bool load_viable = true;
    bool load_sum_viable = true;
    bool time_window_viable = true;
    int used_vehicles = 0;

    vector<string> inviable_customers;
    vector<string> inviable_tws;
    vector<string> inviable_load_sum;

    //Verificando viabilidade de cliente atendido
    for(int i=solution->instance->customer_indexes[0];
        i<solution->instance->customer_indexes[1];
        i++) {

        if(!solution->served[i]) {
            customer_viable = false;
            Node* customer = &solution->instance->nodes.at(i);
            inviable_customers.push_back(customer->id);
            inviabilities++;
        }

    }

    //Verificando viabilidade de load e tw
    for(Route* r: solution->routes ) {

        //Verificando viabilidade de load
        if(r->load > solution->instance->load_capacity) {

            load_viable = false;
            inviabilities++;
        }

        //Verificando se o somatorio de loads está certo
        double load_sum = 0.0;
        for(const Sequence& s: r->sequences) {
            for(Node* n: s.customers) {
                load_sum += n->load_demand;
            }
        }

        if(load_sum != r->load) {
            load_sum_viable = false;
            inviabilities++;
        }

        //Verificando viabilidade de tw
        for(const Sequence& s: r->sequences) {
            if((s.current_time < s.node->time_window[0] && Count::differs(s.current_time,s.node->time_window[0]))
                ||
                (s.current_time > s.node->time_window[1] && Count::differs(s.current_time,s.node->time_window[1]))) {
                Node* node = s.node;
                time_window_viable = false;
                inviable_tws.push_back(node->id);
                inviabilities++;
            }
        }

        //Contando veículos usados
        if((int)r->sequences.size() > 2) {
            used_vehicles++;
        }
    }

    //Imprimindo resultados
    if(!customer_viable) {
        cout<<"Customer inviability ("<<(int)inviable_customers.size()<<"): (";
        for(const string& customer: inviable_customers) {
            cout<< customer<<", ";
        }
        cout<<endl;
    }

    if(!load_sum_viable) {
        cout<<"Load sum!"<<endl;
    }

    if(!load_viable) {
        cout<<"Load inviability ("<<endl;
    }

    if(!time_window_viable) {
        cout<<"Time window inviability ("<<(int)inviable_tws.size()<<"): (";
        for(string time: inviable_tws) {
            cout<< time;
            if(time != *(inviable_tws.end()-1)) {
                cout<<", ";
            }
        }
        cout<<")"<<endl;
        solution->print();
    }

    if(used_vehicles != solution->used_routes) {
        inviabilities++;
        cout<<"# Vehicle differs: "<<used_vehicles<<" x "<<solution->used_routes<<endl;
    }

    //Saindo se houver inviabilidade
    if(inviabilities>0) {
        cout<<to_string(inviabilities)<<" inviabilities"<<endl;
        cout<<"SEED "<<to_string(seed)<<endl;
        exit(10);
    }



}
//TODO testar
void Utils::test_cost(Solution *solution) {
    double cost = 0.0;

    for(Route* route: solution->routes) {
        for(int i=1;i<(int)route->sequences.size();i++) {
            Sequence prev_sequence = route->sequences.at(i-1);
            Sequence sequence = route->sequences.at(i);
            cost+= solution->instance->distances[prev_sequence.node->index][sequence.node->index];
        }
    }

    if( Count::differs(cost,solution->cost)) {
        cout<<"Erro de custo (E:"<<cost<<" G:"<<solution->cost<<")"<< endl;
        exit(8);
    }

}
//TODO testar
void Utils::print_candidate_list(vector<tuple<int, int, Sequence, double>> *cand_list) {
    for(tuple<int,int,Sequence, double> cand: *cand_list) {
        int cand_route = get<0>(cand);
        int cand_index = get<1>(cand);
        Sequence *cand_sequence = &get<2>(cand);

        if(cand_sequence->node!=cand_sequence->customers.at(0)) {
            cout<<cand_route<<" - "<<cand_index<<" - (";
            for(int i=0; i<(int)(cand_sequence->customers.size())-1;i++) {
                cout<<cand_sequence->customers.at(i)->id<<", ";
            }
            cout<<cand_sequence->customers.at((int)(cand_sequence->customers.size())-1)->id<<")"<<cand_sequence->node->id<<" - "<<get<3>(cand)<<endl;;
        }else {
            cout<<cand_route<<" - "<<cand_index<<" - "<<cand_sequence->node->id<<" - "<<get<3>(cand)<<endl;;
        }
    }
    cout<<endl;
}


