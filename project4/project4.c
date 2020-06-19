//
//  project4.c
//  CSS 430
//
//  Created by Uyen Minh Hoang on 5/19/20.
//  Copyright © 2020 Uyen Minh Hoang. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MEMSIZE 80  //the maximum size of a memory pool

char *pool[MEMSIZE];    //create an array to represent a memory pool

void init() {   //initialize the array
    for(int i = 0; i < MEMSIZE; i++) {
        pool[i] = ".";
    }
}

void allocate(char *name, int size, char *algo) {
    switch(*algo) {
        case 'F': {     //first-fit algorithm
            int it = 0;     //iterator
            int currSize = 0;     //size of the current unused block
            bool found = false;     //flag to break out of loop
            while(found != true) {    //loop through the array
                if(strcmp(pool[it], ".") == 0) {    //if a hole is empty
                    if(it == MEMSIZE - 1 && currSize < size - 1) {
                        printf("Not enough available memory to allocate!\n");
                        return;
                    }
                    if(currSize >= size - 1) {  //if found a block, immediately grab it
                        found = true;
                    }
                    else {
                        currSize++; //increase the current size
                    }
                }
                else {  //if a hole is not empty
                    if(it == MEMSIZE - 1 && currSize < size - 1) {
                        printf("Not enough available memory to allocate!\n");
                        return;
                    }
                    currSize = 0;   //reset the current size
                }
                it++;   //move to the next hole
            }
            it -= 1;
            for(int i = 0; i < size; i++) {     //initialize the block with the process
                pool[it] = name;
                it--;
            }
            break;
        }
        case 'B': {     //best-fit algorithm
            int it = 0;     //iterator
            int currSize = 0;   //size of the current unused block
            int min = MEMSIZE;  //holds the smallest block that fits
            int best = 0;   //holds the index of min
            bool flag = false;      //flag to break out of loop
            while(flag != true) {
                while(strcmp(pool[it], ".") == 0) {    //if a hole is empty
                    if(it == MEMSIZE - 1) {     //if reach the end of the pool
                        if(currSize < size - 1) {   //if last unused block is smaller than requested size
                            printf("Not enough available memory to allocate!\n");
                            return;
                        }
                        else {      //if not, break out of loop
                            currSize++;
                            flag = true;
                            break;
                        }
                    }
                    ++it;   //move to the next hole
                    currSize++;
                }
                if(currSize < min && currSize >= size - 1) {    //find the smallest block that fits
                    min = currSize;
                    best = it - currSize;
                    if(it == MEMSIZE - 1) {
                        break;
                    }
                }
                while(strcmp(pool[it], ".") != 0) {  //if a hole is not empty
                    if(it == MEMSIZE - 1) {
                        if(currSize < size - 1) {
                            printf("Not enough available memory to allocate!\n");
                            return;
                        }
                        else {
                            flag = true;
                            break;
                        }
                    }
                    ++it;       //move to the next block
                    currSize = 0;
                }
            }
            for(int i = 0; i < size; i++) {     //initialize the block with the process
                pool[best] = name;
                best++;
            }
            break;
        }
        case 'W': {     //worst-fit algorithm
            int it = 0;     //iterator
            int currSize = 0;   //size of the current unused block
            int max = 0;    //biggest block in the pool
            int worst = 0;      //index of max
            bool flag = false;      //flag to break out of loop
            while(flag != true) {
                while(strcmp(pool[it], ".") == 0) {    //if a hole is empty
                    if(it == MEMSIZE - 1) {
                        if(currSize < size - 1) {
                            printf("Not enough available memory to allocate!\n");
                            return;
                        }
                        else {
                            currSize++;
                            flag = true;
                            break;
                        }
                    }
                    it++;
                    currSize++;
                }
                if(currSize > max && currSize > size - 1) {     //find the biggest block in the pool
                    max = currSize;
                    worst = it - currSize;
                }
                while(strcmp(pool[it], ".") != 0) {  //if a hole is not empty
                    if(it == MEMSIZE - 1) {
                        if(currSize < size - 1) {
                            printf("Not enough available memory to allocate!\n");
                            return;
                        }
                        else {
                            flag = true;
                            break;
                        }
                    }
                    it++;
                    currSize = 0;
                }
            }
            for(int i = 0; i < size; i++) {     //initialize the block with the process
                pool[worst] = name;
                worst++;
            }
            break;
        }
    }
}

