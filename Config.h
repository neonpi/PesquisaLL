//
// Created by Rafael on 12/01/2025.
//

#ifndef CONFIG_H
#define CONFIG_H
#include <ctime>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
class Config {
public:
    Config(int runs, double alpha) {
        this->runs = runs;
        this->alpha = alpha;
        //srand(clock());
        srand(time(NULL));
        for(int i=0;i<runs;i++) {
            this->seeds.push_back(rand());
        }
    };
    int runs;
    vector<long> seeds;
    double alpha;

    void print() {
        ofstream file;
        file.open("Output/0_config",ofstream::out);
        file<<"runs: "<<to_string(this->runs)<<endl;
        file<<"alpha: "<<to_string(this->alpha)<<endl;
        file<<"seeds:"<<endl;
        for(int i=0;i<this->runs;i++) {
            file<<to_string(i)<<"-"<<this->seeds.at(i)<<endl;
        }

        file.close();
    }

};


#endif //CONFIG_H
