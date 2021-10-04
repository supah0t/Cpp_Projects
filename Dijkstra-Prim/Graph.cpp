#include "Graph.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
using namespace std;

//Just picking a big number, should change based on graph size
const int HEAP_CAPACITY = 100;


//  INDEX
//  -------
//  1: Graph
//  2: minHeap
//  3: ShortestPathAlgo
//  4: MSTalgo


//-----------------------------------------------------------------------------
//                              1: Graph
//-----------------------------------------------------------------------------

//Constructor for graph, arguments are: vectors, density and distance range (min distance, max distance)
Graph::Graph(int v, double d, int minDistance, int maxDistance){
    //Creates a 2D (two dimensional) vector, basically a matrix, and initializes all values to 0
    vector<vector<int>> generatedGraph(v, vector<int> (v, 0));
    //Populates the classe's variables
    listGraph = generatedGraph;
    vertices = v;
    density = d;
    this->minDistance = minDistance;
    this->maxDistance = maxDistance;

    //Calculates the total ammount of possible edges
    allPossibleEdges = 0;
    for(int i = 0; i < v; i++){
        for(int j = i + 1; j < v; j++){
            allPossibleEdges++;
        }
    }
    generateEdges();
}

//Overloaded constuctor to accept filename of graph file as argument
Graph::Graph(string filename){
    vertices = 0;
    ifstream data_file("graph.txt"); //text file containing graph in format
                                     //(i, j, cost) \n
    istream_iterator<int> start(data_file), end; //iterator for the file
    vector<int> data(start, end); //vector containing the graph
    //First number in the file is the number of nodes
    int numberOfNodes = data[0];
    //Updates vertices variable 
    vertices = numberOfNodes;
    //Creates 2D vector, basically a matrix, and initializes all values to 
    //generatedGraph[0][2] = 3 means: edge from node 0 to 2 has weight 3
    vector<vector<int>> generatedGraph(numberOfNodes, vector<int> (
                numberOfNodes, 0));
    listGraph = generatedGraph;
    //Populates the graph with nodes from file
    for(int i = 1; i < data.size(); i+=3){ //every triplet is an edge
        //i is first node, i + 1 is second node, i + 2 is weight of edge
        listGraph[data[i]][data[i+1]] = data[i+2];
    }
}

//Function to calculate the appropriate number of edges and assign them randomly with a random value
void Graph::generateEdges(){
    //----This is my implementation of randomly distributing the number of edges that will be in the graph------
    //Calculates the actual number of edges the graph will have based on density(will be rounded down to tens)
    int actualNumberOfEdges = allPossibleEdges * density / 100;
    //Creates a vector with size the number of all the possible edges in the graph and initializes it to zero
    vector<int> listOfEdges(allPossibleEdges, 0);
    //Adds non zero values equal to the actualNumberOfEdges
    for(int k = 0; k < actualNumberOfEdges; k++){
    //Assigns a random value within the distance range to a number of elements equal 
    //to the number of edges that the graph will have
        listOfEdges[k] = (rand() % maxDistance) + minDistance;
    }
    //At this point listOfEdges is a vector of the size of the total possible edges of our graph
    //And a number of it's elements, equal to the number of edges our graph will have based on its density, has
    //Random values within the given distance range
    //Creates index variable
    int index;
    //Loops through every possible edge of the graph
    for(int i = 0; i < vertices; i++){
        //Ignores douplicates because the edges are two directional
        for(int j = i + 1; j < vertices; j++){
            //Randomly choses an index of an element from our list
            index = rand() % listOfEdges.size();
            //Assigns the value of the indexed element to both directions of the edge
            listGraph[i][j] = listGraph[j][i] = listOfEdges[index];
            //Removes the element from our list so that each value gets chosen once
            listOfEdges.erase(listOfEdges.begin() + index);
        }
    }
    return;
}

//Returns number of vertices in graph
int Graph::V(){ return vertices; }

//Returns number of edges in graph
int Graph::E(){ return allPossibleEdges; }