void release(char *name) {      //release the block with the given process
    int it = 0;
    while(it < MEMSIZE) {
        if(strcmp(pool[it], name) == 0) {
            pool[it] = ".";
        }
        it++;
    }
}

//compact helper method
//given the indices of the unused and used blocks -> swap these blocks
void swap(int lowUnused, int highUnused, int lowUsed, int highUsed) {
    char *temp;
    int sizeUnused = highUnused - lowUnused;
    int sizeUsed = highUsed - lowUsed;
    if(sizeUnused < sizeUsed) {     //if the unused block is smaller
        while(lowUnused <= highUnused) {
            temp = pool[lowUsed];
            pool[lowUsed] = pool[lowUnused];
            pool[lowUnused] = temp;
            lowUsed++;
            lowUnused++;
        }
    }
    else if(sizeUnused > sizeUsed) {    //if the unused block is bigger
        while(lowUsed <= highUsed) {
            temp = pool[lowUsed];
            pool[lowUsed] = pool[lowUnused];
            pool[lowUnused] = temp;
            lowUsed++;
            lowUnused++;
        }
    }
    else if(sizeUnused == sizeUsed) {   //if they're equal
        while(lowUsed <= highUsed) {
            temp = pool[lowUsed];
            pool[lowUsed] = pool[lowUnused];
            pool[lowUnused] = temp;
            lowUsed++;
            lowUnused++;
        }
    }
}

void compact() {
    int it = 0;
    int lowUnused, highUnused, lowUsed, highUsed = 0;   //indices of unused and used blocks
    //only do the swap when the unused block is in front of the used block
    while(it < MEMSIZE) {
        //find the unused block
        if(strcmp(pool[it], ".") == 0) {
            lowUnused = it;
            while(strcmp(pool[it], ".") == 0) {
                it++;
                if(it == MEMSIZE) {
                    break;
                }
            }
            highUnused = it - 1;
            if(it == MEMSIZE) {
                break;
            }
            lowUsed = it;
            //find the used block
            while(strcmp(pool[it], ".") != 0) {
                it++;
                if(it == MEMSIZE) {
                    break;
                }
            }
            highUsed = it - 1;
            swap(lowUnused, highUnused, lowUsed, highUsed);     //swap these 2 blocks
            it = 0;     //reset the iterator
        }
        else {
            it++;
        }
    }
}

void show() {
    for(int i = 0; i < MEMSIZE; i++) {
        printf("%s", pool[i]);
    }
    printf("\n");
}

//read the text file line by line and execute the commands
void doCommand(char *file) {
    FILE *in = fopen(file, "r");
    char arg[MEMSIZE];  //store the command lines
    char *copy;
    char line;
    char *cmd;
    char *name;
    int size;
    char *algo;
    int index = 0;
    while((line = fgetc(in))) { //get character by character
        if(line == EOF) {   //if reach the end of file, execute commands then break out of loop
            arg[index] = '\0';
            copy = strdup(arg);
            cmd = strtok(copy, " ");
            if(cmd == NULL) {
                break;
            }
            if(strcmp(cmd, "A") == 0) {
                name = strtok(NULL, " ");
                size = atoi(strtok(NULL, " "));
                algo = strtok(NULL, " ");
                allocate(name, size, algo);
            }
            if(strcmp(cmd, "F") == 0) {
                name = strtok(NULL, " ");
                release(name);
            }
            if(strcmp(cmd, "S") == 0) {
                show();
            }
            if(strcmp(cmd, "C") == 0) {
                compact();
            }
            break;
        }
        if(line == '\n') {  //if reach the end of line, execute commands then continue with the loop
            arg[index] = '\0';
            index++;
            copy = strdup(arg);
            cmd = strtok(copy, " ");
            if(strcmp(cmd, "A") == 0) {
                name = strtok(NULL, " ");
                size = atoi(strtok(NULL, " "));
                algo = strtok(NULL, " ");
                allocate(name, size, algo);
            }
            if(strcmp(cmd, "F") == 0) {
                name = strtok(NULL, " ");
                release(name);
            }
            if(strcmp(cmd, "S") == 0) {
                show();
            }
            if(strcmp(cmd, "C") == 0) {
                compact();
            }
            index = 0;  //reset the index
            continue;
        }
        else {
            arg[index] = (char)line;    //insert the command line to the array
            index++;
        }
    }
    fclose(in);     //close the file
}

