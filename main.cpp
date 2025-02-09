#include <iostream>
#include <string>

#include "Config.h"
#include "Instance.h"
#include "Search.h"
#include "Stats.h"
#include "Utils.h"

using namespace std;

void default_run(Instance *instance, Config* config, Stats* stats);
void grasp_run(Instance *instance, Config* config, Stats* stats);
void irace_run(int argc, char *argv[]);


int main(int argc, char *argv[])
{
    if(argc < 2) {

        cout<<"LOADING INSTANCES"<<endl;
        vector<Instance*> instances = Utils::buildInstances("vrppl");
        cout<<"LOADING FINISHED"<<endl;

        Config* config = new Config(30,0.8);
        config->print();

        cout<<"RUNNING EXPERIMENTS"<<endl;

        for(Instance* instance: instances) {

            //if(instance->inst_name == "RC103_co_25.txt") {
                cout<<"Instance "<< instance->inst_name<<endl;
                Stats* stats = new Stats(instance, config);
                default_run(instance, config, stats);

                stats->finish_stats();
                cout<<"AVG_COST: "<<stats->avg_cost<<" - AVG_TIME: "<<stats->avg_time<<" - BEST_COST: "<<stats->best_cost<<" - BEST_TIME: "<<stats->best_time<<endl;
                Utils::print_stats_file(stats);
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
        //cout<<config->seeds.at(i)<<endl;
        //cout<<config->seeds.at(i)<<endl;
        //srand(25680);// TODO inviabilidade com alpha em 0.8
        config->run = i;

        Search* search = new Search(instance,config);
        clock_t time = clock();
        search->run();
        time = clock() - time;

        stats->set_result(search->solution->total_cost,((double) time / CLOCKS_PER_SEC));
        Utils::print_route_file(search,i==0, config->seeds.at(i));
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

            if(bestSearch == nullptr || search->solution->total_cost < bestSearch->solution->total_cost) {
                delete bestSearch;
                bestSearch = search;
                bestSearchTime = time;
                bestSearch->solution->print_is_viable(config->seeds.at(i));
            }else {
                search->solution->print_is_viable(config->seeds.at(i));
                delete search;
            }

        }

        stats->set_result(bestSearch->solution->total_cost,((double) bestSearchTime / CLOCKS_PER_SEC));
        Utils::print_route_file(bestSearch,i==0, config->seeds.at(i));
        delete bestSearch;



    }
}


void irace_run(int argc, char *argv[]) {
    string instance = argv[2];
    long seed = stol(argv[4]);
    double alpha = stod(argv[6]);

    //srand(time(NULL));
    cout<<1<<endl;
}