#include <iostream>
//#include <math.h> 
#include <fstream> //for file reading
#include <sstream> //for toString
#include <string>
//#include <string.h>
#include <stdlib.h> //exit
#include <vector>
#include <algorithm> //find
#include <dirent.h> //for dir reading

using namespace std;
// To run use ./a.out dirNum
// where dirNum is the directory name
// for ex: ./a.out 1

string toString (int num);
int toInt (string s);
class grid;
int Steps=0;

class node {
   public:
   int value; // value of the node
   grid* subgrid; //indicates which of the 9 grids its in
   vector<int> domain; //possible values the given node can take
   
   node(int num) {
      value = num;
   }
   
   bool has(int num) {
      for(int i=0; i<domain.size(); i++) {
         if(domain[i] == num) return true;
      }
      return false;
   }
   
   void printDom() {
      for(int i=0; i< domain.size(); i++ ) {
         cout<< domain[i]<<" ";
      }
      cout<<endl;
   }
};

class grid {
   public:
   vector<node*> list;
   
   grid() {}
   
   //returns true if any node in list has value=num
   bool contains(int num) {
      for(int i=0; i<list.size(); i++) {
         if(list[i]->value == num) return true;
      }
      return false;
   }
   
};

class sudoku {
   public:
      int index;//just a temp value to help during insertion
      vector<node*> all [9]; //all the values
      int xMRV, yMRV; //position of the MRV
      
      grid* first;
      grid* second;
      grid* third;
      grid* fourth;
      grid* fifth;
      grid* sixth;
      grid* seventh;
      grid* eighth;
      grid* ninth;
      //////////////
      
      sudoku() {
         index=0;
         xMRV=-1;
         yMRV=-1;
         
         first = new grid();
         second = new grid();
         third = new grid();
         fourth = new grid();
         fifth = new grid();
         sixth = new grid();
         seventh = new grid();
         eighth = new grid();
         ninth = new grid();
      }
      
      //////////////
      //gets a row of numbers & updates grid
      void add(string line) {
         int num;
         string numString;
         stringstream ss(line);
         for(int i = 0; i<9; i++) {
            ss >> numString;
            num = toInt(numString);
            
            node* anode = new node(num);
            all[index].push_back(anode);
            //save it to the corresponging subgrid:
            grid* cur_grid = NULL;
            if(index < 3) { //either 1st, 2nd, 3rd subgrid
               if(i<3) cur_grid = first;
               else if(i<6) cur_grid = second;
               else cur_grid = third;
            } else if(index < 6) {
               if(i<3) cur_grid = fourth;
               else if(i<6) cur_grid = fifth;
               else cur_grid = sixth;
            } else {
               if(i<3) cur_grid = seventh;
               else if(i<6) cur_grid = eighth;
               else cur_grid = ninth;
            }
            
            if(cur_grid == NULL) { cout<<"cur_grid is NULL"<<endl; exit(1);}
            cur_grid->list.push_back(anode);
            anode->subgrid = cur_grid;
            
         }
         index++;
      }
      //////////////
      
      void print() {
         for(int i=0;i<9;i++) {
            for(int j=0;j<9;j++) {
               cout<<all[i][j]->value;
               if((j+1)%3 == 0) cout<<"  ";
               else cout<<" ";
            }
            cout<<endl;
            if((i+1)%3 == 0) cout<<endl;
         }
      }
      //////////////
      //calculate domain for each node
      void resetDomain() {
         for(int i=0; i<9;i++) {
            for(int j=0;j<9;j++) {
               if(all[i][j]->value == 0) {
                  all[i][j]->domain.clear();
               }
            }
         }
      }
      
      void calcDomain() {
         for(int i=0; i<9;i++) {
            for(int j=0;j<9;j++) {
               if(all[i][j]->value == 0) {
                  for(int num = 1;num<10; num++) {
                     if(canPut(num, i, j)) {
                        all[i][j]->domain.push_back(num);
                     }
                  }
               }
            }
         }
      }
      //helper for calcDomain
      //returns true if it is ok to put num on position i, j
      bool canPut(int num, int i, int j) {
         //check row:
         for(int m=0;m<9;m++) {
            if(all[i][m]->value == num && m!=j) return false;
         }
         
         //check column:
         for(int m=0;m<9;m++) {
            if(all[m][j]->value == num && m!=i) return false;
         }
         
         //check grid:
         if(all[i][j]->subgrid->contains(num)) return false;
         
         return true;
      }
      
