#include <iostream>
#include <string>

#include "vector"
#include "Config.h"
#include "Instance.h"
#include "Search.h"
#include "Stats.h"
#include "Utils.h"

using namespace std;

void default_run(vector<Instance*> *instances, Config* config);
void run_pair_instance_seed(vector<Instance*> *instances, string instance_name, long seed, Config* config);
void run_instance(vector<Instance*> *instances, string instance_name, Config* config);
void grasp_run(Instance *instance, Config* config, Stats* stats);
void test_solution(vector<Instance *> *instances, string instance_name, Config *config);
void irace_run(int argc, char *argv[]);
void fix_instances(vector<Instance*> *instances);

int main(int argc, char *argv[])
{
    if(argc < 2) {

        cout<<"LOADING INSTANCES"<<endl;
        vector<Instance*> instances = Utils::buildInstances();
        cout<<"LOADING FINISHED"<<endl;

        Config* config = new Config(30,0.2,1.0,false);
        //Config* config = new Config(1000000,0.2,false);
        config->print();

        cout<<"RUNNING EXPERIMENTS"<<endl;

        /*run_instance(&instances,"fixed_C104_co_25.txt",config);*/
        default_run(&instances,config);
        //test_solution(&instances,"fixed_C108_co_25.txt", config);

        cout<<"EXPERIMENTS FINISHED"<<endl;
        delete config;
    }else {
        irace_run(argc, argv);
    }
    return 0;
}
void default_run(vector<Instance*> *instances, Config* config) {

    for(Instance * instance: *instances) {
        cout<<"Instance "<< instance->name<<endl;
        Stats* stats = new Stats(instance, config);
        Utils::print_result_file(nullptr, instance, 0, 0.0, 0.0);

        for(int i=0;i<config->runs;i++) {

            srand(config->seeds.at(i));
            config->run = i;

            Search* search = new Search(instance,config);
            clock_t time = clock();
            search->run();
            time = clock() - time;

            stats->set_result(search->solution,((double) time / CLOCKS_PER_SEC));
            Utils::print_result_file(search, instance, i, (double) time / CLOCKS_PER_SEC, config->seeds.at(i));
            Utils::test_cost(search->solution);
            Utils::test_print_viability(search->solution,config->seeds.at(i));
            delete search;



        }
        stats->finish_stats();

        Utils::print_screen_run(stats);
        Utils::print_final_stats(stats);
        delete stats;
    }
}


void run_instance(vector<Instance *> *instances, string instance_name, Config *config) {
    Instance* instance = nullptr;
    for(Instance* i: *instances) {
        if (i->name == instance_name) {
            instance = i;
            break;
        }
    }

    if(instance == nullptr) {cout<<"Instance not found"<<endl; return;}

    cout<<"Instance "<< instance->name<<endl;
    Stats* stats = new Stats(instance, config);
    Utils::print_result_file(nullptr, instance, 0, 0.0, 0.0);
    double best_known = -1.0;
    for(int i=0;i<config->runs;i++) {

        srand(config->seeds.at(i));
        srand(0);
        config->run = i;

        Search* search = new Search(instance,config);
        clock_t time = clock();
        search->run();
        time = clock() - time;


        stats->set_result(search->solution,((double) time / CLOCKS_PER_SEC));
        Utils::print_result_file(search, instance, i, (double) time / CLOCKS_PER_SEC, config->seeds.at(i));
        Utils::test_cost(search->solution);
        Utils::test_print_viability(search->solution,config->seeds.at(i));

        delete search;

    }

    stats->finish_stats();

    Utils::print_screen_run(stats);
    Utils::print_final_stats(stats);
    delete stats;


}

void run_pair_instance_seed(vector<Instance *> *instances, string instance_name, long seed, Config *config) {
    Instance* instance = nullptr;
    for(Instance* i: *instances) {
        if (i->name == instance_name) {
            instance = i;
            break;
        }
    }

    if(instance == nullptr) {cout<<"Instance not found"<<endl; return;}


    cout<<"Instance "<< instance->name<<endl;
    Stats* stats = new Stats(instance, config);
    Utils::print_result_file(nullptr, instance, 0, 0.0, 0.0);

    srand(seed);

    Search* search = new Search(instance,config);
    clock_t time = clock();
    search->run();
    time = clock() - time;

    stats->set_result(search->solution,((double) time / CLOCKS_PER_SEC));
    Utils::print_result_file(search, instance, 0, (double) time / CLOCKS_PER_SEC, seed);
    Utils::test_cost(search->solution);
    Utils::test_print_viability(search->solution,seed);
    delete search;
}

