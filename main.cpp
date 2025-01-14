#include <iostream>
#include <string>

#include "Config.h"
#include "Instance.h"
#include "Search.h"
#include "Stats.h"
#include "Utils.h"

using namespace std;
int main()
{
    cout<<"LOADING INSTANCES"<<endl;
    vector<Instance*> instances = Utils::buildInstances("vrppl");
    cout<<"LOADING FINISHED"<<endl;

    Config* config = new Config(30,0.03);
    //Config* config = new Config(30,0.05);

    //Utils::print_output(s);
    cout<<"RUNNING EXPERIMENTS"<<endl;
    for(Instance* instance: instances) {
        if (instance->inst_name != "R103_co_100.txt") {
            cout<<"Instance "<< instance->inst_name<<endl;
            Stats* stats = new Stats(instance, config);

            for(int i=0;i<config->runs;i++) {

                srand(config->seeds.at(i));
                srand(0);

                Search* search = new Search(instance,config);
                clock_t time = clock();
                search->run();
                time = clock() - time;

                stats->set_result(search->total_cost,((double) time / CLOCKS_PER_SEC));
                Utils::print_route_file(search,i==0, config->seeds.at(i));
                search->print_is_viable();
                delete search;


            }

            stats->finish_stats();
            cout<<"AVG_COST: "<<stats->avg_cost<<" - AVG_TIME: "<<stats->avg_time<<" - BEST_COST: "<<stats->best_cost<<" - BEST_TIME: "<<stats->best_time<<endl;
            Utils::print_stats_file(stats);
            delete stats;
        }

    }
    delete config;
    cout<<"EXPERIMENTS FINISHED"<<endl;
    return 0;
}