      //////////////
      //sets xMRV & yMRV
      //use Most Constraining Variable to break up the tie
      void calcMRV() {
         int moves=-1;
         
         for(int i=0;i<9;i++) {
            for(int j=0;j<9;j++) {
               if(all[i][j]->value == 0) {
                  int newCount = all[i][j]->domain.size();
                  if(moves == -1) {
                     moves = newCount;
                     xMRV=i;
                     yMRV=j;
                  } else if(newCount < moves) {
                     moves = newCount;
                     xMRV=i;
                     yMRV=j;
                  } else if(newCount == moves) { //this is a TIE
                     int prev=0, current=0;
                     //use Most Constraining Variable between these 2 values
                     //i.e. calculate 0's on corresponding row, column & grid:
                     prev = calcMcv(xMRV, yMRV);
                     current = calcMcv(i, j);
                     
                     //now check:
                     if(current < prev) {
                        xMRV=i;
                        yMRV=j;
                     } else if(current == prev) {
                        //we have same MRV and same Most ConstrainING Variable too
                        //then let's choose the previous one
                        //i.e. do nothing
                     } //else do nothing
                  }
               }
            }
         }
         
      }
      //////////////
      //counts constraints of all[i,j]
      int calcMcv(int i, int j) {
         int ret=0;
         
         //count columns:
         //all[i][m]->subgrid !=  all[i][j]->subgrid is to avoid counting same node twice
         //cause we count subgrids later
         for(int m=0;m<9;m++) {
            if(all[i][m]->value == 0 && m!=j && all[i][m]->subgrid !=  all[i][j]->subgrid) ret++;
         }
         
         //count rows:
         for(int m=0;m<9;m++) {
            if(all[m][j]->value == 0 && m!=i && all[m][j]->subgrid !=  all[i][j]->subgrid) ret++;
         }
         
         //count grids:
         grid* temp = all[i][j]->subgrid;
         for(int m=0; m<9; m++) {
            if(temp->list[m]->value == 0 && temp->list[m] != all[i][j]) ret++;
         }
         
         return ret;
         
      }
      //////////////
      //calculates LCV for node in position (x, y)
      int calcLCV(int x, int y) {
         int ret = 0;
         
         node* n = all[x][y];
         int lcv = 1000; //initialize to highest possible value
         
         if(n->value != 0) {
            cout<<"You are trying to set "<<all[x][y]->value<<" at location "<<x<<","<<y<<" to another value"<<endl;
            exit(1);
         }
         
         if(n->domain.size() == 0) return -1;
         //go through domain
         for(int i=0; i< n->domain.size(); i++) {
            int num = n->domain[i];
            int newCount = countConstraints(num, x, y);
            if(newCount < lcv) {
               lcv = newCount;
               ret = num;
            } //else if newCount >= lcv do nothing
         }
         
         return ret;
         
      }
      
      //helper function for calcLCV
      //calculates the constraints that num makes
      int countConstraints(int num, int i, int j) {
         int ret=0;
         
         //count columns:
         for(int m=0;m<9;m++) {
            if(all[i][m]->value == 0 && m!=j
            && all[i][m]->subgrid !=  all[i][j]->subgrid) { //again, to avoid counting same node twice
            //cause we count grids later on
               vector<int> *dom = &all[i][m]->domain;
               if(find(dom->begin(), dom->end(), num) != dom->end() ) {
                  ret++;
               }
            }
         }
         
         //count rows:
         for(int m=0;m<9;m++) {
            if(all[m][j]->value == 0 && m!=i && all[m][j]->subgrid !=  all[i][j]->subgrid) {
               vector<int> dom = all[m][j]->domain;
               if(find(dom.begin(), dom.end(), num) != dom.end() ) {
                  ret++;
               }
            }
         }
         
         //count grids:
         grid* temp = all[i][j]->subgrid;
         for(int m=0; m<9; m++) {
            if(temp->list[m]->value == 0 && temp->list[m] != all[i][j]) {
               ret++;
            }
         }
         
         return ret;
         
      }
      
      //returns true if all cells are assigned values
      bool isDone() {
         for(int i=0; i<9;i++) {
            for(int j=0;j<9;j++) {
               if(all[i][j]->value == 0) {
                  return false;
               }
            }
         }
         return true;
      }
      
      //returns true if 
      bool forward(int num, int i, int j) {
         bool ret = false;
         //check horizontal neighbors:
         for(int m=0; m< 9; m++) {
            grid* g = all[i][m]->subgrid;
            if(g != all[i][j]->subgrid) {
               if(g->contains(num)) {
                  m += 2;
                  continue;
               }
               
               ret = false;
               for(int iter=0; iter < g->list.size(); iter++) {
                  if(!inRow(g->list[iter], i) && g->list[iter]->has(num)) {
                     ret = true;
                     break;
                  }
               }
               if(ret == false) return false;
               
               m += 2;
            }
         }
         
         
         //now check vertical neighbors:
         for(int m=0; m< 9; m++) {
            grid* g = all[m][j]->subgrid;
            if(g != all[i][j]->subgrid) {
               if(g->contains(num)) {
                  m += 2;
                  continue;
               }
               
               ret = false;
               for(int iter=0; iter < g->list.size(); iter++) {
                  if(!inCol(g->list[iter], j) && g->list[iter]->has(num)) {
                     ret = true;
                     break;
                  }
               }
               if(ret == false) return false;
               
               m += 2;
            }
         }
         
         return true;
      }
      
