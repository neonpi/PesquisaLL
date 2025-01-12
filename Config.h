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
        for(int i=0;i<runs;i++) {
            this->seeds.push_back(clock());
        }
    };
    int runs;
    vector<long> seeds;
    double alpha;

};


#endif //CONFIG_H
