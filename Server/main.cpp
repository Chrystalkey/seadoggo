//
// Created by Chrystalkey on 04.02.2021.
//

#include <iostream>
#include "commandocentral.h"

int main(int argc, char **argv){
    CommandoCentral cc = CommandoCentral::init(PORT);
    cc.run();
    return 0;
}
