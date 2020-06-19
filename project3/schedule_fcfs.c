//
//  schedule_fcfs.c
//  CSS 430
//
//  Created by Uyen Minh Hoang on 4/22/20.
//  Copyright © 2020 Uyen Minh Hoang. All rights reserved.
//

//implement the FCFS scheduling algorithm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node *list = NULL;   //create a task list

//implement the add method
void add(char *name, int priority, int burst) {
    //allocate a space for the struct
    Task *newTask = malloc(sizeof(Task));   //same way the source code use
    
    //initialize the new node
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&list, newTask); //add it to the list
}

//need to get the last node of the list because new tasks are inserted at the beginning of the list
Task *pickNextTask() { //have to put the method as a pointer to use it in the next method
    struct node *currentTask = list;    //iterator to iterate through the list
    while(currentTask->next != NULL) {  //while loop to pick the task
        currentTask = currentTask->next;
    }
    return currentTask->task;
}

void schedule() {
    while(list != NULL) {
        Task *currentTask = pickNextTask(); //need pointer to access the struct
        run(currentTask, currentTask->burst);
        delete(&list, currentTask); //remove the finished task from the list
    }
}
