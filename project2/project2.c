//
//  project2.c
//  CSS 430
//
//  Created by Uyen Minh Hoang on 4/19/20.
//  Copyright © 2020 Uyen Minh Hoang. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

//structure for passing data to threads
typedef struct {
    int row;
    int column;
} parameters;

int sudoku[9][9] = {
    {6, 2, 4, 5, 3, 9, 1, 8, 7},
    {5, 1, 9, 7, 2, 8, 6, 3, 4},
    {8, 3, 7, 6, 1, 4, 2, 9, 5},
    {1, 4, 3, 8, 6, 5, 7, 2, 9},
    {9, 5, 8, 2, 4, 7, 3, 6, 1},
    {7, 6, 2, 3, 9, 1, 4, 5, 8},
    {3, 7, 1, 9, 5, 6, 8, 4, 2},
    {4, 9, 6, 1, 8, 2, 5, 7, 3},
    {2, 8, 5, 4, 7, 3, 9, 1, 6}};  //this is the sudoku puzzle in the book

/*int sudoku[9][9] = {
    {6, 2, 4, 5, 3, 2, 1, 8, 7},
    {5, 1, 9, 7, 2, 8, 6, 3, 4},
    {6, 3, 7, 6, 1, 4, 2, 9, 5},
    {1, 4, 3, 8, 6, 5, 7, 2, 9},
    {9, 5, 8, 2, 4, 7, 3, 6, 1},
    {7, 6, 2, 3, 9, 1, 3, 5, 8},
    {3, 7, 1, 9, 5, 6, 8, 4, 2},
    {4, 9, 6, 1, 8, 2, 5, 7, 3},
    {2, 8, 5, 4, 7, 3, 9, 1, 6}};  //this is an invalid sudoku puzzle with duplicate values
*/

/*int sudoku[9][9] = {
    {6, 2, 4, 5, 3, 9, 1, 8, 7},
    {5, 1, 9, 7, 2, 8, 6, 3, 4},
    {8, 3, 7, 6, 1, 4, 2, 9, 5},
    {1, 4, 3, 8, 10, 5, 7, 2, 9},
    {9, 5, 8, 2, 4, 7, 3, 6, 1},
    {7, 6, 2, 3, 9, 1, 4, 5, 8},
    {3, 7, 1, 9, 5, 6, 8, 4, 2},
    {4, 9, 6, 1, 8, 2, 5, 7, 3},
    {2, 8, 5, 4, 7, 3, 9, 1, 6}}; //this is an invalid sudoku puzzle with value bigger than 9 or smaller than 1
*/

/* This is an array of integer values that is visible to each thread. The ith index in this array corresponds to the ith worker thread. If a worker sets its corresponding value to 1, it is indicating that its region of the Sudoku puzzle is valid. A value of 0 indicates otherwise. When all worker threads have completed, the parent thread checks each entry in the result array to determine if the Sudoku puzzle is valid.
 */
int valid[11];
/* A thread to check that each column contains the digits 1 through 9
 A thread to check that each row contains the digits 1 through 9
 Nine threads to check that each of the 3 × 3 subgrids contains the digits 1
 through 9
 => A total of 11 threads
 
 Index:
 - Index 0 for the column
 - Index 1 for the row
 - The remaining indices are for the subgrids
 */

int subgrid = 2; //the index of the first thread of the first subgrid in the valid array

/*----------------workerCol-----------------------
 Check to see if a column is valid.
 Parameter:
 - param: the thread
 Return: nothing
 */
void *workerCol(void *param) {
    parameters *worker  = (parameters *) param;
    
    //loop through each column
    for(int i = worker->column; i < 9; i++) {
        int checkCol[9] = {0};  //hold the column which is being checked
        
        for(int j = worker->row; j < 9; j++) {
            int value = sudoku[j][i];   //get the value from the sudoku board
            
            if(checkCol[value - 1] == 0) {    //hasn't checked this square yet
                if(value >= 1 && value <= 9) {      //check the value to see if it's between 1 and 9
                    checkCol[value - 1]++;
                }
                else {      //if it's not in that range, then exit the thread
                    valid[1] = 0;
                    pthread_exit(0);
                }
            }
            //this square has already been checked -> meaning there is duplicate value
            else {
                valid[0] = 0;
                pthread_exit(0);
            }
        }
    }
    valid[0] = 1;
    pthread_exit(0);
}

/*----------------workerRow-----------------------
 Check to see if a row is valid.
 Parameter:
 - param: the thread
 Return: nothing
 */
void *workerRow(void *param) {
    parameters *worker  = (parameters *) param;
    
    //loop through each row
    for(int i = worker->row; i < 9; i++) {
        int checkRow[9] = {0};  //hold the row which is being checked
        
        for(int j = worker->column; j < 9; j++) {
            int value = sudoku[i][j];       //get the value from the sudoku board
            
            if(checkRow[value - 1] == 0) {    //hasn't checked this square yet
                if(value >= 1 && value <= 9) {  //check the value to see if it's between 1 and 9
                    checkRow[value - 1]++;
                }
                else {      //if it's not in that range, then exit the thread
                    valid[1] = 0;
                    pthread_exit(0);
                }
            }
            //this square has already been checked -> meaning there is duplicate value
            else {
                valid[1] = 0;
                pthread_exit(0);
            }
        }
    }
    valid[1] = 1;       //all rows have been checked
    pthread_exit(0);
}

/*----------------worker3x3-----------------------
 Check to see if a 3x3 subgrid is valid.
 Parameter:
 - param: the thread
 Return: nothing
 */
