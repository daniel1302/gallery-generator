#include "Generator.hpp"

#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout<<"Usage: "<<argv[0]<< " CONFIG_PATH.yml"<<std::endl;
    }

    Generator g{std::string{argv[1]}};
    g.generate();
}