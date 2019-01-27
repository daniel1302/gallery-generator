#!/bin/bash

g++ \
    Config.hpp \
    Config.cpp \
    Generator.hpp \
    Generator.cpp \
    main.cpp \
    -std=gnu++17 \
    -o replace \
        `Magick++-config --cxxflags --cppflags --ldflags --libs` \
        -lyaml-cpp \
        -lboost_system \
        -lboost_filesystem \
&& ./replace "config.yml"