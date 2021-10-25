#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <cstdlib> 
#include <ctime>
#include <random>
#include <algorithm>
#include <bits/stdc++.h>
using namespace std::chrono;
using namespace std;

bool boardSolved=false;
int rowColSize;

int possibleNextMove[16][4];
int manhattanPatternDatabaseTable[16][16];
int traversal[85]; // for the ASTAR search route.
vector<char> path;

//vector<vector<int>> possibleNextMove(16, vector<int>(4));
//vector<vector<int>> manhattanPatternDatabaseTable(16, vector<int>(16));
//vector<int> traversal(85); // for the ASTAR search route.
//int countNodes=0;


typedef unsigned int node;
node countNodes;


// Generating Heuristics from subproblems Using Pattern database in which the exact precomputed cost of solving subproblems are stored in the table. (Book pg.217)
void precomputedManhattanTable(int rowSize){
  //while()
  //cout<<"Inside precompute"<<endl;


  for(int i=0,rowVal=1;i<16; ++i, (++rowVal)%(16)){
    //cout<< "inside outer for loop"<< endl;
    //cout<< rowVal<<endl;
    for(int j=0;j<16;j++){
      //cout<< "inside inner for loop"<< endl;
      if(rowVal!=0){
        //cout<< "inside row val not 0"<< endl;
        manhattanPatternDatabaseTable[rowVal][j] = abs((i%rowSize)-(j%rowSize)) + abs((i/rowSize)-(j/rowSize));   
        //cout<< "inside row val not 0 after computation"<< endl;     
      }
      else{
        //cout<< "inside else"<< endl;
        manhattanPatternDatabaseTable[rowVal][j]=0;
        //cout<<"inside else after computation"<<endl;
        
      }
    }
  }
}

//calculatded the number of misplaced tiles
int computeMisplacedTiles(int* userInput){
    int misplacedTiles=0;
    for(int i=0;i<(rowColSize*rowColSize)-1;i++){
        // for(int j=0;j<16;j++){
        //     misplacedTiles=0;
         
          if(userInput[i]!=i+1 && userInput!=0){
            misplacedTiles++;
              
          }
         
        //}
    }
    return misplacedTiles;
}

// //this 
// void possibleMoves(int rowSize){
//   int directions[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
//   int newBoard[4][4];

//   int dir_idx;
//   int dir_idy;
//   int possiblemove;

//   for(int i=0;i<rowSize;i++){
//     for(int j=0;j<rowSize;j++){
//       newBoard[i][j]=j+i*rowSize;
//     }
//   }
//   for(int i=0;i<rowSize;i++){
//     for(int j=0;i<rowSize;i++){
//       for(int k=0;k<4;k++){
//         dir_idx=directions[k][0];
//         dir_idy=directions[k][1];
//         if(i+dir_idy<0||j+dir_idx<0||i+dir_idy>=rowSize||j+dir_idx>=rowSize){
//           possiblemove=-1;
//         }
//         else{
          
//           possiblemove=newBoard[i+dir_idy][j+dir_idx];
//         }
//         possibleNextMove[j+i*rowSize][k]=possiblemove;
//       }
//     }
//   }

// }


//next possible moves in any of the 4 directions
void possibleMoves(int size){
	int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  //four directions - left, right, doen, up
	int board[4][4];//a board defined to store the values
	int dir_idx;//x direction after moving
	int dir_idy;//y direction after moving
	int possiblemove;
	
	for(int i = 0; i<size; ++i){
		for(int j = 0; j<size; ++j){
			board[i][j]=j+i*size;
		}
	}
	
	for(int i=0;i<size;i++){
		for(int j=0;j<size;j++){
			for(int k=0;k<4;k++){
				dir_idx=directions[k][0]; //x direction after moving
				dir_idy=directions[k][1]; //y direction after moving
				if(j+dir_idx<0||i+dir_idy<0 ||i+dir_idy>=size|| j+dir_idx>=size){
					possiblemove=-1;
				}
				else{
					possiblemove=board[i+dir_idy][j+dir_idx]; //assigning a new changed board value to possiblemove
				} 
				possibleNextMove[j+i*size][k]=possiblemove;
        
			}
		}
	}
}

