//
// Created by Rafael on 12/01/2025.
//

#ifndef CONFIG_H
#define CONFIG_H
#include <ctime>
#include <vector>
#include <string>

using namespace std;
class Config {
public:
    Config(int runs, double alpha) {
        this->runs = runs;
        this->alpha = alpha;
        srand(clock());
        for(int i=0;i<runs;i++) {
            this->seeds.push_back(rand());
        }
    };
    int runs;
    vector<long> seeds;
    double alpha;

};


#endif //CONFIG_H
