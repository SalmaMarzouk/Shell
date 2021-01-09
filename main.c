#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_SIZE 1000

//function to execute built-in command
void exitCommand(){
    printf("Bye!^_^\n");
    exit(0) ;
}
void handler(int signum){
    FILE *fp;
    fp = fopen("logger.log","a");     //open log file in append mode
    if(fp == NULL)
    {
        printf("Error opening file");
    }
    fputs("Child process was terminated \n",fp);
    fclose(fp);
    wait(NULL);
}

//function to execute system command
void execute(char **parsedInput)
{
    signal(SIGCHLD,handler);
    pid_t pid = fork();                                  //forking child

    if (pid == -1) {
        perror("\nFailed forking child..\n");
        return;
    } else if (pid == 0) {                                //child process
        if (execvp(parsedInput[0], parsedInput) < 0) {
            perror("\nCould not execute command..\n");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}


//function to execute background commands
void backgroundExecute(char **parsedInput){
    pid_t pid = fork();                                  //forking child

    if (pid == -1) {
        perror("\nFailed forking child..\n");
        return;
    } else if (pid == 0) {                                //child process
        if (execvp(parsedInput[0], parsedInput) < 0) {
            perror("\nCould not execute command..\n");
        }
        exit(0);
    } else {
        signal(SIGCHLD,handler);          // the signal delivered to the parent, handler activated
        return;
    }

}
int main() {
    char input[MAX_SIZE];                                  //array of chars to take input
    char *parsedInput[MAX_SIZE];                            //array of strings for each word of the input
    char lower[MAX_SIZE];


    while (1) {
        usleep(10000);                                //to wait for the output in background instruction
        //set all elements with null
        memset(parsedInput,0,MAX_SIZE*sizeof(char*));
        memset(lower,0,MAX_SIZE*sizeof(char));
        memset(input,0,MAX_SIZE*sizeof(char));
        printf(">>>");
        fgets(input,MAX_SIZE,stdin);                           //read input line
        char *withoutNewLine = strtok(input,"\n");       //remove \n to get the correct command
        strcpy(input,withoutNewLine);
        char *token = strtok(input, " ");                //split the input into words
        int i=0;
        while (token != NULL) {
            parsedInput[i]=token;                              //put every word in the array of words
            token = strtok(NULL, " ");
            i++;
        }
        //convert the first word into lowercase to compare it
        for(int k=0;k<strlen(parsedInput[0]);k++){
            lower[k]=(char )tolower(parsedInput[0][k]);
        }

        //exit if the first word of the command is exit
        if(strcmp(lower,"exit")==0||strcmp(lower,"exit&")==0){
            exitCommand();
            return 0;
        }

        else if(parsedInput[i-1][strlen(parsedInput[i-1])-1]=='&'){
            parsedInput[i-1] = strtok(parsedInput[i-1],"&");       //remove & to get the correct args.
            backgroundExecute(parsedInput);
        }
        else{
            execute(parsedInput);
        }
    }
}
