//
//  CPU.c
//  CSS 430
//
//  Created by Uyen Minh Hoang on 4/22/20.
//  Copyright © 2020 Uyen Minh Hoang. All rights reserved.
//

#include "CPU.h"
/**
 * "Virtual" CPU that also maintains track of system time.
 */

#include <stdio.h>

#include "task.h"

// run this task for the specified time slice
void run(Task *task, int slice) {
    printf("Running task = [%s] [%d] [%d] for %d units.\n",task->name, task->priority, task->burst, slice);
}