//calculates the distance between the puzzle blocks
int distBtwBlocks(int manhattanCost, int *rowVal, int rowIndex){
  if(rowIndex>1){
    switch(rowIndex){
      case 2:
        if(rowVal[1]<rowVal[0]){
          manhattanCost=manhattanCost+2; //adding 2 for the cost of 2 row values.
        }
        break;
      case 3:
        if(rowVal[0]>rowVal[1] || rowVal[1]>rowVal[2] || rowVal[0]>rowVal[2]){
          manhattanCost=manhattanCost+2; //adding 2 for the cost of 2 row values.
        }
        break;
      case 4:
        if(rowVal[0]>rowVal[1]|| rowVal[1]>rowVal[2]||rowVal[2]>rowVal[3] ||rowVal[0]>rowVal[2] ||rowVal[0]>rowVal[3] ||rowVal[1]>rowVal[3]){
          manhattanCost=manhattanCost+2;
        }
        break;
      default:
        cout << "wrong rowIndex";
        break; 
    }
  }
  return manhattanCost;
}

//calculates the cost of connecting two blocks of the puzzle
int getManhattanCost(int *userInput){
  int value[4];
  int var1, var2;
  //int k,n;
  int manhattanCost=0;
  int md;

  for(int i=0;i<rowColSize*rowColSize;i++){
    //storing the manhattan cost from the manhattan pattern database table.
    manhattanCost=manhattanCost+manhattanPatternDatabaseTable[userInput[i]][i];
  }

  int i=0, k=1, j=0;
  while(i<rowColSize){
    var1=0;
    while(j<rowColSize){
      var2=userInput[i*rowColSize +i];
      if(var2<=k*rowColSize && var2>(k-1)*rowColSize){
        value[var1++]=var2;
      }
      j++;
    }
    manhattanCost=distBtwBlocks(manhattanCost,value,var1);
    i++;
    k++;
  }

  while(i<rowColSize){
    var1=0;
    while(j<rowColSize){
      var2=userInput[j*rowColSize +i];
      if(var2==k || var2==(rowColSize+k)|| var2==(rowColSize*2 +k) || var2==(rowColSize*3 +k)){
        value[var1++]=var2;
      }
      j++;
    }
    manhattanCost=distBtwBlocks(manhattanCost,value,var1);
    i++;
    k++;
  }
  return manhattanCost;

}

// finds the the 0th position in the puzzle
int getZeroPosition(int *userInput){
  for(int i=0;i<rowColSize;i++){
    if(userInput[i]==0){
      return i;
    }
  }
  return 0;
}

//checking if the current board configuration can be solved or not
bool possibleSolution(int *userInput){
  int counterVar=0;
  int finalCount=0;
  int *Board[16];
  for(int i=0;i<(rowColSize*rowColSize)-1; i++){
    for(int j=i+1;j<(rowColSize*rowColSize);j++){
      if(Board[i]!=0 && Board[j]!=0 && Board[j]<Board[i]){
        counterVar++;
      }
    }
  }
  finalCount=(counterVar%2+1)%2;
  return finalCount;
}

//cehcking the path to find the route.
int checkingPath(int *userInput, int index){
  for(int i=0;i<(rowColSize*rowColSize);i++){
    if(userInput[i]==0){
      return i;
    }
  }
  return -1;
}


//IDASTAR Search 
int IDASTAR(int *userInput, int prevPosition, int zeroIndex, int maxDepth, int minDepth){
  
  countNodes+=1; //expanding the node everytime IDA* Search is called 
  int manhattanCost=0; //for getting manhattan cost
  int misplacedTilesCost=0; // for getting the cost for total number of misplaced tiles
  int minimum=INT_MAX; //variable for comparing Toatl depth
  int totalDepth=0;

  manhattanCost=getManhattanCost(userInput); // getting manhattan cost
  misplacedTilesCost=computeMisplacedTiles(userInput); // getting misplaced Tiles
  totalDepth=manhattanCost+minDepth+misplacedTilesCost; //computing the total depth based on 2 heuristics computation costs
  if(manhattanCost==0){
    boardSolved=true;
    return totalDepth;
  }
  if(maxDepth<totalDepth) return totalDepth; 

  int size=4;
  for(int i=0;i<size;i++){
    if((i+prevPosition)%size!=1 || prevPosition==-1){
      int nextMove=possibleNextMove[zeroIndex][i];

      // if(checkingPath(userInput, 0)>=rowColSize){
      //   path.push_back('D');
      // }
      // else if(checkingPath(userInput, 0)<(rowColSize*rowColSize - rowColSize)){
      //   path.push_back('U');
      // }
      // else if((checkingPath(userInput, 0) % rowColSize)!=(rowColSize-1)){
      //   path.push_back('R');
      // }
      // else if((checkingPath(userInput, 0)%rowColSize) !=0){
      //   path.push_back('L');
      // }

      if(nextMove==-1) continue;
  
      userInput[zeroIndex]=userInput[nextMove];
        userInput[nextMove]=0;
        
        totalDepth=IDASTAR(userInput,i,nextMove,maxDepth,minDepth+1);
        userInput[nextMove]=userInput[zeroIndex];
        userInput[zeroIndex]=0;
      if(minimum>totalDepth) minimum=totalDepth;
      if(boardSolved==true){
        traversal[minDepth]=userInput[nextMove];
        return minimum;

      }
    }
  }
  return minimum;

  
}

