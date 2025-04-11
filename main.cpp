#include <iostream>
#include <random>
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
void test_solutions(vector<Instance *> *instances, string instance_name, Config *config);


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

        //run_instance(&instances,"C101_co_50.txt",config);
        default_run(&instances,config);
        //test_solutions(&instances,"C104_co_25.txt", config);
        //test_solution(&instances,"C104_co_25.txt", config);

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

    for(int i=0;i<config->runs;i++) {
        srand(config->seeds.at(i));
        //srand(3);
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
bool criter(vector<string> a, vector<string> b){return (int)a.size() > (int)b.size();}
void test_solutions(vector<Instance *> *instances, string instance_name, Config *config) {
    Instance* instance = nullptr;
    for(Instance* inst: *instances) {
        if(inst->name == instance_name) {
            instance = inst;
            break;
        }
    }

    /*for(int i=instance->locker_indexes[0]; i < instance->locker_indexes[1]; i++) {
        Node* locker = &instance->nodes.at(i);
        double load = 0;
        for (Node* n : locker->designated_customers) {
            load += n->load_demand;
        }
        cout<<locker->id<<" "<<load<<endl;
    }*/

    Utils::print_result_file(nullptr, instance, 0, 0.0, 0.0);


    vector<string> nodes = {};
    for(Node &n: instance->nodes) {
        if(n.type == "c1" || n.type == "p") {
            nodes.push_back(n.id);
        }
    }

    for(int i=0; i<config->runs; i++) {


        //vector<int> locker_route = {-1, -1};

        int qtd_nodes = (int)nodes.size();

        srand(config->seeds.at(i));


        vector<vector<string>> routes;
        do {
            shuffle(nodes.begin(),nodes.end(), std::mt19937(std::random_device()()));
            routes = {{},{},{}};
            for(int j=0; j<qtd_nodes; j++) {
                string route_str = nodes.at(j);
                int i_route = rand()%(int)routes.size();
                int i_seq = (int)routes.at(i_route).size() == 0? 0 : rand()%(int)routes.at(i_route).size();

                vector<string>* route = &routes.at(i_route);
                if(!route_str.find("P")) {
                    //int locker_index = route[1]=='0'? 0 : 1;

                   // locker_route.at(locker_index) = i_route;
                    route_str = route_str+"()";

                }

                route->insert(route->begin()+i_seq,route_str);
            }

            for(vector<string> &route: routes) {
                route.insert(route.begin(),"D0");
                route.insert(route.end(),"Dt");
            }

            stable_sort(routes.begin(),routes.end(),criter);

            while((routes.end()-1)->size() == 2) {
                routes.erase(routes.end()-1);
            }
        }while((int)routes.size() < 3);

        Search *s = new Search(instance,config);
        s->build_predefined_solution(routes);

        if(s->solution->cost <= 182.1) {
            Utils::print_result_file(s, instance, 0, 0, 0);
        }

        if(i%10000 == 0) {
            cout<<i<<endl;
        }

        if(!Count::differs(s->solution->cost,181.8) || !Count::differs(s->solution->cost,182.0)) {
            exit(0);
        }
        delete s;

    }


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
        {"D0", "C16", "C14", "C11", "P0(C8,C15,C18,C22,C12,C13)", "C3", "Dt"},
        {"D0", "C19", "C23", "C24", "C21", "C20", "Dt"},
        {"D0", "C6","C7","P1(C2,C4,C5,C17,C1,C0)","C10","C9",  "Dt"}
        //{"D0", "C9", "C10", "P1(C2,C4,C5,C17,C1,C0)", "C7", "C6", "Dt"}
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