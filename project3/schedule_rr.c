//
//  schedule_rr.c
//  CSS 430
//
//  Created by Uyen Minh Hoang on 4/22/20.
//  Copyright © 2020 Uyen Minh Hoang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node *list = NULL;   //create a task list
struct node *currentTask; //put it here so it can be initialized
struct node *queue = NULL;

void add(char *name, int priority, int burst) {
    //allocate a space for the struct
    Task *newTask = malloc(sizeof(Task));   //same way the source code use
    
    //initialize the new node
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&list, newTask);
}

//perform as FCFS scheduling
Task *pickNextTask() { //have to put the method as a pointer to use it in the next method
    Task *rr = currentTask->task;
    if(currentTask->next != NULL) { //if the current node is not NULL
        currentTask = currentTask->next;
    }
    else {
        currentTask = queue;
    }
    return rr;
}

//make a queue
void makeQueue() {
    while(currentTask != NULL) {
        //allocate a space for the struct
        Task *newTask = malloc(sizeof(Task));   //same way the source code use
        //initialize the new node
        newTask->name = currentTask->task->name;
        newTask->priority = currentTask->task->priority;
        newTask->burst = currentTask->task->burst;
        insert(&queue, newTask);
        currentTask = currentTask->next;
    }
}

void schedule() {
    //initialize the current node here because the list is already in order
    currentTask = list;
    makeQueue();
    currentTask = queue;
    while(queue != NULL) {
        Task *currTask = pickNextTask(); //need pointer to access the struct
        int runTime = 0;    //holds the remaining run time for each task
        if(QUANTUM < currTask->burst) {
            runTime = QUANTUM;
        }
        else {
            runTime = currTask->burst;
        }
        run(currTask, runTime);
        currTask->burst -= runTime;
        if(currTask->burst == 0) {
            delete(&queue, currTask);
        }
    }
}

