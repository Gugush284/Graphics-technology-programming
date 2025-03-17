#!/bin/bash

g++ ./in.cpp
./a.out
g++ ./main.cpp -lGL -lGLU -lglut -lm
./a.out