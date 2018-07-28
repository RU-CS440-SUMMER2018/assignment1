#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//char** _2DBOARD_;

//BOARD SIZE
int Boundc;
int Boundr;
    
void PrintBoard(char ** _BOARD_);

//Min Path of all possible path choices up down left right.
//int MinPath(int Startr, int Startc, int Endr, int Endc);

//CALL STRLEN on char* sequence to get path length, useful for minimizing complete paths.
int STRLEN(char* s);

//For setting up variables
int GCOUNTER = 0;
int GET(char* S,char* P);
//
char* GetNextValue(FILE* File);
//
void SetUpBoard(FILE* File,char ** _BOARD_);
//points to each exact path cost list
//LIST OF LISTS
//Queue of Paths
typedef struct Queue_of_Paths_{
    
    //Position Start
    int StartR;
    int StartC;
    
    //Position End
    int EndR;
    int EndC;
    
    //The Entire Path Sequence
    //Sequence of steps u,l,r,d, character array (May be as long as necessary)
    //u = column-1
    //d = column+1
    //l = row-1
    //r = row+1
    // use STRLEN(ListN) to get path length.
    char* ListN;
    
    //Another Path List (Potential)
    struct Queue_of_Paths_* Next;
    
}Queue_of_Paths;

//Priority Queue, because want to minimize paths of queue

//Inserts a Path into Queue
void Insert(Queue_of_Paths * Q);

//Delete an entry in Queue
void Delete(Queue_of_Paths * Q);

//Call strcmp of each complete path sequence in queue, if true, delete one, if not, continue for all path pointers in queue
//WILL READ PATH BY up,right,down,left.
bool CheckDuplicatePath(Queue_of_Paths * Q1, Queue_of_Paths *Q2);

//Min Path calls strlen on every path in queue (which leads to goal), Delete Paths that are not complete, and return Optimal Path Seq
Queue_of_Paths* Minimize(Queue_of_Paths* Q);

//Complete Heuristic 1, always return 0 (0 when reached goal), call this heuristic on a path, must expand each path, and then will determine if path is complete...
int CompleteHeuristic(int startr, int startc, int goalr, int goalc);

//Manhattan Heuristic 2, return estimate dist (Magnitude), used when, if found a complete path, get estimated magnitude.
int ManhattanHeuristic(int r1, int c1, int r2, int c2);

//STDIN_FILNO = -1;

int main(void) {

    FILE* S = fopen("file1.txt","r");
 
    if(S==NULL){
    printf("Open Error");
    return -1;
    } 
	
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    
    //Use h1, Complete Heuristic, and h2, Manhattan Heuristic, as the estimate
    
    //Use D, distance from start state, s, to end state, e
    
    //Use Wm Weight multiplier of heuristic (H1+H2)
    //int H = 0;
    
    //Weight must be set for density
    //double W;
    
    //Cost = dist(s,e) + h(e)
    //s is start state, e is given state, not necesarily Goal...
    //Will Minimize list of paths... Cost* = min(dist1(s,e1)+h(e1),......,distn(s,en)+h(en))
    
    //First, Second, and Third \n new line...  

    //CURRENT LOCATION
    //int wherec;
    //int wherer;
    //GOAL LOCATION
    //int goalc;
    //int goalr;
    
    //BoardInfo* START = malloc(sizeof(BoardInfo));

    //BoardInfo* GOAL = malloc(sizeof(BoardInfo));
    
    //////////////READ FILE INFO
  
    Boundr = atoi(GetNextValue(S));
    
    printf("Rows:%d",Boundr);
    
    Boundc = atoi(GetNextValue(S));
    
    printf("Columns:%d",Boundr); 
     //return 0;
    
    //Important if want to always find current player position and keep track of goal node in the space!!!!!!!
    char** _BOARD_=(char**)malloc(Boundr*sizeof(char*));
    int i=0;
    while(i<Boundr){
   _BOARD_[i]=(char*)malloc(Boundc*sizeof(char));
    i++;
    }
    
    //Malloc the space once given the coordinate
    //return 0;
    SetUpBoard(S,_BOARD_);
   
    //return 0; 
    PrintBoard(_BOARD_);
    
    //FIND OUT WHERE GOAL IS
    
    /*
    goalr = atoi(GetNextValue(STDIN_FILENO));
    
    printf("FOOD ROWS:%d",goalr);    
    
    goalc = atoi(GetNextValue(STDIN_FILENO));
    
    printf("FOOD COLUMNS:%d",goalc);
    */
    
    //FIND OUT WHERE YOU START
    /*wherer = atoi(GetNextValue(STDIN_FILENO));
    
    printf("FOOD ROWS:%d",wherer);    
    
    wherec = atoi(GetNextValue(STDIN_FILENO));
    
    printf("FOOD COLUMNS:%d",wherec);
    */
    //
    
    return 0;
}

int GET(char* F, char* H){
    
    if(F[GCOUNTER]!='\0'){
    
    H[0] = F[GCOUNTER];
        
    GCOUNTER=GCOUNTER+1;
    
        return 1;
        
    }
    else{
        
        return 0;
        
    }
}

char* GetNextValue(FILE* File){

int pos = 0;    
    
char op = ' ';  
  
char* cpy = (char*)malloc(2*sizeof(char));    
bzero(cpy,2);

cpy[0]='1';  
 
//int SIZE = fseek(File,0,SEEK_END);
 
while(fscanf(File,"%c",&op)!=EOF){
    //printf("%c",op); 
    
    if(op=='\n'){
        //Found space to next information
        printf("\nExit Read \n");
        
	//fscanf(File,"%c",&op);
        
	break;   
    }
    
    else{
        
    cpy[pos]=op;
    
    cpy = (char*)realloc(cpy,(pos+2)*sizeof(char));    
         
    if (cpy==NULL){
        
        printf("MEM ERROR, EXITING");
        
        exit(0);
    }
    else{
            
        cpy[pos+1] = '\0';    
   
    }
        
    pos++;
 
    }
    
}   
    
    
    return cpy;
}

int STRLEN(char* s){
int Length = 0;    
    
    char t = *s;
    
    while(t!='\0'){
        Length=Length+1;
        t = (*s+Length);
    }
    
    
return Length;
}

void SetUpBoard(FILE* File, char ** _2DBOARD_){
    
//int pos = 0;    

int row = 0;
    
int column = 0;
    
char op = ' '; 
//char* cpy = malloc(sizeof(char));    
//bzero(cpy,1);
    
//TEST
/*
while(read(File,op,1)!=0){
    
    if(*op=='\n'){
        break;
    }
    
}  
 
printf("\n"); 
//_2DBOARD_[0][Boundc-1]='%';
*/
    
while(fscanf(File,"%c",&op)!=EOF){
    //printf("%c",*op);     
    if(op=='\n'){
        //Found next row
        if(row>Boundr)
	return;
	row = row+1;
        column = 0;
        printf("\n");
    }
    
    else{
     if(column<Boundc&&row<Boundr){
     _2DBOARD_[row][column]=op;
     printf("%c",_2DBOARD_[row][column]);
     }  
     column = column+1;
    }
    
}   
    
    return;
}

void PrintBoard(char ** _2DBOARD_){
    int row = 0;
    int column = 0;
    
    printf("\n 2D BOARD \n");
    
    while(row<Boundr){
        column = 0;
        while(column<Boundc){
            printf("%c ",_2DBOARD_[row][column]);
            column = column+1;
        }
            row = row+1;
            printf("\n");
    }
    
    
    return;
}
