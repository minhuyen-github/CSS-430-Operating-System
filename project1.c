//
//  project1.c
//  CSS 430
//
//  Created by Uyen Minh Hoang on 4/7/20.
//  Copyright © 2020 Uyen Minh Hoang. All rights reserved.
//

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80 /* The maximum length command */
#define TOKEN_DELIMITERS " \n\t\f\r\v"

int numCommand = 0; //number of commands that the user input

/*--------------isAmpersandOrSemiColon--------------
 Takes in an argument and check if the argument has
 an ampersand "&" or a semicolon ";".
 Parameter:
 - arg: the argument
 - numArgs: the number of arguments
 Return:
 - 0 for semicolon or none of them
 - 1 for ampersand
 */
int isAmpersandOrSemiColon(char **arg, int *numArgs) {
    long length = strlen(arg[*numArgs - 1]);
    //loop through the argument to search for the symbols
    for(int i = 0; i <= length - 1; i++) {
        if(arg[*numArgs - 1][i] == '&') {  //check for the ampersand
            arg[*numArgs - 1][i] = '\0';   //remove it from the argument
            return 1;
        }
        else if(arg[*numArgs - 1][i] == ';') { //check for the semicolon
            arg[*numArgs - 1][i] = '\0';  //remove it from the argument
            return 0;
        }
    }
    return 0;
}

/*-----------------isRedirectOperator-----------------------
 Takes in an argument and check if the argument has the
 redirect operators. If there is, then find either the
 input or output file and assign it to the dummy
 parameter.
 Parameter:
 - arg: the argument
 - numArgs: the number of arguments
 - inputFile: the input file (dummy parameter)
 - outputFile:the output file (dummy parameter)
 Return:
 - flag: a number to indicate whether there is the operator
 - 1 for input and 0 for output
 */
int isRedirectOperator(char **arg, int *numArgs, char **inputFile, char **outputFile) {
    int flag = 0;
    //loop through the argument to search for the operators
    for(int i = 0; i <= *numArgs; i++) {
        if(strcmp(arg[i], "<") == 0) { //check for input operator
            flag = 1;
            *inputFile = arg[i + 1];  //get and assign the input file
        }
        else if(strcmp(arg[i], ">") == 0) { //check for output operator
            flag = 0;
            *outputFile = arg[i + 1];  //get and assign the output file
        }
    }
    return flag;
}

/*--------------------------------redirectIO-----------------------------------
 Either redirect the input to a command from a file or redirect the output from
 a command to a file
 Paramter:
 - flag: a number to indicate the type of redirect operator
 - inputFile: the input file
 - outputFile: the output file
 - input:
 - output:
 Return:
 - 0 if no success
 - 1 if suceeded
 */
int redirectIO(int flag, char *inputFile, char *outputFile, int *input, int *output) {
    if(flag == 0) { //input
        *input = open(inputFile, O_RDONLY);
        if(*input == -1) {
            fprintf(stderr, "Cannot open the input file.");
            return 0;
        }
        dup2(*input, STDIN_FILENO);
    }
    if(flag == 1) {  //output
        *output = open(outputFile, O_RDONLY);
        if(*output == -1) {
            fprintf(stderr, "Cannot open the output file.");
            return 0;
        }
        dup2(*output, STDIN_FILENO);
    }
    return 1;
}

/*--------------------------------isPipeOperator-----------------------------
 Takes in an argument and check if there is a pipe operator between them
 Parameter:
 - arg1: the argument, if there is the operator, it will be assigned the first
         argument meaning the one before the operator
 - arg2: the second argument - the one after the operator
 - numArgs1: the number of the first argument
 - numArgs2 : the number of the second argument
 */
void isPipeOperator(char **arg1, int *numArgs1, char ***arg2, int *numArgs2) {
    //loop through the argument to find the operator
    for(int i = 0; i <= *numArgs1; i++) {
        if(strcmp(arg1[i], "|") == 0) { //if found then remove it from the argument
            free(arg1[i]);
            arg1[i] = NULL;
            //get and assign the second argument
            *numArgs2 = *numArgs1 - i - 1;
            *numArgs1 = i;
            *arg2 = arg1 + i + 1;
            break;
        }
    }
}

