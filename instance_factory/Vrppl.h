//
// Created by Rafael on 06/01/2025.
//

#ifndef VRPPL_H
#define VRPPL_H
#include "../Utils.h"
#include "Factory.h"



class Vrppl: public Factory {
public:
    static Instance* buildInstance(string fileName);

};



#endif //VRPPL_H
