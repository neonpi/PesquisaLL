//
// Created by Rafael on 06/01/2025.
//

#ifndef EVRPTWPRPL_H
#define EVRPTWPRPL_H
#include "../Utils.h"
#include <fstream>
#include "Factory.h"


class Evrptwprpl: public Factory {
    public:
    static Instance* buildInstance(string fileName);
};



#endif //EVRPTWPRPL_H