/*--------------------------execute------------------------------------
 Takes in an argument and the number of arguments to execute
 Parameter:
 - arg: the argument
 - numArgs: the number of argument
 */
void execute(char **arg, int numArgs) {
    //check for ampersand sign "&" and ";"
    //if "&", then parent and child run concurrently
    //if ";", then parent wait for child
    int concurrently = isAmpersandOrSemiColon(arg, &numArgs);
    
    //check for pipe
    char **arg1;
    int numArgs1 = 0;
    isPipeOperator(arg, &numArgs, &arg1, &numArgs1);
    
    //fork a child process
    pid_t pid = fork();
    if(pid < 0) {           //if fail to fork
        fprintf(stderr, "Cannot fork!\n");
    }
    else if(pid == 0) { //create the child process
        if(numArgs1 != 0) {         //if the pipe sign is found
            int fd[2];
            pipe(fd);
            
            //fork another child process from the previous child process
            //for easier understanding, call this a grandchild process :)
            pid_t pid1 = fork();
            if(pid1 > 0) {          //back to the child process
                char *inputFile, *outputFile;
                int input, output;
                int flag = isRedirectOperator(arg1, &numArgs1, &inputFile, &outputFile);
                if(redirectIO(flag, inputFile, outputFile, &input, &output) == 0) {
                }
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                wait(NULL);     //wait for the first command to finish
                execvp(arg1[0], arg1);
                
                //close the file
                if(flag == 0) {
                    close(input);
                }
                if(flag == 1) {
                    close(output);
                }
                
                close(fd[0]);
                fflush(stdin);
            }
            else if(pid1 == 0) {        //create grandchild process
                char *inputFile, *outputFile;
                int inputDes, outputDes;
                int flag = isRedirectOperator(arg1, &numArgs1, &inputFile, &outputFile);
                if(redirectIO(flag, inputFile, outputFile, &inputDes, &outputDes) == 0) {
                }
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                wait(NULL);     //wait for the first command to finish
                execvp(arg1[0], arg1);
                
                //close the file
                if(flag == 0) {
                    close(inputDes);
                }
                if(flag == 1) {
                    close(outputDes);
                }
                
                close(fd[0]);
                fflush(stdin);
            }
        }
        else {          //no pipe sign was found
            char *inputFile, *outputFile;
            int inputDes, outputDes;
            int flag = isRedirectOperator(arg1, &numArgs1, &inputFile, &outputFile);
            if(redirectIO(flag, inputFile, outputFile, &inputDes, &outputDes) == 0) {
            }
            execvp(arg1[0], arg1);
            
            //close the file
            if(flag == 0) {
                close(inputDes);
            }
            if(flag == 1) {
                close(outputDes);
            }
            fflush(stdin);
        }
    }
    else {          //back to the parent process
        if(!concurrently) {             //parent and child run concurrently
            wait(NULL);
        }
    }
}

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    char commandBuffer[MAX_LINE + 1];
    char historyCommand[MAX_LINE + 1];
    while (should_run) {
        printf("osh>");
        fflush(stdout);
        //read the input
        if(fgets(commandBuffer, MAX_LINE + 1, stdin) == NULL) {
            fprintf(stderr, "Error: Command line cannot be read.\n");
            continue;
        }
        
        if(strcmp(commandBuffer, "!!") == 0) {
            if(strlen(historyCommand) == 0) {
                fprintf(stderr, "No commands in history.");
            }
            printf("%s", historyCommand);
        }
        strcpy(historyCommand, commandBuffer); //update the history commands.

        //parse input
        char *token = strtok(commandBuffer, TOKEN_DELIMITERS);
        
        while(token != NULL) {
            args[numCommand] = malloc(strlen(token) + 1);
            strcpy(args[numCommand], token);
            ++numCommand;
            token = strtok(NULL, TOKEN_DELIMITERS);
        }
        
        if(numCommand == 0) {
            printf("Please enter a command!");
            continue;
        }
        execute(args, numCommand);
        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless command included & */
    }
    return 0;
}
