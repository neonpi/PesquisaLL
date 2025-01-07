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
    for(Instance* i: instances) {
        //i->print();
        //if (i->inst_name == "C101_co_25.txt") {
            cout<<"Instance "<< i->inst_name<<endl;
            Search* s = new Search(i,25);
            s->construct();
            s->print_is_viable();
            Utils::print_output_file(s);
            s->print();
            delete s;
            cout<<endl<<endl;
        //}

    }
    //delete s;
    cout<<"EXPERIMENTS FINISHED"<<endl;
    return 0;
}