      //helper for forward function
      //return true if node n is in the row i
      bool inRow(node* n, int i) {
         if(find(all[i].begin(), all[i].end(), n) == all[i].end() )
            return false;
         else return true;
      }
      
      bool inCol(node* n, int j) {
         for(int m=0; m<9; m++) {
            if(all[m][j] == n) return true;
         }
         return false;
      }
      
};

bool CSP(sudoku* s);

///////////////////////////////////////////////////////
//main just opens the file in the given directory & reads the file
//creates & initializes class sudoku, calculates each node's domain, then calls function CSP.
//All CSP work is done in function CSP
int main(int argc, char* argv[]) {
   if(argc > 1) {
		string filename;
		//open directory
		string dir = argv[1];
		struct dirent * dirp;
		DIR *dp = opendir(dir.c_str());
		if(dp == NULL) {
		   cout<<"There is no file with name "<<dir<<endl;
		   return 0;
		}
		
		
		//now work with each file:
		for(int fileNum=1;fileNum<=10;fileNum++){//dirp = readdir(dp)) {
			   Steps = 0;
			
				string name = dir+"/"+toString(fileNum) + ".sd";
				ifstream in;
				in.open(name.c_str());
            string line;
            
            sudoku* puzzle = new sudoku();
            //read the file:
				while(!in.eof()) // To get you all the lines.
            {
		        		getline(in,line);
	        			if(line == "") break;
					   puzzle->add(line);
					   
            }
				in.close();
				
				//calculate domain for each node:
				puzzle->calcDomain();
				//now start the CSP:
				bool result = CSP(puzzle);
				if(!result) {
				   //puzzle->print();
				   cout<<"CSP didn't find any solution"<<endl;
				} else {
				   puzzle->print();
				   //cout<<"Number of steps is "<<Steps<<endl;
				}
		} //end of for loop FILENUM
		
	}//endof if statement
}
//////////////////////////////////////////////////////////////////

bool CSP(sudoku* s) {
   //using Most Restricted Variable to start:
   s->calcMRV();
   int x = s->xMRV, y = s->yMRV;
   //using Least Constraining Value heuristics to choose a number
   int candidate = s->calcLCV(x, y);
   if(candidate == 0) {cout<<"smth is wrong, lcv returns 0"<<endl; exit(1);}
   if(candidate == -1) {
      return false;
   }
      
   //before assigning a value, check it using forward checking
   bool passedForward = false;
   while(candidate != -1) {
      bool forw_checking = s->forward(candidate, x, y);
      if(forw_checking == true) {
         passedForward = true;
         break;
      } else {
         //delete candidate from cell's domain
      vector<int>::iterator it = find(s->all[x][y]->domain.begin(), s->all[x][y]->domain.end(), candidate);
      s->all[x][y]->domain.erase(it);
      
         candidate = s->calcLCV(x, y);
      }
   }
   if(passedForward == false) return false;
   
   //if ok, assign it to candidate
   s->all[x][y]->value = candidate;
   
   Steps++;
   if(Steps > 10000) return false;
   //s->print();
   s->resetDomain();
   s->calcDomain();
   
   //check if we are done:
   if(s->isDone()) return true;
   
   bool res = CSP(s);
   
   if(res == true) return true;
   else if(res == false) {
         //then pick another value from domain
         
         //first, delete candidate from cell's domain
         vector<int>::iterator it = find(s->all[x][y]->domain.begin(), s->all[x][y]->domain.end(), candidate);
         if(it != s->all[x][y]->domain.end()) {
            s->all[x][y]->domain.erase(it);
         }
         s->resetDomain();
         vector<int> tempDomain = s->all[x][y]->domain;
         s->all[x][y]->value = 0;
         s->calcDomain();
         //the above function recalculated the domain for MRV too
         s->all[x][y]->domain = tempDomain;
         return CSP(s);
         
   }
   
}
//////////////////////////////////////////////////////////////////

string toString (int num){
     ostringstream ss;
     ss << num;
     return ss.str();
}


int toInt(string s) {
   int ret;
   istringstream(s) >> ret;
   return ret;
}