void grasp_run(Instance *instance, Config* config, Stats* stats) {
    for(int i=0;i<config->runs;i++) {

        srand(config->seeds.at(i));
        Search* bestSearch = nullptr;
        clock_t bestSearchTime = 0;

        for(int j=0;j<50;j++) {
            Search* search = new Search(instance,config);
            clock_t time = clock();
            search->run();
            time = clock() - time;

            if(bestSearch == nullptr || search->solution->cost < bestSearch->solution->cost) {
                delete bestSearch;
                bestSearch = search;
                bestSearchTime = time;
                Utils::test_print_viability(search->solution,config->seeds.at(i));
            }else {
                Utils::test_print_viability(search->solution,config->seeds.at(i));
                delete search;
            }

        }

        stats->set_result(bestSearch->solution,((double) bestSearchTime / CLOCKS_PER_SEC));
        Utils::print_result_file(bestSearch, instance, i, (double) bestSearchTime / CLOCKS_PER_SEC, config->seeds.at(i));
        delete bestSearch;



    }
}


void irace_run(int argc, char *argv[]) {
    string instance_name = argv[2];
    long seed = stol(argv[4]);
    double alpha_constr = stod(argv[6]);
    double alpha_ig = stod(argv[8]);

    Config* config = new Config(1, alpha_constr, alpha_ig,false);
    Instance* instance = Utils::buildInstance(instance_name);

    Search* search = new Search(instance,config);

    search->construct();
    cout<<search->solution->cost<<endl;

    delete config;
    delete instance;
    delete search;
}

void test_solution(vector<Instance *> *instances, string instance_name, Config *config) {

    Instance* instance = nullptr;
    for(Instance* inst: *instances) {
        if(inst->name == instance_name) {
            instance = inst;
            break;
        }
    }

    vector<vector<string>> routes = {
        {"D0", "P0(C0,C1,C2,C4,C7,C8,C10,C22,C23,C24,C6)", "Dt"},
        {"D0", "C16", "C17", "C14", "C11", "P1(C15,C13,C12,C18)", "C20", "Dt"},
        {"D0", "C9", "C5", "C3", "P0(C19,C21)", "Dt"}
    };

    Search *s = new Search(instance,config);

    s->build_predefined_solution(routes);

    s->solution->print();
    cout<<"\n-----------TESTING SOLUTION:--------"<<endl<<endl;
    Utils::test_cost(s->solution);
    Utils::test_print_viability(s->solution,0);
    cout<<endl;

    delete s;


}

void fix_instances(vector<Instance*> *instances) {
    for(Instance* i: *instances) {
        bool happens = false;
        if(i->customers_qty == 25) {

            for(int i_cus = i->customer_indexes[0]; i_cus < i->customer_indexes[1]; i_cus++) {
                Node* n = &i->nodes.at(i_cus);
                if(n->type == "c3" || n->type == "c2") {
                    Node* node_locker = n->designated_locker;
                    for(int i_locker=i->locker_indexes[0]; i_locker<i->locker_indexes[1]; i_locker++) {
                        Node* locker_i = &i->nodes.at(i_locker);

                        if(node_locker!= nullptr && locker_i != node_locker &&  i->distances[n->index][locker_i->index] < i->distances[n->index][node_locker->index]) {
                            cout<<"O cliente "<<n->id<<" esta mais proximo do locker "<<locker_i->id<<" que do seu locker "<<node_locker->id<<", instancia "<<i->name<<endl;
                            happens = true;
                            Utils::fix_instance(i);
                            //break;
                        }
                        if(node_locker!= nullptr && locker_i != node_locker &&  i->distances[n->index][locker_i->index] == i->distances[n->index][node_locker->index]) {
                            cout<<"O cliente "<<n->id<<" iguaaaaaal "<<locker_i->id<<" que do seu locker "<<node_locker->id<<", instancia "<<i->name<<endl;
                            happens = true;
                            //Utils::fix_instance(i);
                            //break;
                        }
                    }
                }
                //if(happens){break;}

            }
        }
    }
}