/*int main(int argc, char *arg[]) {
 init();
 
 if(argc == 1) {
 printf("Please enter a command!\n");
 }
 if(argc > 1) {
 if(strcmp(arg[1], "R") == 0) {
 doCommand(arg[2]);
 }
 }
 //char *file = "/Users/minhuyen/Downloads/CSS 430/CSS 430/CSS 430/project4/en.lproj/test2.txt";
 //char *file = "/Users/minhuyen/Downloads/CSS 430/CSS 430/CSS 430/project4/en.lproj/test.txt";
 //doCommand(file);
 return 0;
 }*/
int main() {
    init();
    
    char buffer[MEMSIZE];
    char temp;
    char *copy;
    char line;
    char *cmd;
    char *name;
    int size;
    char *algo;
    char *file;
    int index = 0;
    while(1) {
        while(temp = fgetc(stdin)) {
            if(temp == EOF) {
                buffer[index] = '\0';
                copy = strdup(buffer);
                cmd = strtok(copy, " ");
                printf("%s", cmd);
                if(*cmd >= 97) {    //if lowercase, change it to uppercase
                    *cmd -= 32;
                }
                if(strcmp(cmd, "A") == 0) {
                    name = strtok(NULL, " ");
                    if(*name >= 97) {
                        *name -= 32;
                    }
                    size = atoi(strtok(NULL, " "));
                    algo = strtok(NULL, " ");
                    if(*algo >= 97) {
                        *algo -= 32;
                    }
                    allocate(name, size, algo);
                }
                if(strcmp(cmd, "F") == 0) {
                    name = strtok(NULL, " ");
                    if(*name >= 97) {
                        *name -= 32;
                    }
                    release(name);
                }
                if(strcmp(cmd, "S") == 0) {
                    show();
                }
                if(strcmp(cmd, "C") == 0) {
                    compact();
                }
                if(strcmp(cmd, "R") == 0) {
                    file = strtok(NULL, " ");
                    doCommand(file);
                }
                break;
            }
            if(temp == '\n') {
                buffer[index] = '\0';
                index++;
                copy = strdup(buffer);
                cmd = strtok(copy, " ");
                if(*cmd >= 97) {
                    *cmd -= 32;
                }
                if(strcmp(cmd, "A") == 0) {
                    name = strtok(NULL, " ");
                    if(*name >= 97) {
                        *name -= 32;
                    }
                    size = atoi(strtok(NULL, " "));
                    algo = strtok(NULL, " ");
                    if(*algo >= 97) {
                        *algo -= 32;
                    }
                    allocate(name, size, algo);
                }
                if(strcmp(cmd, "F") == 0) {
                    name = strtok(NULL, " ");
                    if(*name >= 97) {
                        *name -= 32;
                    }
                    release(name);
                }
                if(strcmp(cmd, "S") == 0) {
                    show();
                }
                if(strcmp(cmd, "C") == 0) {
                    compact();
                }
                if(strcmp(cmd, "R") == 0) {
                    file = strtok(NULL, " ");
                    doCommand(file);
                }
                else if(strcmp(cmd, "A") != 0 || strcmp(cmd, "F") != 0 || strcmp(cmd, "S") != 0 || strcmp(cmd, "C") != 0 || strcmp(cmd, "R") != 0) {
                    printf("Please enter a valid command!\n");
                }
                index = 0;
                continue;
            }
            else {
                buffer[index] = (char)temp;
                index++;
            }
        }
    }
    return 0;
}
