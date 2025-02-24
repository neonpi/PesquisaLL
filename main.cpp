#include <iostream>
#include <string>

#include "vector"
#include "Config.h"
#include "Instance.h"
#include "Search.h"
#include "Stats.h"
#include "Utils.h"
#include "instance_factory/Vrppl.h"

using namespace std;

void default_run(Instance *instance, Config* config, Stats* stats);
void grasp_run(Instance *instance, Config* config, Stats* stats);
void test_solution(Instance *instance, Config* config, Stats* stats);
void irace_run(int argc, char *argv[]);


int main(int argc, char *argv[])
{
    if(argc < 2) {

        cout<<"LOADING INSTANCES"<<endl;
        vector<Instance*> instances = Utils::buildInstances("vrppl");
        cout<<"LOADING FINISHED"<<endl;

        Config* config = new Config(30,0.2);
        config->print();

        cout<<"RUNNING EXPERIMENTS"<<endl;

        for(Instance* instance: instances) {

            //if(instance->inst_name == "C106_co_100.txt") {
                cout<<"Instance "<< instance->inst_name<<endl;
                Stats* stats = new Stats(instance, config);
                Utils::print_result_file(nullptr, instance, 0, 0.0, 0.0);

                //test_solution(instance, config, stats);
                default_run(instance, config, stats);

                stats->finish_stats();
                Utils::print_screen_run(stats);
                Utils::print_final_stats(stats);
                delete stats;
            //}

        }

        delete config;
        cout<<"EXPERIMENTS FINISHED"<<endl;
    }else {
        irace_run(argc, argv);
    }
    return 0;
}

void default_run(Instance *instance, Config* config, Stats* stats) {
    for(int i=0;i<config->runs;i++) {

        srand(config->seeds.at(i));
        //srand(19360);
        config->run = i;

        Search* search = new Search(instance,config);
        clock_t time = clock();
        search->debug_run();
        //search->solution->print();
        //search->run();
        time = clock() - time;

        stats->set_result(search->solution,((double) time / CLOCKS_PER_SEC));
        Utils::print_result_file(search, instance, i, (double) time / CLOCKS_PER_SEC, config->seeds.at(i));
        search->solution->print_is_viable(config->seeds.at(i));
        delete search;



    }
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
                bestSearch->solution->print_is_viable(config->seeds.at(i));
            }else {
                search->solution->print_is_viable(config->seeds.at(i));
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
    double alpha = stod(argv[6]);

    Config* config = new Config(1,alpha);
    Instance* instance = Vrppl::buildInstance(instance_name);

    Search* search = new Search(instance,config);

    search->construct();
    cout<<search->solution->cost<<endl;

    delete config;
    delete instance;
    delete search;
}

void test_solution(Instance *instance, Config* config, Stats* stats) {
        /*{"C18", "P0-C2","P0-C6","P0-C7","P0-C8","P0-C10","P0-C11","P0-C17","P0-C19","P0-C23", "C9", "C0"},
        {"C14", "P1-C1", "P1-C3", "P1-C4", "P1-C5", "P1-C12", "P1-C13", "P1-C15", "P1-C20", "P1-C21", "P1-C22", "P1-C24", "C16"}
    */
    vector<vector<string>> routes = {
    {"C18","P0-C2","P0-C6","P0-C7","P0-C8","P0-C10","P0-C11","P0-C17","P0-C19","P0-C23","C9","C0"},
    {"C14","P1-C1", "P1-C3", "P1-C4", "P1-C5", "P1-C12", "P1-C13", "P1-C15", "P1-C20", "P1-C21", "P1-C22", "P1-C24","C16"}
    };

    Search *s = new Search(instance,config);

    s->build_predefined_solution(routes);

    delete s;


}
