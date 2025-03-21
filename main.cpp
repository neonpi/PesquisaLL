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

void test_shortest_path(vector<Instance*> *instances);

int main(int argc, char *argv[])
{
    if(argc < 2) {

        cout<<"LOADING INSTANCES"<<endl;
        vector<Instance*> instances = Utils::buildInstances();
        cout<<"LOADING FINISHED"<<endl;


        Config* config = new Config(100,0.2,0.6,false);
        //Config* config = new Config(1000000,0.2,false);
        config->print();

        cout<<"RUNNING EXPERIMENTS"<<endl;

        //test_shortest_path(&instances);
        //run_pair_instance_seed(&instances,"C101_co_25.txt",0,config);
        //run_instance(&instances,"C104_co_25.txt",config);
        //run_instance(&instances,"R205_co_50.txt",config);
        default_run(&instances,config);
        //test_solution(&instances,"R205_co_50.txt", config);

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
        config->run = i;

        Search* search = new Search(instance,config);
        clock_t time = clock();
        search->run();
        time = clock() - time;


        stats->set_result(search->solution,((double) time / CLOCKS_PER_SEC));
        Utils::print_result_file(search, instance, i, (double) time / CLOCKS_PER_SEC, config->seeds.at(i));
        Utils::test_cost(search->solution);
        Utils::test_print_viability(search->solution,config->seeds.at(i));

        /*if(best_known == -1.0 || search->solution->cost < best_known) {
            best_known = search->solution->cost;
            cout<<"Improvement found "<<best_known<<" on run "<<i<<endl;
            if(best_known < 182) {
                break;
            }
        }*/

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
        {"D0", "C30", "C6", "C45", "P0()", "Dt"},
        {"D0", "C20", "C21", "P1()", "C3", "C23", "C25", "Dt"},
        {"D0", "C27", "C32", "C8", "C29", "C31", "C0", "C49", "Dt"},
        {"D0", "C4", "P2()", "C15", "C43", "C36", "C5", "Dt"}
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

void test_shortest_path(vector<Instance *> *instances) {
    int broken_inequalties = 0;
    double inf = 1000000.0;
    for(Instance* instance: *instances) {
        //cout<<"Instance "<< instance->inst_name<<endl;
        bool broke_inequalty = false;
        //Instanciando matriz de distancias
        vector<vector<double>> shortest_path;
        shortest_path.reserve(instance->n_node);

        for(int i=0; i<instance->n_node; i++) {
            shortest_path.emplace_back();
            vector<double>* current_vector = &*(shortest_path.end() -1);

            for(int j=0; j < instance->n_node; j++) {
                if(j==i) {
                    current_vector->push_back(0.0);
                }else {
                    current_vector->push_back(inf);
                }
            }
        }
        //Calculando distâncias mínimas
        for(int i_node_i=0; i_node_i<instance->n_node; i_node_i++) {
            vector<double>* current_vector = &shortest_path.at(i_node_i);
            Node* node_i = &instance->nodes.at(i_node_i);

            vector<bool> relaxed;
            for(int i=0;i<instance->n_node; i++){relaxed.push_back(false);}


            for(int j=0; j < instance->n_node; j++) {

                //Escolhendo o menor nó não relaxado
                int i_node_j = -1;
                for(int k=0; k<instance->n_node;k++) {
                    if(!relaxed.at(k) && (i_node_j == -1 || current_vector->at(k) < current_vector->at(i_node_j))) {
                        i_node_j = k;
                    }
                }

                Node* node_j = &instance->nodes.at(i_node_j);

                for(int i_node_k=0; i_node_k< instance->n_node; i_node_k++) {
                    Node* node_k = &instance->nodes.at(i_node_k);

                    if(current_vector->at(i_node_k) > (current_vector->at(i_node_j) + instance->distances[i_node_j][i_node_k])) {
                        broke_inequalty = true;
                        current_vector->at(i_node_k) = (current_vector->at(i_node_j) + instance->distances[i_node_j][i_node_k]);
                    }

                }
                relaxed.at(i_node_j) = true;
            }
        }

        for(int i=0; i < instance -> n_node; i++) {

            for(int j=0; j < instance -> n_node; j++) {

                    instance->distances[i][j] = shortest_path.at(i).at(j);
                /*if(shortest_path.at(i).at(j) < instance->distances[i][j] && Count::differs(shortest_path.at(i).at(j),instance->distances[i][j])) {
                    double* bo = &instance->distances[i][j];
                    broke_inequalty = true;
                }*/

            }

        }

        if(broke_inequalty) {
            broken_inequalties++;
        }
    }

    cout<<broken_inequalties<<" broken"<<endl;

}
