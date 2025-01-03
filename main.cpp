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

    //Utils::print_output(s);
    for(Instance* i: instances) {
        if (i->inst_name == "c206C5_pl.dat") {
        //if (i->inst_name == "c101C5_pl.dat") {
            cout<<"Instance "<< i->inst_name<<endl;
            Search* s = new Search(i,2);
            s->construct();
            s->print_is_viable();
            Utils::print_output(s);
            s->print();
            delete s;
            cout<<endl<<endl;
        }

    }
    //delete s;
    cout<<"FINISHED"<<endl;
    return 0;
}
