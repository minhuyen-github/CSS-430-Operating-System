//
//  schedule_priority_rr.c
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

//create multiple task lists for each priority level as the OS book described
struct node *list[MAX_PRIORITY + 1];
struct node *currentTask;   //put it here so it can be initialized
struct node *queue = NULL;

void add(char *name, int priority, int burst) {
    //allocate a space for the struct
    Task *newTask = malloc(sizeof(Task));   //same way the source code use
    
    //initialize the new node
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&list[priority], newTask);
}

//make a queue starting from T1
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

//perform rr scheduling algorithm
Task *pickNextTask(struct node *task) { //have to put the method as a pointer to use it in the next method
    Task *nextTask = currentTask->task;
    if(currentTask->next != NULL) {
        currentTask = currentTask->next;
    }
    else {
        currentTask = task;
    }
    return nextTask;
}

void schedule() {
    //schedule the highest-priority queue first, then move down to others
    for(int i = MAX_PRIORITY; i >= MIN_PRIORITY; i--) {
        currentTask = list[i];
        makeQueue();
        currentTask = queue;
        while(queue != NULL) {
            Task *curr = pickNextTask(queue);
            int runTime = 0;
            if(QUANTUM < curr->burst) {
                runTime = QUANTUM;
            }
            else {
                runTime = curr->burst;
            }
            run(curr, runTime);
            curr->burst -= runTime;
            if(curr->burst == 0) {
                delete(&queue, curr);
            }
        }
    }
}