//Returns true if node A and B are connected
bool Graph::adjacent(int nodeA, int nodeB){
    if(listGraph[nodeA][nodeB] != 0)
        return true;
    return false;
}

//Returns a list of all the neighbours of nodeA
vector<int> Graph::neighbours(int nodeA){
    vector<int> neighboursList;
    for(int i = 0; i < listGraph[nodeA].size(); i++){
        if(listGraph[nodeA][i] != 0){
            //neighboursList.push_back(listGraph[nodeA][i]);
            neighboursList.push_back(i);
        }
    }
    return neighboursList;
}

//Adds an edge between nodeA and nodeB if it doesn't already exist and assigns it a value
void Graph::add(int nodeA, int nodeB, int value){
    for(int i = 0; i < listGraph[nodeA].size(); i++){
        //If the node already exists the function ends
        if(listGraph[nodeA][nodeB] != 0)
            return;
    }
    //Bidirectional graph, so it creates an edge for each vertice, -1 means there is an edge without a value
    listGraph[nodeA][nodeB] = value;
    listGraph[nodeB][nodeA] = value;
}

//Removes the edge between nodeA and nodeB if it exists
void Graph::remove(int nodeA, int nodeB){
    //We only check for A -> B and since it's bidirectional we remove B -> A as well
    if(listGraph[nodeA][nodeB] != 0){
        listGraph[nodeA][nodeB] = 0;
        listGraph[nodeB][nodeA] = 0;
    }
}

//Returns edge value from nodeA to nodeB
int Graph::get_edge_value(int nodeA, int nodeB){
    return listGraph[nodeA][nodeB];
}

//Sets edge value from nodeA to nodeB
void Graph::set_edge_value(int nodeA, int nodeB, int value){
    listGraph[nodeA][nodeB] = value;
}


//-----------------------------------------------------------------------------
//                            2: minHeap
//-----------------------------------------------------------------------------

//Constructor
minHeap::minHeap(int maxCapacity){
    size = 0;
    this->maxCapacity = maxCapacity;
    heap.resize(maxCapacity);
}

void minHeap::empty(){
    if(!heap.empty())
    heap.clear();
}

//Function to insert an edge in the queue
//If the edge already exists, it gives it the lower value between it's current and new value
//If the edge doesn't exist, it inserts it in the heap
void minHeap::insert(int startNode, int destinationNode, int value){
    //Checks if the edge already exists
    bool exists = false;
    int position = 0;
    for(int i = 0; i < size; i++){
        exists = false;
        if(heap[i].nodeB == destinationNode){
            exists = true;
            //Stores position
            position = i;
            break;
        }
    }
    //If the edge exists and its current value is less than the new one, we exit the function
    if(exists && heap[position].weight <= value){
        return; 
    //If the edge exists and its current value is larger than the new one
    //we replace the old value, and call swim to compare the new value with
    //the parent values (since newValue < oldValue we only have to check higher in the tree to maintain structure)
    }else if(exists && heap[position].weight > value){
        heap[position].weight = value;
        heap[position].nodeA = startNode;
        swim(position);
    //If the edge doesn't exist, we increase the heap size, add the new element
    //And compare it with its parents (since it's at the end of the heap, it can only go up)
    }else{
        size++;
        int j = size - 1;
        heap[j] = {startNode, destinationNode, value};
        swim(j);
    }
}

//Function to maintain structure, compares element with it's parent
void minHeap::swim(int i){
    while(i != 0 && heap[parent(i)].weight > heap[i].weight){
        swap(heap[i], heap[parent(i)]);
        i = parent(i);
    }
}

//Recursive function to maintain structure, compares element with it's children
void minHeap::heapify(int i){
    int l = left(i);
    int r = right(i);
    int smallest = i;

    //Find the smallest element of the three
    if((l < size) && (heap[l].weight < heap[smallest].weight)){
        smallest = l; 
    }if((r < size) && (heap[r].weight < heap[smallest].weight)){
        smallest = r;
    }

    //If the smallest of l or r, continue heapify
    if(smallest != i){
        swap(heap[i], heap[smallest]);
        heapify(smallest);
    }
}