void *worker3x3(void *param) {
    parameters *worker  = (parameters *) param;
    
    //loop through a 3x3 subgrid
    for(int i = worker->row; i < worker->row + 3; i++) {
        int sortedSubgrid[9] = {0};  //hold the subgrid which is being checked
        
        for(int j = worker->column; j < worker->column + 3; j++) {
            int value = sudoku[i][j];   //get the value from the sudoku board
            
            if(sortedSubgrid[value - 1] == 0) {    //hasn't checked this square yet
                if(value >= 1 && value <= 9) {  //check the value to see if it's between 1 and 9
                    sortedSubgrid[value - 1]++;
                }
                else {          //if it's not in that range, then exit the thread
                    valid[subgrid] = 0;
                    subgrid++;      //move to the next index of subgrids in the valid array
                    pthread_exit(0);
                }
            }
            //this square has already been checked -> meaning there is duplicate value
            else {
                valid[subgrid] = 0;
                subgrid++; //move to the next index of subgrids in the valid array
                pthread_exit(0);
            }
        }
    }
    valid[subgrid] = 1;     //finished checking 1 subgrid
    subgrid++;  //move to the next index of subgrids in the valid array
    pthread_exit(0);
}

int main(int arg, char *argv[]) {
    
    //create pointer to the worker thread
    parameters *rowData = (parameters *) malloc(sizeof(parameters));
    rowData->row = 0;
    rowData->column = 0;
    
    parameters *colData = (parameters *) malloc(sizeof(parameters));
    colData->row = 0;
    colData->column = 0;
    
    parameters * first3x3 = (parameters *) malloc(sizeof(parameters));
    first3x3->row = 0;
    first3x3->column = 0;
    
    parameters * second3x3 = (parameters *) malloc(sizeof(parameters));
    second3x3->row = 0;
    second3x3->column = 3;
    
    parameters * third3x3 = (parameters *) malloc(sizeof(parameters));
    third3x3->row = 0;
    third3x3->column = 6;
    
    parameters * fourth3x3 = (parameters *) malloc(sizeof(parameters));
    fourth3x3->row = 3;
    fourth3x3->column = 0;
    
    parameters * fifth3x3 = (parameters *) malloc(sizeof(parameters));
    fifth3x3->row = 3;
    fifth3x3->column = 3;
    
    parameters * sixth3x3 = (parameters *) malloc(sizeof(parameters));
    sixth3x3->row = 3;
    sixth3x3->column = 6;
    
    parameters * seventh3x3 = (parameters *) malloc(sizeof(parameters));
    seventh3x3->row = 6;
    seventh3x3->column = 0;
    
    parameters * eighth3x3 = (parameters *) malloc(sizeof(parameters));
    eighth3x3->row = 6;
    eighth3x3->column = 3;
    
    parameters * ninth3x3 = (parameters *) malloc(sizeof(parameters));
    ninth3x3->row = 6;
    ninth3x3->column = 6;
    
    void * row;
    void * col;
    void * firstSubgrid;
    void * secondSubgrid;
    void * thirdSubgrid;
    void * fourthSubgrid;
    void * fifthSubgrid;
    void * sixthSubgrid;
    void * seventhSubgrid;
    void * eighthSubgrid;
    void * ninthSubgrid;
    
    pthread_t rowThread, colThread, first3x3Thread, second3x3Thread, third3x3Thread, fourth3x3Thread, fifth3x3Thread, sixth3x3Thread, seventh3x3Thread, eighth3x3Thread, ninth3x3Thread;
    //pthread_attr_t attr; This one does not work for pthread_create
    
    //create the threads
    pthread_create(&rowThread, NULL, workerRow, (void*) rowData);
    pthread_create(&colThread, NULL, workerCol, (void*) colData);
    pthread_create(&first3x3Thread, NULL, worker3x3, (void*) first3x3);
    pthread_create(&second3x3Thread, NULL, worker3x3, (void*) second3x3);
    pthread_create(&third3x3Thread, NULL, worker3x3, (void*) third3x3);
    pthread_create(&fourth3x3Thread, NULL, worker3x3, (void*) fourth3x3);
    pthread_create(&fifth3x3Thread, NULL, worker3x3, (void*) fifth3x3);
    pthread_create(&sixth3x3Thread, NULL, worker3x3, (void*) sixth3x3);
    pthread_create(&seventh3x3Thread, NULL, worker3x3, (void*) seventh3x3);
    pthread_create(&eighth3x3Thread, NULL, worker3x3, (void*) eighth3x3);
    pthread_create(&ninth3x3Thread, NULL, worker3x3, (void*) ninth3x3);
    
    pthread_join(rowThread, &row);
    pthread_join(colThread, &col);
    pthread_join(first3x3Thread, &firstSubgrid);
    pthread_join(second3x3Thread, &secondSubgrid);
    pthread_join(third3x3Thread, &thirdSubgrid);
    pthread_join(fourth3x3Thread, &fourthSubgrid);
    pthread_join(fifth3x3Thread, &fifthSubgrid);
    pthread_join(sixth3x3Thread, &sixthSubgrid);
    pthread_join(seventh3x3Thread, &seventhSubgrid);
    pthread_join(eighth3x3Thread, &eighthSubgrid);
    pthread_join(ninth3x3Thread, &ninthSubgrid);
    
    //check the valid array
    for(int i = 0; i < 11; i++) {
        if(valid[i] == 0) {
            printf("The Sudoku Board is not solved. \n");
            exit(0);
        }
    }
    printf("Congratulation, the Sudoku Board is solved. \n");
    
    return 0;
}
