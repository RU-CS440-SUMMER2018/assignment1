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

void ResetBoard(char ** _BOARD_);

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

void DeleteBoard(char** _BOARD);

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
    //u = row - 1
    //d = row+1
    //l = column - 1
    //r = column + 1
    // use STRLEN(ListN) to get path length.
    char* ListN;
    
    //Another Path List (Potential)
    struct Queue_of_Paths_* Next;
    
}Queue_of_Paths;

//Priority Queue, because want to minimize paths of queue

//Inserts a Path into Queue, All Paths should be COMPLETE and UNIQUE.
//INCLUDE ALL ENTRIES FULFILLED for Q(n)
void Insert(Queue_of_Paths * Q);

//Delete an entry in Queue, MAKE SURE, WHEN FOUND nth Optimal Path, Save that queue node and prepare for the next Challenge
void Delete(Queue_of_Paths * Q);

void PrintQueue();

void Free();

//Call strcmp of each complete path sequence in queue, if true, delete one, if not, continue for all path pointers in queue
//WILL READ PATH BY U,D,R,L.... From position path started
//RETURN THE POINTER TO DUPLICATE NODE, NULL IF NONE, CALL Delete(CheckDuplicatePath(Queue_of_Paths * Q1, Queue_of_Paths *Q2));
Queue_of_Paths* CheckDuplicatePath(Queue_of_Paths * Q1);

//Min Path calls strlen on every path in queue (which leads to goal), Delete Paths that are not complete, and return Optimal Path Seq
Queue_of_Paths* Minimize(Queue_of_Paths* Q);

//Complete Heuristic 1, always return 0 (0 when reached goal), call this heuristic on a path, must expand each path, and then will determine if path is complete...
int CompleteHeuristic(int startr, int startc, int goalr, int goalc,char** A);

//Manhattan Heuristic 2, return estimate dist (Magnitude), used when, if found a complete path, get estimated magnitude.
int ManhattanHeuristic(int r1, int c1, int r2, int c2);

//Root contains blank char... 
//Else contains path seq char
typedef struct TraceTree_{

char ActFromPrev;

struct TraceTree_* NextUp;
struct TraceTree_* NextRight;
struct TraceTree_* NextDown;
struct TraceTree_* NextLeft;

}TraceTree;

//Once Tree is done, all have to do is search tree preorder
//Create State Tree
void PopulateTree(int Starts, int Startc, char** ENV);
//Insert chars, but only the next node of tree... Check if valid move as well
void InsertTree(char C,char* V);
//Delete Starting at specified node
void DeleteTree(TraceTree* TT,int Level);
//Print Tree PReorder Up Right Down Left... Important, because these are all adjecent actions to eachother!!!
void PrintTree(TraceTree* TT, int Level);

//Trace, Main Function used to trace path(s) to goal, Complete, but needs Hcomplete in order to act on the best action first instead of trial and error
char* Trace(char** ENV,int Startr,int Startc, int Endr, int Endc,char* App,int Size);

//Used to determine where path started given path sequence.
int* TraceBack(char* App, int Startr, int Startc, char** ENV);

//First Node of Queue
Queue_of_Paths* Q = NULL;
//First Node of 4-tree
TraceTree* T = NULL;

