//
// Created by Rafael on 24/11/2024.
//

#ifndef UTILS_H
#define UTILS_H
#include "Instance.h"
#include <fstream>
#include <vector>
#include <sstream>
#include "Search.h"


class Utils {
public:
    static vector<Instance*> buildInstances(string problem);
    static void print_output(Search* search);
    static vector<string> tookenize(string str, string symbol);

private:
    static Instance* buildInstance(string fileName);
    static void defineNodeIndexes(Instance* instance);

};



#endif //UTILS_H
