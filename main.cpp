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

    vector<Instance*> instances = Utils::buildInstances();

    //instances.at(0)->print();

   /* Search* s = new Search(instances.at(0),2);
    //s->construct();
    //s->construct_2(0.5, 10);
    s->insertion_heuristic();
    s->is_viable();
    s->print();*/

    //Utils::print_output(s);
    for(Instance* i: instances) {
        //if (i->inst_name == "c206C5_pl.dat") {
            cout<<"Instance "<< i->inst_name<<endl;
            Search* s = new Search(i,2);
            s->construct_3();
            s->print_is_viable();
            Utils::print_output(s);
            s->print();
            delete s;
            cout<<endl<<endl;
        //}

    }
    //delete s;
    cout<<"FINISHED"<<endl;
    return 0;
}
