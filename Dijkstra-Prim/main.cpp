//Homework 3: Create MST
//By: Odysseas Mastorakis

//This should be in a folder along with Graph.h and Graph.cpp
//The header file Graph.h is included and thus doesn't need to be compiled seperately
//Graph.cpp has to be compiled and linked with this file

//Example compilation using g++:
//[Folder] -> Graph.h, Graph.cpp, main.cpp
//Compiling: "g++ main.cpp Graph.cpp -o source.o"
//Executing: "./source.o"

//Custom header file for this program, should be in the same folder
#include "Graph.h"

#include <iostream>
#include <ctime>
#include <iterator>
#include <fstream>
using namespace std;


int main(){
    srand(clock());

    Graph graph;
    MSTalgo algo(graph);
    vector<Edge> mst = algo.MSTalgoLogic();
    
    int mstCost = 0;

    for(auto i : mst){
        mstCost += i.weight;
    }

    algo.pretty_print(mst);
    cout << "Total cost of mst: " << mstCost << endl;

    return 0;
}
