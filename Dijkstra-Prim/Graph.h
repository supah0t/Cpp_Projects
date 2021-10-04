//The reason I am grouping all my classes in this file is because PriorityQueue and ShortestPathAlgo will not
//be accessible to the user, they will be handled properly through functions of the Graph class.
//Thus creating separate header and cpp files for each class would be reduntant and not accomodate the programer/user
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <fstream>
using namespace std;


//Simple struct with 3 variables to represent an edge in the graph
struct Edge{
    int nodeA, nodeB, weight;
};

//Class to create a Graph
class Graph{

    public:
        //Constructor for assignement3, argument is name of graph file
        Graph(string filename = "graph.txt");
        //Constructor, arguments are: vectors, density and distance range(min, max distance)
        Graph(int v, double d, int minDistance, int maxDistance);
        //Function to randomly generate edges and assign values to them
        void generateEdges();
        //Returns number of vertices
        int V();
        //Returns number of edges
        int E();
        //Returns true if nodes A and B are connected
        bool adjacent(int nodeA, int nodeB);
        //Returns a list of all the neighbours of nodeA
        vector<int> neighbours(int nodeA);
        //Adds an edge between nodes A and B if it doesn't already exist and assigns it the given value
        void add(int nodeA, int nodeB, int value);
        //Removes the edge between nodes A and B if it exists
        void remove(int nodeA, int nodeB);
        //Returns the value of edge A -> B
        int get_edge_value(int nodeA, int nodeB);
        //Sets value of edge A -> B
        void set_edge_value(int nodeA, int nodeB, int value);

    private:
        int vertices;
        double density;
        int allPossibleEdges;
        int minDistance, maxDistance;
        vector<vector<int>> listGraph;

};

//Basic minHeap implementation for priorityQueue
class minHeap{
    
    private:
        //Size of heap
        int size;
        //Capacity of heap
        int maxCapacity;
        //Represents the priority queue
        vector<Edge> heap;
        //Returns index of parent element
        int parent(int i){return (i - 1) / 2;}
        //Returns indexes of left/right children elements
        int left(int i){return 2 * i + 1;}
        int right(int i){return 2 * i + 2;}

    public:
        //Constructor
        minHeap(int maxCapacity);
        //Empties the minHeap
        void empty();
        //Inserts or updates an edge+value
        void insert(int startNode, int destinationNode, int value);
        //Compares element's value with its parents
        void swim(int i);
        //Compares element's value with its children
        void heapify(int i);
        //Removes highest priority element and recreates heap structure
        Edge extractMin();
        //Prints all the elements
        void printHeap();
        //Returns size of heap
        int Size();
};

//Class to store the logic of Dijkstra's algorithm
class ShortestPathAlgo{

    public:
        //Constructor
        ShortestPathAlgo(Graph graph);
        //Returns list of vertices of the graph
        void numberOfVertices();
        //Find the shortest path between u-w and returns the sequence of vertices representing shortest path
        vector<Edge> Path(int startNode, int endNode);
        //Constructs shortest path from closedQueue
        vector<Edge> ActualPath(vector<Edge> closedQueue, int startNode, int endNode);
        //Returns the path cost associated with the shortest path
        int path_size();
        //Prints in a nice format the shortest path
        void pretty_print(vector<Edge> shortestPath);
        //Returns weight of shortest path
        int path_weight(vector<Edge> shortestPath);

    private:
        //closedQueue is a vector of a pair of ints and an int
        //so that we can store the pair of connected nodes and the weight of the edge
        vector<Edge> closedQueue;
        Graph graph;
        int shortestPathCost;
        minHeap priorityQueue;
};

//Class to store logic for generating MST using Prim's algorithm
class MSTalgo{
    public:
        //Constructor
        MSTalgo(Graph graph);
        //Contains Prim's algorithm logic
        vector<Edge> MSTalgoLogic();
        //Prints the mst in a nice format
        void pretty_print(vector<Edge> mst);

    private:
        vector<Edge> mst;
        Graph graph;
        minHeap priorityQueue;
};


#endif //GRAPH_H