int main(void) {

    //open file2.txt read only
    FILE* S = fopen("file2.txt","r");
    //If error opening
    if(S==NULL){
    printf("Open Error");
    return -1;
    } 
    //Trace Tree 
    T = (TraceTree*)malloc(sizeof(TraceTree));
	
/////////////////////////////////////// MEM TEST    
Queue_of_Paths* Q1 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));
    if(Q1==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    //Q->Next=Q1;
    char* s = (char*)malloc(5*sizeof(char));
    if(s==NULL){
     
    printf("\nMEM ERROR\n");
    exit(-1);   
    }
    s[0]='u';
    s[1]='u';
    s[2]='l';
    s[3]='r';
    s[4]='\0';


    //return 0;    
    Q1->ListN=s;
    
    Insert(Q1);

Queue_of_Paths* Q2 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));
    if(Q2==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    //Q1->Next=Q2;
    char* s1 = (char*)malloc(5*sizeof(char));
    if(s1==NULL){
     
    printf("\nMEM ERROR\n");
    exit(-1);   
    }
    s1[0]='l';
    s1[1]='l';
    s1[2]='u';
    s1[3]='l';
    s1[4]='\0';
 
    Q2->ListN = s1;  

    Insert(Q2);

Queue_of_Paths* Q3 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));
    if(Q3==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    //Q2->Next=Q3;
    char* s3 = (char*)malloc(5*sizeof(char));
    if(s3==NULL){
     
    printf("\nMEM ERROR\n");
    exit(-1);   
    }
    s3[0]='u';
    s3[1]='u';
    s3[2]='u';
    s3[3]='u';
    s3[4]='\0';

    Q3->ListN = s3;
    Insert(Q3);
Queue_of_Paths* Q4 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));
    if(Q4==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    //Q3->Next=Q4;
    char* s2 = (char*)malloc(5*sizeof(char));
    if(s2==NULL){
     
    printf("\nMEM ERROR\n");
    exit(-1);   
    }
    s2[0]='u';
    s2[1]='l';
    s2[2]='d';
    s2[3]='r';
    s2[4]='\0';

    Q4->ListN = s2;

    Insert(Q4);
Queue_of_Paths* Q5 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));
    //Q4->Next=Q5;
    if(Q5==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    char* s4 = (char*)malloc(5*sizeof(char));
    if(s4==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    s4[0]='d';
    s4[1]='d';
    s4[2]='d';
    s4[3]='d';
    s4[4]='\0';

    Q5->ListN = s4;

    Insert(Q5);
    
    PrintQueue();
 
    //Delete(Q1);

    //PrintQueue();

    Delete(Q5);

    Delete(Q2);

    Delete(Q3);

    Delete(Q4);

    //Delete(Q);

    //Delete(Q);
// END MEM TEST////////////////////////////////////////////


    //4- WAY TREE TEST
    char * You = (char*)malloc(2*sizeof(char));
    if(You==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    You[0]='u';
    You[1]='\0';
    InsertTree('u',NULL);

    InsertTree('u',You);
    //4- WAY TREE TEST
    
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
 
    char* c = GetNextValue(S);
    Boundr = atoi(c);
    free(c);
    c=NULL; 
    printf("Rows:%d",Boundr);
    
    char* d = GetNextValue(S);
    Boundc = atoi(d);
    free(d);
    d = NULL;
    printf("Columns:%d\n\n",Boundr); 

    //CHECK VALUES FOR BOARD     
    if(Boundc<=0||Boundr<=0){

    printf("\nError, BOUNDC or BOUNDR Data Lost");

    return -1;

    }
    printf("\nBOUNDC:%d\nBOUNDR:%d\n",Boundc,Boundr);
    ////////////////////////////

    //Important if want to always find current player position and keep track of goal node in the space!!!!!!!
    char** _BOARD_=(char**)malloc(Boundr*sizeof(char*));
    if(_BOARD_==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    int i=0;
    while(i<Boundr){
   _BOARD_[i]=(char*)malloc(Boundc*sizeof(char));
   if(_BOARD_[i]==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
    i++;
    }
    
    //Malloc the space once given the coordinate
    SetUpBoard(S,_BOARD_);
   
    PrintBoard(_BOARD_);
  

    //36-1,36-1,36-1,2-1
    int I = CompleteHeuristic(35, 35, 35, 1,_BOARD_);


    //printf("\nCOMPLETE PATH FOUND: %d \n",I);

    PrintTree(T,0);

    PrintQueue();
 
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
    
    //Delete ENV
    DeleteBoard(_BOARD_);   
    //Free All Queue
    Free();
    //Free All TraceTree
    DeleteTree(T,0);
    //ONLY FREE ROOT AT END WHEN FINISHED USING TRACE ALG
    free(T);
    T=NULL; 
    //Close File 
    fclose(S);
   
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
if(cpy==NULL){
    printf("\nMEM ERROR\n");
    exit(-1);
    }
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
        
        exit(-1);
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
  
/////////////////IMPORTANT!!! P AND . ARE EXCLUSIVE CHARS!! USED TO INDICATE THE STARTING POSITION P AND GOAL POSITION . !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

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
    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////READ INCLUDING EXCLUSIVE CHARS P and .///////////////////////////////////////////////////////////////////
    else if(op=='0'||op=='1'||op=='P'||op=='.'){
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

void ResetBoard(char ** _2DBOARD_){
int row = 0;
    int column = 0;

    printf("\n RESET 2D BOARD TO UNVISITED \n");

    while(row<Boundr){
        column = 0;
        while(column<Boundc){
            //printf("%c ",_2DBOARD_[row][column]);
            if(_2DBOARD_[row][column]=='X'){
	    _2DBOARD_[row][column]='1';
 	    }
	    column = column+1;
        }
            row = row+1;
            //printf("\n");
    }

 PrintBoard(_2DBOARD_);
return;
}
void DeleteBoard(char** _2DBOARD_){
    
    int i=0;
    
    while(i<Boundr){
    //_BOARD_[i]=(char*)malloc(Boundc*sizeof(char));
    free((char*)_2DBOARD_[i]);
    _2DBOARD_[i]=NULL;
    i++;
    }
    
    free((char**)_2DBOARD_);

    return;
}

void  Insert(Queue_of_Paths * Q1){

if(Q==NULL&&Q1!=NULL){
/*Q = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));
Q->StartR = Q1->StartR;
Q->StartC = Q1->StartC;
Q->EndR = Q1->EndR;
Q->EndC = Q1->EndC;
Q->ListN = Q1->ListN;
*/
Q=Q1;
Q->Next=NULL;
printf("\nFIRST ENTRY\n");
return;
}

if(Q1==NULL){
printf("\nNULL ENTRY\n");
return;
}

Queue_of_Paths*Q2 =Q;//(Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

//Q2 = Q;

while(Q2!=NULL){

if(Q2->Next==NULL){
printf("\nAPPEND ENTRY:[%d,%d] ,[%d,%d] , %s\n",Q1->StartR,Q1->StartC,Q1->EndR,Q1->EndC,Q1->ListN);
/*
Q2->Next=(Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));
Q2->Next->StartR = Q1->StartR;
Q2->Next->StartC = Q1->StartC;
Q2->Next->EndR = Q1->EndR;
Q2->Next->EndC = Q1->EndC;
Q2->Next->ListN = Q1->ListN;
*/
Q2->Next=Q1;
Q2->Next->Next=NULL;
return;
}


Queue_of_Paths* Q3 = Q2->Next;

Q2 = Q3;
}

return;

}

Queue_of_Paths* CheckDuplicatePath(Queue_of_Paths * Q1){
if(Q1==NULL){
return NULL;
}
Queue_of_Paths * Q2 = Q;

if(Q1->ListN!=NULL&&Q2->ListN!=NULL){

if(strcmp(Q1->ListN,Q2->ListN)==0&&Q1->StartR==Q2->StartR&&Q1->StartC==Q2->StartC&&Q1->EndR==Q2->EndR&&Q1->EndC==Q2->EndC){

if(Q1->ListN!=NULL)
free(Q1->ListN);
Q1->ListN=NULL;

free(Q1);
Q1=NULL;

PrintQueue();

return Q2;

}
}else{
printf("\nPath SEQS are NULLLLL\n");
return NULL;
}

while(Q2!=NULL){

if(Q2->Next!=NULL){


if(Q1->ListN!=NULL&&Q2->ListN!=NULL){

if(strcmp(Q1->ListN,Q2->Next->ListN)==0&&Q1->StartR==Q2->Next->StartR&&Q1->StartC==Q2->Next->StartC&&Q1->EndR==Q2->Next->EndR&&Q1->EndC==Q2->Next->EndC){
printf("\nFOUND SAME PATH SEQS(((INCLUDING FROM SAME POSITION)))\n");

//Return pointer within the queue, but only the parent of the pointer!

if(Q1->ListN!=NULL)
free(Q1->ListN);
Q1->ListN=NULL;

free(Q1);
Q1=NULL;

PrintQueue();

return Q2;

}
}
}
else{

printf("\nPATH SEQS ARE NULLLL, CONTINUEEEEE\n");

if(Q1->ListN==NULL&&Q2->ListN==NULL&&Q1->StartR==Q2->StartR&&Q1->StartC==Q2->StartC&&Q1->EndR==Q2->EndR&&Q1->EndC==Q2->EndC){

printf("\nDELETING DUPLICATE...\n");

if(Q1->ListN!=NULL)
free(Q1->ListN);
Q1->ListN=NULL;

free(Q1);
Q1=NULL;

PrintQueue();

//return Q2; 
}

//printf("\nPATH SEQS ARE NULLLL, CONTINUEEEEE\n");

}
Queue_of_Paths* Q3 = Q2->Next;
Q2 = Q3;
}

return NULL;

}

void PrintQueue(){
if(Q==NULL){
printf("\n-----START-QUEUE-----");
printf("\nNULL\n");
printf("-----END-QUEUE-----\n");
return;
}

Queue_of_Paths* Q1 = Q;

int i=0;

printf("\n-----START-QUEUE-----");

while(Q1!=NULL){
printf("\n%d:",i);
printf("Start Row:%d\nStart Column:%d\n",Q1->StartR,Q1->StartC);
printf("Goal Row:%d\nGoal Column:%d\n",Q1->EndR,Q1->EndC);
printf("SEQ:%s (SIZE:%d) \n",Q1->ListN,(int)strlen(Q1->ListN));

Queue_of_Paths* Q2 = Q1->Next;

Q1 = Q2;
i++;
}
if(Q1==NULL){
//printf("\n-----START-QUEUE-----");
printf("\nNULL\n");
//printf("-----END-QUEUE-----\n");
//return;
}


printf("-----END-QUEUE-----\n");


return;
}


void Free(){
if(Q==NULL){
return;
}
Queue_of_Paths* Q1;
// = Q->Next;
while(Q!=NULL){
Q1=Q->Next;

if(Q->ListN!=NULL){
free(Q->ListN);
Q->ListN=NULL;
}

free(Q);
Q=NULL;
Q=Q1;

}
return;
}

int CompleteHeuristic(int startr, int startc, int goalr, int goalc,char** _2DARRAY_){

if(startr==goalr&&startc==goalc){
Queue_of_Paths* Q1 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

Q1->StartR = startr;
Q1->StartC = startc;
Q1->EndR = goalr;
Q1->EndC = goalc;

//Don't Have to Move
Q1->ListN = (char*)malloc(2*sizeof(char));
bzero((Q1->ListN),2);
(Q1->ListN)[0]='\0';

Insert(Q1);
return 1;
}

//EX: Start 35,35
//END 35,1

//int Row = startr;
//int Column = startc;

//bool GFOUND = false;
//Have ENV

//Queue_of_Paths * Q2 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

//Order: U,R,D,L
//U = row-1
//D = row+1
//L = Column -1
//R = Column +1

//Worst Case 4^N Lengths Strings!!!
//char* CurrPath = malloc

int ChangeR;
int ChangeC;

//while(!GFOUND){
//First, Trace A Path
//TRY ALL TRACES
//TRACE FUNCTIION WILL EXHAUST ALL PATH POSSIBILITIES, RECURSIVELY, GIVEN A STARTING DIRECTION, END coord, and ENV

ChangeR=startr-1;
char* App = (char*)malloc(2*sizeof(char));
if(App==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App,2);
App[0]='u';
//COORDINATES START 0 - n-1!!!!!!!!!!
char* U = Trace(_2DARRAY_,ChangeR,startc,goalr,goalc,App,2);

ResetBoard(_2DARRAY_);

ChangeC=startc+1;

//return 0;

free(App);
App=NULL;
App = (char*)malloc(2*sizeof(char));
if(App==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App,2);
App[0]='r';

//COORDINATES START 0 - n-1!!!!!!!!!!
char* R = Trace(_2DARRAY_,startr,ChangeC,goalr,goalc,App,2);

ResetBoard(_2DARRAY_);

ChangeR=startr+1;

free(App);
App=NULL;
App = (char*)malloc(2*sizeof(char));
if(App==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App,2);
App[0]='d';

//COORDINATES START 0 - n-1!!!!!!!!!!
char* D = Trace(_2DARRAY_,ChangeR,startc,goalr,goalc,App,2);

ResetBoard(_2DARRAY_);

ChangeC=startc-1;

free(App);
App=NULL;
App = (char*)malloc(2*sizeof(char));
if(App==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App,2);
App[0]='l';
//COORDINATES START 0 - n-1!!!!!!!!!!
char* L = Trace(_2DARRAY_,startr,ChangeC,goalr,goalc,App,2);

//L = Trace(_2DARRAY_,startr,ChangeC,goalr,goalc,App);
free(App);
App=NULL;

printf("\nUP PATH:%s\nRIGHT PATH:%s\n\nDOWN PATH:%s\nLEFT PATH:%s\n",U==NULL? "NULL":U,R==NULL? "NULL":R,D==NULL? "NULL":D,L==NULL? "NULL":L);

PrintQueue();

/*
if(U==NULL&&R==NULL&&D==NULL&&L==NULL){
//NO PATH FOUND!
return 0;
}
*/
//RETURN NULL if NO PATH..
//IF ALL = NULL, NO PATH POSSIBLE!
//ELSE, FOUND [a1,,,,an] Path

//Then, Scan for Duplicate Path..

/*Queue_of_Paths* Q1 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

Q1->StartR = startr;
Q1->StartC = startc;
Q1->EndR = goalr;
Q1->EndC = goalc;
//FIND MIN OF THESE PATHS...
//Q1->ListN = Min {PATHS}
//
//IF Not Duplicate Path
if(CheckDuplicatePath(Q1)==NULL){
//Then, Insert Path into Queue
Insert(Q1);
//Then return 1 IFF found goal (Somehow)
}
*/
//IF EXHAUSTED ALL PATH OPTIONS, exit while and quit
//}

ResetBoard(_2DARRAY_);


//If Not a Path to Goal
return 1;
}

void Delete(Queue_of_Paths * Q1){

if(Q==NULL){
return;
}
if(Q1==NULL){
return;
}

printf("\n\nDELETING SEQ:%s\n\n",Q1->ListN==NULL ? "NULL":Q1->ListN);

Queue_of_Paths*Q2 = Q;

//Delete First Node
if(Q1->ListN!=NULL&&Q2->ListN!=NULL){
if(strcmp(Q1->ListN,Q2->ListN)==0&&Q1->StartR==Q2->StartR&&Q1->StartC==Q2->StartC&&Q1->EndR==Q2->EndR&&Q1->EndC==Q2->EndC){

printf("\nFOUND SAME PATH SEQS [FIRST DELETE] (((INCLUDING FROM SAME POSITION)))\n");

Queue_of_Paths* Q4 = Q2->Next;

if(Q2->ListN!=NULL)
free(Q2->ListN);
Q2->ListN=NULL;

free(Q2);
Q2=NULL;

Q = Q4;

PrintQueue();

return;
}
}
else{
if(Q1->ListN==NULL&&Q2->ListN==NULL&&Q1->StartR==Q2->StartR&&Q1->StartC==Q2->StartC&&Q1->EndR==Q2->EndR&&Q1->EndC==Q2->EndC){

printf("\nPATH SEQ ARE NULLLLL\n");

Queue_of_Paths* Q4 = Q2->Next;

free(Q2);
Q2=NULL;

Q = Q4;

PrintQueue();

return;
}
}

while(Q2!=NULL){

if(Q2->Next!=NULL){
if(Q2->Next->ListN!=NULL&&Q1->ListN!=NULL){
if(strcmp(Q1->ListN,Q2->Next->ListN)==0&&Q1->StartR==Q2->Next->StartR&&Q1->StartC==Q2->Next->StartC&&Q1->EndR==Q2->Next->EndR&&Q1->EndC==Q2->Next->EndC){
printf("\nFOUND SAME PATH SEQS [DELETE] (((INCLUDING FROM SAME POSITION)))\n");
//
Queue_of_Paths* Q4 = Q2->Next->Next;


if(Q2->Next->ListN!=NULL)
free(Q2->Next->ListN);
Q2->Next->ListN=NULL;

free(Q2->Next);
Q2->Next=NULL;

Q2->Next = Q4;
//

PrintQueue();

return;
//
}
}
else{

if(Q2->Next->ListN==NULL&&Q1->ListN==NULL&&Q1->StartR==Q2->Next->StartR&&Q1->StartC==Q2->Next->StartC&&Q1->EndR==Q2->Next->EndR&&Q1->EndC==Q2->Next->EndC){
printf("\nPATH SEQ ARE NULLLLL, CONTINUEEEEE\n");
//return;

Queue_of_Paths* Q4 = Q2->Next->Next;

free(Q2->Next);
Q2->Next=NULL;

Q2->Next = Q4;

PrintQueue();

}
}
}
//
Queue_of_Paths* Q3 = Q2->Next;
Q2 = Q3;
}

//
//return (Queue_of_Paths*)NULL;
return;
}


//Depth Branching... U,R,D,L
//Order: U,R,D,L
////U = row-1
////D = row+1
////L = Column -1
////R = Column +1
char* Trace(char** ENV,int Startr,int Startc, int Endr, int Endc,char* App,int Size){
//Given Start Coord and End coord....
//START at ENV

if(Size==2){
//Start Size

ENV[Startr][Startc]='X';

}

PrintBoard(ENV);

printf("\n COORD START:%d,%d, Char:%c,Char interted:%c",Startr,Startc,ENV[Startr][Startc],ENV[Startc][Startr]);
printf("\n COORD END:%d,%d, Char:%c Char inverted:%c",Endr,Endc,ENV[Endr][Endc],ENV[Endc][Endr]);

if(ENV==NULL||(Startr<0||Startc<0||Endr<0||Endc<0||Startr>Boundr-1||Startc>Boundc-1||Endr>Boundr-1||Endc>Boundc-1)||(ENV[Endr][Endc]!='.')){
printf("\nInput Trace Error\n");
return NULL;
}

if(Startr==Endr&&Startc==Endc){
//Complete Path Found

return App;

}

//Where You Start?
//Start by ORDER U,R,D,L, Recursive!
//Check Up
if(Startr>0){
if(ENV[Startr-1][Startc]=='1'){
//May Proceed Here
if(ENV[Startr-1][Startc]!='X'){
//MARK AS VISITED!
ENV[Startr-1][Startc]='X';
////
Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"u");
/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'u';
*/
printf("\nAPPENDED PATH:%s",App2);

Trace(ENV,Startr-1, Startc, Endr, Endc,App2,Size);

}
else{
printf("\nALREADY VISITED [%d,%d]\n",Startr-1,Startc);
}
}
else{
//Try Another Way...
///////////////////////////////////////////////////////////FOUND GOAL
if(ENV[Startr-1][Startc]=='.'){
//Sequence of Steps
Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
//App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"u");

/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'u';
*/
printf("\nAPPENDED PATH:%s",App2);

char* App3 = (char*)malloc(Size*sizeof(char));
if(App3==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App3,Size);
strncpy(App3,App2,Size);
//Trace(ENV,Startr-1, Startc, Endr, Endc,App);


Queue_of_Paths* Q1 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

int* V = TraceBack(App3, Startr, Startc, ENV);

printf("\n ORIG START : [%d,%d]\n",V[0],V[1]);
Q1->StartR = V[0];
Q1->StartC = V[1];
free(V);
V=NULL;
Q1->EndR = Endr;
Q1->EndC = Endc;
Q1->ListN = App3;
//FIND MIN OF THESE PATHS...
///Q1->ListN = Min {PATHS}
////
////IF Not Duplicate Path
if(CheckDuplicatePath(Q1)==NULL){
////Then, Insert Path into Queue
Insert(Q1);

}
else{
//free(Q1->ListN);
free(Q1);
Q1=NULL;
}

PrintQueue();

//exit(0);

return App3;
//
}
///////////////////////////////////////////////////////////////GOOOOOOOOOOOOOOOAAAAAOL
//
////CANT GO UP ANY MORE
printf("\nCANT GO UP ANY MORE(DEAD END)");
}
//
}
else{
//Try Another Way...
//CANT GO UP ANY MORE
printf("\nCANT GO UP ANY MORE");

}
//Check Right
if(Startc<Boundc-1){
if(ENV[Startr][Startc+1]=='1'){


if(ENV[Startr][Startc+1]!='X'){
//May Proceed Here
ENV[Startr][Startc+1]='X';

Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"r");

/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'r';
*/
printf("\nAPPENDED PATH:%s",App2);

Trace(ENV,Startr, Startc+1, Endr, Endc,App2,Size);
//
}
else{
printf("\nALREADY VISITED [%d,%d]\n",Startr,Startc+1);
}


}
else{
//Try Another Way...
///////////////////////////////////////////////////////////FOUND GOAL
if(ENV[Startr][Startc+1]=='.'){
//Sequence of Steps
Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"r");

/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'r';
*/
printf("\nAPPENDED PATH:%s",App2);

char* App3 = (char*)malloc(Size*sizeof(char));
if(App3==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App3,Size);
strncpy(App3,App2,Size);

Queue_of_Paths* Q1 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

int* V = TraceBack(App3, Startr, Startc, ENV);

printf("\n ORIG START : [%d,%d]\n",V[0],V[1]);
Q1->StartR = V[0];
Q1->StartC = V[1];
free(V);
V=NULL;
Q1->EndR = Endr;
Q1->EndC = Endc;
Q1->ListN = App3;
//FIND MIN OF THESE PATHS...
/////Q1->ListN = Min {PATHS}
//////
//////IF Not Duplicate Path
if(CheckDuplicatePath(Q1)==NULL){
//////Then, Insert Path into Queue
Insert(Q1);
//
}
else{
//free(Q1->ListN);
free(Q1);
Q1=NULL;
}
//
//

PrintQueue();

//exit(0);

return App3;
//
}
///////////////////////////////////////////////////////////////GOOOOOOOOOOOOOOOAAAAAOL
//
////CANT GO RIGHT ANY MORE
//
printf("\nCANT GO RIGHT ANY MORE (DEAD END)");


//
}
//
}
else{
//Try Another Way...
//CANT GO RIGHT ANY MORE

printf("\nCANT GO RIGHT ANY MORE");

}
//Check Down
if(Startr<Boundr-1){
if(ENV[Startr+1][Startc]=='1'){
//
if(ENV[Startr+1][Startc]!='X'){
//May Procees Here
ENV[Startr+1][Startc]='X';

Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"d");
/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'd';
*/
printf("\nAPPENDED PATH:%s",App2);

Trace(ENV,Startr+1, Startc, Endr, Endc,App2,Size);
}
else{
printf("\nALREADY VISITED [%d,%d]\n",Startr+1,Startc);
}
//
}
else{
//Try Another Way...
///////////////////////////////////////////////////////////FOUND GOAL
if(ENV[Startr+1][Startc]=='.'){
//Sequence of Steps
Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"d");
/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'd';
*/
printf("\nAPPENDED PATH:%s",App2);

char* App3 = (char*)malloc(Size*sizeof(char));
if(App3==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App3,Size);
strncpy(App3,App2,Size);

Queue_of_Paths* Q1 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

int* V = TraceBack(App3, Startr, Startc, ENV);

printf("\n ORIG START : [%d,%d]\n",V[0],V[1]);
Q1->StartR = V[0];
Q1->StartC = V[1];
free(V);
V=NULL;
Q1->EndR = Endr;
Q1->EndC = Endc;
Q1->ListN = App3;
//FIND MIN OF THESE PATHS...
///////Q1->ListN = Min {PATHS}
////////
////////IF Not Duplicate Path
if(CheckDuplicatePath(Q1)==NULL){
////////Then, Insert Path into Queue
Insert(Q1);
////
}
else{
//free(Q1->ListN);
free(Q1);
Q1=NULL;
}
////


PrintQueue();

//exit(0);


return App3;
//
}
///////////////////////////////////////////////////////////////GOOOOOOOOOOOOOOOAAAAAOL
//
//////CANT GO DOWN ANY MORE
//
printf("\nCANT GO DOWN ANY MORE (DEAD END)");
//
//
}
}
else{

//Try Another Way...
////CANT GO DOWN ANY MORE
printf("\nCANT GO DOWN ANY MORE");

}
//Check Left
if(Startc>0){
if(ENV[Startr][Startc-1]=='1'){

if(ENV[Startr][Startc-1]!='X'){
//MARK AS VISITED!
ENV[Startr][Startc-1]='X';
//MARKED VISIT!!!

//PrintBoard(ENV);

//return NULL;

//May Procede Here
Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"l");
/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'l';
*/
printf("\nAPPENDED PATH:%s",App2);

Trace(ENV,Startr, Startc-1, Endr, Endc,App2,Size);

}
else{
printf("\nALREADY VISITED [%d,%d]\n",Startr,Startc-1);
}

}
else{
///////////////////////////////////////////////////////////FOUND GOAL
if(ENV[Startr][Startc-1]=='.'){
//Sequence of Steps
Size = Size+1;

char App2[Size];
bzero(App2,Size);
strncpy(App2,App,Size);
App2[Size-1]='\0';
//App2[Size-2]='u';
strcat(App2,"l");
/*
App = (char*)realloc(App,((Size)*sizeof(char)));
App[Size-1] = '\0';
App[Size-2] = 'l';
*/
printf("\nAPPENDED PATH:%s",App2);

char* App3 = (char*)malloc(Size*sizeof(char));
if(App3==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
bzero(App3,Size);
strncpy(App3,App2,Size);

Queue_of_Paths* Q1 = (Queue_of_Paths*)malloc(sizeof(Queue_of_Paths));

int* V = TraceBack(App3, Startr, Startc,ENV);

printf("\n ORIG START : [%d,%d]\n",V[0],V[1]);
Q1->StartR = V[0];
Q1->StartC = V[1];
free(V);
V=NULL;
Q1->EndR = Endr;
Q1->EndC = Endc;
Q1->ListN = App3;
//FIND MIN OF THESE PATHS...
///////Q1->ListN = Min {PATHS}
////////
////////IF Not Duplicate Path
if(CheckDuplicatePath(Q1)==NULL){
////////Then, Insert Path into Queue
Insert(Q1);
////
}else{
//free(Q1->ListN);
free(Q1);
Q1=NULL;
}
////
//

PrintQueue();

//exit(0);

return App3;

}
/////////////////////////////////////////////////////////////GOOOOOOOOOOOOOOOAAAAAOL
//Try Another Way...
////CANT GO LEFT ANY MORE
printf("\nCANT GO LEFT ANY MORE (DEAD END)");
//
//
}
}
else{
//Try Another Way...
//CANT GO LEFT ANY MORE
printf("\nCANT GO LEFT ANY MORE");
}
//Remember, if path is dead, don't need to return a pointer... delete pointer and return NULL

printf("\n ALL PATH OPTIONS EXHAUSTED... \n");

return NULL;

}


int* TraceBack(char* S,int Startr, int Startc, char** ENV){
int i = strlen(S)-1;

while(i>=0){

if(S[i]=='u')
Startr = Startr-1;

else if(S[i]=='r')
Startc=Startc-1;

else if(S[i]=='d')
Startr=Startr+1;

else if(S[i]=='l')
Startc=Startc+1;

i = i-1;
}

//Down
if(ENV[Startr-1][Startc]=='P'){

Startr=Startr-1;

}
//Right
else if(ENV[Startr][Startc-1]=='P'){

Startc=Startc-1;

}
//Up
else if(ENV[Startr+1][Startc]=='P'){

Startr=Startr+1;
}
//Left
else if(ENV[Startr][Startc+1]=='P'){

Startc = Startc+1;

}


int* V = (int*)malloc(2*sizeof(int));
if(V==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
V[0] = Startr;
V[1] = Startc;

//Return Coordinates of original place
return V;
}

/*
void PopulateTree(int Starts, int Startc, char** ENV){


}
*/

//Follow Array Trace Given Down Tree until reached
//TRACE CHAR SEQ IS VERY IMPORTANT!
void InsertTree(char C,char* Trace){
TraceTree * T2 = T;

//ROOT NODE, BASE!!!!!!!!!!!!!!
if(T2==NULL){
printf("\n FIRST TREE NODE \n");

T2 = (TraceTree*)malloc(sizeof(TraceTree));
if(T2==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
T2-> ActFromPrev='\0';
T2->NextUp=NULL;
T2->NextRight=NULL;
T2->NextDown=NULL;
T2->NextLeft=NULL;

PrintTree(T,0);

return;
}

//ONLY SHOULD BE ROOT!!!!!!!!!!
if(Trace!=NULL){
//If Must Trace Down Tree
int i=0;
while(Trace[i]!='\0'&&T2!=NULL){
TraceTree * T4 = NULL;
if(Trace[i]=='u'){

printf("\nTRAVERSE \"UP\"\n");

T4 = T2->NextUp;
T2 = T4;
}
if(Trace[i]=='r'){

printf("\nTRAVERSE \"RIGHT\"\n");

T4 = T2->NextRight;
T2 = T4;

}
if(Trace[i]=='d'){

printf("\nTRAVERSE \"DOWN\"\n");

T4 = T2->NextDown;
T2 = T4;

}
if(Trace[i]=='l'){
printf("\nTRAVERSE \"LEFT\"\n");
T4 = T2->NextLeft;
T2 = T4;

}
i++;
}

free(Trace);
Trace=NULL;

printf("\nFINISHED FULLY RETRACING STEPS\n");
//NO LONGER NEED TO USE THIS

}
//while(T2!=NULL){
if(T2->NextUp==NULL){
if(C=='u'){
T2->NextUp=(TraceTree*)malloc(sizeof(TraceTree));
if(T2->NextUp==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
T2->NextUp->ActFromPrev = C;

T2->NextUp->NextUp=NULL;
T2->NextUp->NextRight=NULL;
T2->NextUp->NextDown=NULL;
T2->NextUp->NextLeft=NULL;

printf("\nSTORED \"UP\" ACTION\n");

}
}

else if(T2->NextRight==NULL){
if(C=='r'){

T2->NextRight=(TraceTree*)malloc(sizeof(TraceTree));
if(T2->NextRight==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
T2->NextRight->NextUp=NULL;
T2->NextRight->NextRight=NULL;
T2->NextRight->NextDown=NULL;
T2->NextRight->NextLeft=NULL;

T2->NextRight->ActFromPrev = C;

printf("\nSTORED \"RIGHT\" ACTION\n");

}
}

else if(T2->NextDown==NULL){
if(C=='d'){

T2->NextDown=(TraceTree*)malloc(sizeof(TraceTree));
if(T2->NextDown==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
T2->NextDown->NextUp=NULL;
T2->NextDown->NextRight=NULL;
T2->NextDown->NextDown=NULL;
T2->NextDown->NextLeft=NULL;

T2->NextDown->ActFromPrev = C;


printf("\nSTORED \"DOWN\" ACTION\n");

}
}

else if(T2->NextLeft==NULL){
if(C=='l'){

T2->NextLeft=(TraceTree*)malloc(sizeof(TraceTree));
if(T2->NextLeft==NULL){
    printf("\nMEM ERROR at %d\n"__LINE__);
    exit(-1);
    }
T2->NextLeft->NextUp=NULL;
T2->NextLeft->NextRight=NULL;
T2->NextLeft->NextDown=NULL;
T2->NextLeft->NextLeft=NULL;

T2->NextLeft->ActFromPrev = C;

printf("\nSTORED \"LEFT\" ACTION\n");
}
}

printf("\n\nTREE:");

PrintTree(T,0);

return;
}

//Deletes Tree Starting at a node
void DeleteTree(TraceTree* TT,int Level){

if(TT==NULL){
return;
}

if(TT!=NULL){
//printf("\n%c\n",TT->ActFromPrev);
printf("<DELETE>:%d\n",Level);
printf("%c\n",TT->ActFromPrev);

if(TT->NextUp!=NULL){
TraceTree* TT2 = TT->NextUp->NextUp;

TraceTree* TT3 = TT->NextUp->NextRight;

TraceTree* TT4 = TT->NextUp->NextDown;

TraceTree* TT5 = TT->NextUp->NextLeft;

free(TT->NextUp);

TT->NextUp=NULL;

DeleteTree(TT2,Level+1);
DeleteTree(TT3,Level+1);
DeleteTree(TT4,Level+1);
DeleteTree(TT5,Level+1);

}
else if(TT->NextRight!=NULL){
TraceTree* TT2 = TT->NextRight->NextUp;

TraceTree* TT3 = TT->NextRight->NextRight;

TraceTree* TT4 = TT->NextRight->NextDown;

TraceTree* TT5 = TT->NextRight->NextLeft;

free(TT->NextRight);

TT->NextUp=NULL;

DeleteTree(TT2,Level+1);
DeleteTree(TT3,Level+1);
DeleteTree(TT4,Level+1);
DeleteTree(TT5,Level+1);

}
else if(TT->NextDown!=NULL){
TraceTree* TT2 = TT->NextDown->NextUp;

TraceTree* TT3 = TT->NextDown->NextRight;

TraceTree* TT4 = TT->NextDown->NextDown;

TraceTree* TT5 = TT->NextDown->NextLeft;

free(TT->NextDown);

TT->NextDown=NULL;

DeleteTree(TT2,Level+1);
DeleteTree(TT3,Level+1);
DeleteTree(TT4,Level+1);
DeleteTree(TT5,Level+1);



}
else if(TT->NextLeft!=NULL){

TraceTree* TT2 = TT->NextLeft->NextUp;

TraceTree* TT3 = TT->NextLeft->NextRight;

TraceTree* TT4 = TT->NextLeft->NextDown;

TraceTree* TT5 = TT->NextLeft->NextLeft;

free(TT->NextLeft);

TT->NextLeft=NULL;

DeleteTree(TT2,Level+1);
DeleteTree(TT3,Level+1);
DeleteTree(TT4,Level+1);
DeleteTree(TT5,Level+1);

}

//Delete Last one IF all neighbors are DELETED!

//TraceTree* TT2 = (TraceTree*)malloc(sizeof(TraceTree));

//TT2->NextUp=TT;

//free(TT2);

//TT2=NULL;

//free(TT2->NextUp);

printf("<\\DELETE>:%d\n",Level);
}

if(TT!=T){
//NOT THE ROOT
free(TT);
TT=NULL;
}
printf("\n\nTREE:");

PrintTree(T,0);

return;
}

//PREORDER TRAVERSAL
void PrintTree(TraceTree* TT,int Level){

//printf("\n<LEVEL>:%d (U,R,D,L)",Level);

if(TT==NULL){
printf("\nNULL\n");
return;
}

if(TT!=NULL){
printf("%c\n",TT->ActFromPrev);
//printf("LEVEL:%d\n",Level);
printf("<LEVEL>:%d (U,R,D,L)\n",Level);
PrintTree(TT->NextUp,Level+1);
//printf("\n<\\LEVEL>:%d (U,R,D,L)",Level);
PrintTree(TT->NextRight,Level+1);
//printf("\n<\\LEVEL>:%d (U,R,D,L)",Level);
PrintTree(TT->NextDown,Level+1);
//printf("\n<\\LEVEL>:%d (U,R,D,L)",Level);
PrintTree(TT->NextLeft,Level+1);
printf("<\\LEVEL>:%d (U,R,D,L)\n",Level);
}

return;
}

