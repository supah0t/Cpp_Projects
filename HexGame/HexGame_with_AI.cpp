#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>
#include <chrono>
using namespace std;

//Size of board
const int Size = 9;

class HexGame{
    public:
        //Constructor
        HexGame(int size);
        //Prints instructions at the begining of the game
        bool Instructions();
        //Reponsible for gameplay, returns player who won
        int GamePlay();
        //Runs monte carlo simulations for the CPU
        double MonteCarlo(bool whichTurn);
        //Same with board parameter
        double MonteCarlo(bool whichTurn, 
                vector<int> oneDimensionalBoard);
        //Timing variable for checkwin
        double totalTime;
    private:
        // ------------ Functions ------------

        //Draws the board with players' tiles
        void DrawBoard(vector<vector<int>> board);
        //Creates the "backend" for the game
        void GenerateGame();
        //Making a move with 2D coordinates
        void MakeMove(int x, int y, bool player);
        //Makes move with 1D coordinates
        void MakeMove(int i, bool player);
        //CPU move
        void MakeMoveCPU(bool player);
        //Checks if move is legal
        bool IsLegal(int x, int y);
        //Checks if game is over
        //Finds all tiles that are on the correct edge
        bool CheckEdgeTiles();
        //Finds all neighbouring pieces for default board
        vector<pair<int, int>> CheckNeighbours(int x, int y, bool player);
        //Finds all neighbouring pieces for specified board
        vector<pair<int, int>> CheckNeighbours(int x, int y, bool player,
                vector<vector<int>> myBoard);
        //Checks if game is over
        bool CheckWin(bool player, vector<vector<int>> baord);

        // ----------- Variables --------------

        //Vector holding the board the the moves
        vector<vector<int>> board;
        //Size of board
        int size;
        //Maps numbers with symbols for players' moves
        map<int, string> pieces;
        bool gameOver;
        //Vector to hold empty squares in 1d representation
        vector<int> board1D;
        //Variable to keep track of empty squares
        int emptySquares;
};

HexGame::HexGame(int size):size(size),emptySquares(size*size){
    pieces.insert(make_pair(0, ". "));
    pieces.insert(make_pair(1, "o "));
    pieces.insert(make_pair(2, "x "));
    GenerateGame();
    //Initializes 1D board with zero
    vector<int> tempBoard1D(size*size, 0);
    board1D = tempBoard1D;
}

//Displays instructions for gameplay
bool HexGame::Instructions(){
    string answer;
    cout << "\nWELCOME!!!\n";
    cout << "\nGAMEPLAY INSTRUCTIONS\n";
    cout << "The goal of the game is to create a continuous sequence "
        << "of tiles that 'touches' two oposite sides of the board.\n"
        << "You and the oponent take turns to place a tile "
        << "in an empty square.\n"
        << "The way you specify the position you wish to place your tile is by"
        << " inputing a x and a y value.\nIf the square is occupied you will"
        << " be prompted to enter a new position\n";
    cout << "\nYou, as a human will place 'o' tiles\n";
    cout << "And you have to connect LEFT AND RIGHT";
    cout << " CPU aims for top bottom\n\n";
    cout << "Proceed? (Y/N): ";
    cin >> answer;
    if(answer == "yes" || answer == "Y" || answer == "y")
        cout << "\nEnjoy!\n";
        return true;
    return false;
}

//Function to draw the board
//Uses the map to draw symbols representing each player's tiles
void HexGame::DrawBoard(vector<vector<int>> board){
    cout << "--------------------------------------------------\n";
    cout << endl;
    for(int i = 0; i < size; i++){
        for(int k = 0; k < i; k++){
            cout << "  ";
        }
        cout << pieces[board[i][0]];
        for(int j = 0; j < size - 1; j++){
            cout << "- " << pieces[board[i][j+1]];
        }
        cout << endl;

        if(i == size - 1){
            return;
        }

        for(int c = 0; c < 2*i + 1; c++){
            cout << " ";
        }
        cout << "\\ ";
        for(int f = 0; f < size - 1; f++){
            cout << "/ \\ ";
        }
        cout << endl;
    }
}

//Creates vector to hold moves
void HexGame::GenerateGame(){
    vector<vector<int>> board(size, vector<int>(size, 0));
    this->board = board;
}