//Removes the smallest element and fixes the order
Edge minHeap::extractMin(){
    //Check if the heap is empty
    if(size == 0){
        cout << "EMPTY HEAP" << endl;
        return {};
        //Should return something here
    }else if(size == 1){
        size--;
        return heap[0];
        //Normal extraction
    }else{
        //Store the root
        Edge root = heap[0];

        //Maintain heap shape and then order
        heap[0] = heap[size - 1];
        size--;
        heapify(0);

        //Return min element
        return root;
    }
}

//Prints heap's contents
void minHeap::printHeap(){
    for(int i = 0; i < size; i++)
        cout << "NodeA: " << heap[i].nodeA << ", NodeB: " << heap[i].nodeB << ", Weight: " << heap[i].weight << endl;
}

int minHeap::Size(){
    return size;
}

//-----------------------------------------------------------------------------
//                      3: ShortestPathAlgo
//-----------------------------------------------------------------------------

//Constructor
//Takes as argument a graph
ShortestPathAlgo::ShortestPathAlgo(Graph graph):graph(graph), priorityQueue(HEAP_CAPACITY){}

//Returns list of vertices
void ShortestPathAlgo::numberOfVertices(){
    return graph.generateEdges();
}

//Function to produce the sequence of edges from start to end node, by processing the closedQueue
vector<Edge> ShortestPathAlgo::ActualPath(vector<Edge> closedQueue, int startNode, int endNode){
    vector<Edge> finalPath;
    int currentNode = endNode;
    for(int i = closedQueue.size() - 1; i >= 0; i--){
        if(closedQueue[i].nodeB == currentNode){
            finalPath.push_back(closedQueue[i]);
            currentNode = closedQueue[i].nodeA;
            if(currentNode == startNode){
                return finalPath;
            }
        }
    }

    return {{0, 0, -3}};
}

//Finds the shortest path between u-w and returns the sequence of vertices representing shortest path
//Basically all the logic for Dijkstra's algorithm
vector<Edge> ShortestPathAlgo::Path(int startNode, int endNode){
    //Initialize path cost to 0
    shortestPathCost = 0;
    //Checks whether the given nodes exist in the graph, if not returns -1 so that main can throw an exception
    //and terminate the program
    if(startNode >= graph.V() || endNode >= graph.V()){
        return {{0, 0, -1}};
    }
    //Checks if start and end node are not equal
    if(startNode == endNode){
        return {{0, 0, -2}};
    }
    //Steps of the algorithm
    //1.Empty closed queue and open queue
    closedQueue = {};
    //priorityQueue.empty();
    //Variables used in the algorithm
    int currentNode, previousNode;
    Edge currentEdge;
    int distance;
    //Checking if heap reached max capacity
    if(priorityQueue.Size() == HEAP_CAPACITY)
        return {{0, 0, -4}};
    //2.Set the start node as the current node
    priorityQueue.insert(startNode, startNode, 0);
    //3.For the current node, checks all the unvisited neighbouring nodes and adds them 
    //  with their distances to the priority queue
    while(true){
        //Current Edge(previous node, current node, distance until now)
        currentEdge = priorityQueue.extractMin();
        //Previous node from the faster path
        previousNode = currentEdge.nodeA;
        //Current node
        currentNode = currentEdge.nodeB;
        //If the end node is the highest priority node, we have found the shortest path
        //We add the node to the closed queue and return the queue
        if(currentNode == endNode){
            priorityQueue.empty();
            shortestPathCost = currentEdge.weight;
            closedQueue.push_back({previousNode, currentNode, currentEdge.weight});
            vector<Edge> finalPath = ActualPath(closedQueue, startNode, endNode);
            return finalPath;
        }

        bool exists = false;
        //List of neighbours of the current node
        vector<int> neighboursList = graph.neighbours(currentNode);
        //Loops through neighbours
        for(auto i : neighboursList){
            exists = false;
            //Checks if neighbour exists in closed queue
            for(auto j : closedQueue){
                if(j.nodeB == i){
                    exists = true;
                    //If it does, move to next neighbour
                    break;
                }
            }
            //If it doesn't, adds neighbour to priority queue
            if(!exists){
                distance = currentEdge.weight + graph.get_edge_value(currentNode, i);
                priorityQueue.insert(currentNode, i, distance);
                

            }
        }
        //After checking all neighbours, marks current node as closed
        closedQueue.push_back({previousNode, currentNode, currentEdge.weight});
        //If open queue is empty, since we haven't reached our end node yet. A path doesn't exist
        if(priorityQueue.Size() == 0){
            priorityQueue.empty();
            shortestPathCost = 0;
            return {};
        }
    }
}

