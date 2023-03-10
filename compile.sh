#!/bin/bash

rm *.o

g++ tutorial.cpp $(geant4-config --libs) -I"${G4INCLUDE}" -o tut.o