//Makes a move
void HexGame::MakeMove(int x, int y, bool player){
    //Makes move on the board
    board[x][y] = player + 1;
    //Converts x, y to 1D index
    int index = size*x + y;
    //Makes move on 1D board representation
    board1D[index] = player + 1;
    emptySquares--;
}

void HexGame::MakeMove(int i, bool player){
    //Makes move on 1D board
    board1D[i] = player + 1;
    //Converts values and makes move on 2D board
    int x = i / size;
    int y = i % size;
    board[x][y] = player + 1;
    emptySquares--;
}

//Checks every legal move with monte carlo simulations
void HexGame::MakeMoveCPU(bool player){
    //Variable to hold a move with its win ratio
    pair<int, double> best_move;
    for(int i=0; i<board1D.size(); i++){
        vector<int> tempBoard1D = board1D;
        if(tempBoard1D[i] == 0){
            tempBoard1D[i] = player + 1;
            double winChance = MonteCarlo(0, tempBoard1D);
            if(winChance > best_move.second){
                best_move = {i, winChance};
            tempBoard1D[i] = 0;
            }
        }
    }
    MakeMove(best_move.first, player);
    return;
}

//Checks if move is legal
bool HexGame::IsLegal(int x, int y){
    if(board[x][y] == 0)
        return true;
    cout << "Illegal move, play again\n";
    return false;
}

//Returns list of neighbours for default board
vector<pair<int, int>> HexGame::CheckNeighbours(int x, int y, bool player){
    vector<pair<int, int>> neighbours;
    //Calls specified version with default board as parameter
    neighbours = CheckNeighbours(x, y, player, board);

    return neighbours;
}

//Returns list of neighbours for specified board
vector<pair<int, int>> HexGame::CheckNeighbours(int x, int y, bool player,
        vector<vector<int>> myBoard){

    vector<pair<int, int>> neighbours;
    //Checks every neighbour for same player tiles
    if((x + 1 < size) && (myBoard[x + 1][y] == player + 1))
        neighbours.push_back({x + 1, y});
    if((x - 1 >= 0) && (myBoard[x - 1][y] == player + 1))
        neighbours.push_back({x - 1, y});
    if((y + 1 < size) && (myBoard[x][y + 1] == player + 1))
        neighbours.push_back({x, y + 1});
    if((y - 1 >= 0) && (myBoard[x][y - 1] == player + 1))
        neighbours.push_back({x, y - 1});
    if((x + 1 < size) && (y - 1 >= 0) && (myBoard[x + 1][y - 1] == player + 1))
        neighbours.push_back({x + 1, y - 1});
    if((x - 1 >= 0) && (y + 1 < size) && (myBoard[x - 1][y + 1] == player + 1))
        neighbours.push_back({x - 1, y + 1});
    return neighbours;
}

//Checks the whole board for win condition
//Unoptimized version
bool HexGame::CheckWin(bool player, vector<vector<int>> myBoard){
    //Holds already visited tiles
    vector<pair<int, int>> closedList;
    //Holds not yet visited tiles
    vector<pair<int, int>> openList;
    //Holds edge tiles that could potentially form a win condition
    //As well as a string indicating which edge it is on
    vector<pair<pair<int, int>, string>> edgeTilesList;
    //Temporary list to assist in loops
    vector<pair<int, int>> tempList;
    //Holds side
    string side;

    //Checks edge tiles based on who made the move
    if(player == 0){
        for(int i = 0; i < size; i++){
            if(myBoard[i][0] == player + 1){
                edgeTilesList.push_back({{i, 0}, "Left"});
            }
            if(myBoard[i][size - 1] == player + 1){
                edgeTilesList.push_back({{i, size - 1}, "Right"});
            }
        }
    }else{
        for(int i = 0; i < size; i++){
            if(myBoard[0][i] == player + 1){
                edgeTilesList.push_back({{0, i}, "Up"});
            }
            if(myBoard[size - 1][i] == player + 1){
                edgeTilesList.push_back({{size -1, i}, "Down"});
            }
        }
    }

    //Returns if there are no edge tiles
    if(edgeTilesList.size() == 0)
        return false;

    for(auto i : edgeTilesList){
        //Adds the edge tile to open list
        openList.push_back(i.first);
        //Stores which side the tile is on to check for win later
        side = i.second;
        //Until all neighbours have been checked
        while(openList.size() != 0){
            //Stores neighbours to check
            tempList = CheckNeighbours(openList[0].first,
                    openList[0].second, player, myBoard);
            //Won't be checked again
            closedList.push_back(openList[0]);
            //Removes it from open list
            openList.erase(openList.begin());
            //For all neighbours, checks if it's already been checked, then
            //checks if it wins

            for(auto j : tempList){
                auto it = find(closedList.begin(), closedList.end(), j);

                //Checks for win
                if(it == closedList.end()){
                    //Adds tile to open list
                    openList.push_back(j);
                    if(side == "Left" && j.second == size - 1)
                        return true;
                    if(side == "Right" && j.second == 0)
                        return true;

                    if(side == "Up" && j.first == size - 1)
                        return true;

                    if(side == "Down" && j.first == 0)
                        return true;
                }
            }
        }
    }

    return false;
}