//Returns the path cost associated with the shortest path
int ShortestPathAlgo::path_size(){
    return shortestPathCost;
}

//Prints the sequence of edges in a nice format
void ShortestPathAlgo::pretty_print(vector<Edge> shortestPath){
    cout << "Shortest path: \n";
    if(shortestPath.size() == 0){
        return;
    }else if(shortestPath.size() == 1){
        cout << "Start -> " << shortestPath[0].nodeA << " -> " << shortestPath[0].nodeB << " -> ";
    }else{
        cout << "Start -> " << shortestPath[shortestPath.size() - 1].nodeA << " -> ";
        for(int i = shortestPath.size() - 1; i >= 0; i--){
            cout << shortestPath[i].nodeB << " -> ";
        }
    }
    cout << "End\n";
    cout << "Cost: " << shortestPath[0].weight << endl;
}

//Returns cost of the shortest path
int ShortestPathAlgo::path_weight(vector<Edge> shortestPath){
    //If shortestPath is empty, it means there is no path available
    if(shortestPath.size() == 0)
        return 0;
    else
        return shortestPath[0].weight;
}

//-----------------------------------------------------------------------------
//                            4: MSTalgo
//-----------------------------------------------------------------------------

//Constructor, argument is a graph
MSTalgo::MSTalgo(Graph graph)
    :graph(graph), priorityQueue(HEAP_CAPACITY){}

//Prim's algorithm logic
vector<Edge> MSTalgo::MSTalgoLogic(){
    mst.clear();
    //Number of vertices in the graph
    int vertices = graph.V();
    //Creates empty vector for visited nodes
    vector<int> visitedNodes;
    //Randomly chose the node from which the algorithm will start
    int startNode = rand() % 20;
    //Sets current node
    int currentNode = startNode;
    //Helper variables for the loop
    bool exists;
    //Loops until we have completed the mst
    while(mst.size() < vertices - 1){
        //List of neighbours
        vector<int> neighbours = graph.neighbours(currentNode);
        visitedNodes.push_back(currentNode);
        //Loops through all neighbours
        for(auto i : neighbours){
            exists = false;
            //Checks if the current neighbour has already been visited
            for(auto j: visitedNodes){
                if(i == j){
                    exists = true;
                    break;
                }
            }
            //If it hasn't, add to open queue
            if(exists == false){
                priorityQueue.insert(
                    currentNode, i, graph.get_edge_value(currentNode, i));
            }
        }
        //If priorityQueue is eimpty and we haven't found mst, it doesn't exist
        if(priorityQueue.Size() == 0){
            cout << "Mst size: " << mst.size() << ", Vertices: " <<
                vertices - 1 << endl;
            cout << "Exit because priorityQueue is empty\n";
            return {};
        }

        //Adds current priority edge to mst
        Edge currentEdge = priorityQueue.extractMin();
        mst.push_back(currentEdge);
        //Sets current node for next loop
        currentNode = currentEdge.nodeB;
    }
    return mst;
}

//Prints the sequence of edges in a nice format
void MSTalgo::pretty_print(vector<Edge> mst){
    cout << "MST: \n";
    if(mst.size() == 0){
        return;
    }else{
        cout << "Edges:\t\t" << "Values:" << endl;
        for(int i = 0; i < mst.size(); i++){
            cout << mst[i].nodeA << " -> " << mst[i].nodeB << "\t\t" << 
                mst[i].weight << endl;
        }
    }
}
