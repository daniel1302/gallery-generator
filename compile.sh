#!/bin/bash

g++ main.cpp -std=gnu++17 -o replace -lyaml-cpp -lboost_system -lboost_filesystem && ./replace