int HexGame::GamePlay(){
    bool player = false;
    int x, y = 0;
    gameOver = false;
    //Loops until gameover
    while(!gameOver){
        //Player turn
        if(player == 0){
            DrawBoard(board);
            //Keeps asking until valid move
            do{
                cout << "Player turn\n";
                do{
                cout << "Input x position: ";
                cin >> x;
                }while(x < 0 || x >= size);
                do{
                cout << "Input y position: ";
                cin >> y;
                }while(y < 0 || y >= size);
            }while(!IsLegal(x, y));
            //Makes the move
            MakeMove(x, y, player);
            //Checks for win
            if(CheckWin(player, board)){
                DrawBoard(board);
                return player;
            }
        }else{
            MakeMoveCPU(1);
            if(CheckWin(player, board))
                return player;
        }
        //Changes current player
        player = !player;
    }
    return -1;
}

double HexGame::MonteCarlo(bool whichTurn){
    return MonteCarlo(whichTurn, board1D);
}

//Returns win chance for the oposite player
double HexGame::MonteCarlo(bool whichTurn, 
        vector<int> oneDimensionalBoard){
    //Number of times it will check the board
    int tries = 1000;
    //Win counters for each player
    double playerOneWinCount = 0;
    double playerTwoWinCount = 0;
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);

    for(int i=0; i<tries; i++){
        vector<int> tempBoard1D = oneDimensionalBoard;
        int tempEmptySquares = emptySquares;
        //Random numbers
        //Current player
        bool turn = whichTurn;
        //Populates an array with 1 and 2 values
        vector<int> values(tempEmptySquares, 0);
        for(int i=0; i<values.size(); i++){
            values[i] = turn + 1;
            turn = !turn;
        }

        //Shuffles that array
        shuffle(values.begin(), values.end(), rng);
        //Populates the board1d with 1 and 2 at random
        for(int i=0; i<size*size; i++){
            if(tempBoard1D[i] == 0){
                tempBoard1D[i] = values[tempEmptySquares - 1];
                tempEmptySquares--;
            }
        }
        
        //maps the 1D array of squares to the 2D board so that
        //the function to check for win can be called
        vector<vector<int>> tempBoard(size, vector<int>(size, 0));
        for(int i=0; i<size*size; i++){
            tempBoard[i/size][i%size] = tempBoard1D[i];
        }

        if(CheckWin(0, tempBoard))
            playerOneWinCount++;
        else
            playerTwoWinCount++;

    }
    if (whichTurn == 0)
        return playerTwoWinCount / tries;
    else
        return playerOneWinCount / tries;
}

//Prints win message
void PrintWin(bool player){
    cout << "  _   _   _   _   _   _     _         _   _   _ " << endl;
    cout << " / \\ / \\ / \\ / \\ / \\ / \\   / \\       / \\ / \\ / \\ " 
        << endl;
    cout << "( P | L | A | Y | E | R ) ( " << player + 1 
        << " )     ( W | O | N )" 
        << endl;
    cout << " \\_/ \\_/ \\_/ \\_/ \\_/ \\_/   \\_/       \\_/ \\_/ \\_/ " 
        << endl;
}

int main(){

    //Seeds rng
    srand(time(0));
    //Creates board and prints it
    HexGame game(Size);
    //Presents instructions and asks user if they want to play
    bool answer = game.Instructions();
    if(!answer){
        cout << "See ya!\n";
        return 0;
    }
    //Starts the game and stores the winner
    int winner = game.GamePlay();

    //-1 for error handling
    if(winner == -1)
        cout << "Some error occured, game terminated";
    //Prints congratulatory message
    else
        PrintWin(winner);

    return 0;
}