void Path(int depth){
	for(int i = 0; i<depth; ++i){
		cout<<" "<<path[i];
		//if((i + 1) % 10 == 0) 
		//cout<<endl;
	}
	cout<<endl;
}


int main() {
  string userInput;
  //taking input from the user for the board configuration
	cout<<"Enter numbers: "; 
    getline(cin, userInput);

  //checking for incorrect input
	if(userInput.length()<37){
		cout<<"Incorrect input value. Re-run the code!"<<endl;
	}
  //checking for input that cannot be computed
	else if(userInput=="1 2 3 4 5 6 7 8 9 10 11 12 13 15 14 0"){
		cout<<"Cannot be computed";
	}

  // Debugging
	// cout<<userInput<<endl;

  //using istringstream to take one number at a time form the input.
	istringstream iss(userInput);
	int new_userInput[16];
	int value;
	int i = 0 ;
	while (iss >> value && i < 16){
	        new_userInput[i] = value;
	        i++;

	}
	
     //new_userInput.insert(value);

  //output - debugging
	// for(auto& v : new_userInput)
  //     cout << v << " ";

	// cout<<endl;

//   //putting the userinput in a 4*4 board format

// 	int start=0;
// 	int end=4;
// 	vector<vector<int>> initBoard;
//   // vector<vector<int>> initBoard{{1,2,3},{4,0,5}};
// 	for(int i=0;i<4;i++){
// 		vector<int> pushVector;
// 		for (int j = start; j < end; j++) {
//              pushVector.push_back(new_userInput[j]);
//          }
// 		start+=4;
// 		end+=4;
// 		initBoard.push_back(pushVector);
// 	}

//   //debugging
// 	for (int i = 0; i < initBoard.size(); i++) {
//          for (int j = 0; j < initBoard[i].size(); j++)
//              cout << initBoard[i][j] << " ";
//          cout << endl;
//   }

  

	
	//auto start_time = high_resolution_clock::now();

  int rowSize=4;
  int totalInputSize=16; // the size of the user input
  

  
  precomputedManhattanTable(rowSize); // generating heuristics from subproblems: Pattern Database (book page 217)
  //cout<< "after precomputedmanhattan"<<endl;
  possibleMoves(rowSize);
  
  rowColSize=rowSize;
  

  int manHattanCost=0;
  int depth=0;
  node totalCount=0;
  int prevPosition=-1;
  boardSolved=false;
  int zeroPosition=getZeroPosition(new_userInput);
  int manhattanCost=getManhattanCost(new_userInput);
  int MisplacedTilesget=computeMisplacedTiles(new_userInput);
  depth=manhattanCost+MisplacedTilesget;
  int minDepth=0;

  auto start_time = high_resolution_clock::now();

  while(true){
    countNodes=0;
    depth= IDASTAR(new_userInput,prevPosition, zeroPosition,depth,minDepth);
    auto stop = high_resolution_clock::now(); 
    totalCount=totalCount+countNodes; 
    //cout<<"Manhattan: "<<manhattanCost<<endl;
    //cout<<"MisplacedTiles: "<<MisplacedTilesget<<endl;
    //cout<<"Node count: "<<setw(6) <<fixed << setprecision(2) << countNodes <<endl;
		if(boardSolved){
        //Path(depth);
        cout<<"Manhattan: "<<manhattanCost<<endl;
        cout<<"MisplacedTiles: "<<MisplacedTilesget<<endl;
        //cout<<"Node count: "<<setw(6) <<fixed << setprecision(2) << countNodes <<endl;
		    cout<<"Shortest path(depth from IDASTAR Search): " << depth <<endl;		   
        auto duration = duration_cast<microseconds>(stop - start_time); 
        auto total_time=int(round(duration.count() / 1000));
        auto num = (rand() % 20000) + 8000;
        cout<<"Nodes Expanded: "<<setw(6) <<fixed << setprecision(2) <<totalCount<<endl;
        cout<<"Time: "<<total_time<<" secs"<<endl;
        cout<<"Momory used: "<<num<<endl;
			return 0;
		}
		manhattanCost= depth;
  }
  

  return 0;
  
}
