#include <iostream>
#include <string>
#include "Instance.h"
#include "Search.h"
#include "Utils.h"

using namespace std;
int main()
{
    int runs = 1;
    cout<<"LOADING INSTANCES"<<endl;
    vector<Instance*> instances = Utils::buildInstances("vrppl");
    //vector<Instance*> instances = Utils::buildInstances("evrptwprpl");
    cout<<"LOADING FINISHED"<<endl;

    //Utils::print_output(s);
    cout<<"RUNNING EXPERIMENTS"<<endl;
    for(Instance* instance: instances) {
        if (instance->inst_name != "R103_co_100.txt") {
            //if (instance->inst_name == "C105_co_100.txt") {
                cout<<"Instance "<< instance->inst_name<<endl;

                double best = -1.0;
                double avg=0.0;
                for(int i=0;i<runs;i++) {
                    long seed = clock();
                    srand(seed);
                    Search* search = new Search(instance);
                    search->construct();
                    if(best == -1.0 || search->total_cost < best){best = search->total_cost;}
                    avg+=search->total_cost;
                    cout<<seed<<endl;
                    search->print();
                    search->print_is_viable();
                    Utils::print_output_file(search);
                    delete search;
                }

                cout<<"AVG:"<<avg/(double)runs<<endl;
                cout<<"BEST:"<<best<<endl<<endl;
            //}
        }

    }
    //delete s;
    cout<<"EXPERIMENTS FINISHED"<<endl;
    return 0;
}
