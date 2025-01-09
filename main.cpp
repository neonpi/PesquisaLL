#include <iostream>
#include <string>
#include "Instance.h"
#include "Search.h"
#include "Utils.h"

using namespace std;
int main()
{
    srand(0);
    cout<<"LOADING INSTANCES"<<endl;
    vector<Instance*> instances = Utils::buildInstances("vrppl");
    //vector<Instance*> instances = Utils::buildInstances("evrptwprpl");
    cout<<"LOADING FINISHED"<<endl;

    //Utils::print_output(s);
    cout<<"RUNNING EXPERIMENTS"<<endl;
    for(Instance* instance: instances) {
        //i->print();
        if (instance->inst_name == "C101_co_25.txt") {
            cout<<"Instance "<< instance->inst_name<<endl;
            Search* search = new Search(instance);
            search->construct();
            search->print_is_viable();
            Utils::print_output_file(search);
            search->print();
            delete search;
            cout<<endl<<endl;
       }

    }
    //delete s;
    cout<<"EXPERIMENTS FINISHED"<<endl;
    return 0;
}
