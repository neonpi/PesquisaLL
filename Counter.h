//
// Created by Rafael on 06/03/2025.
//

#ifndef COUNT_H
#define COUNT_H

using namespace std;
class Count {
public:
    static bool differs(double a, double b, double epsilon = 0.001) {return abs(a-b) > epsilon;};
    static bool improves(double best, double current) {return current < best && differs(best,current);};

};

#endif //COUNT_H
