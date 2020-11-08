#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define inLen 1000
#define argLen 50

//Create log file
FILE * file_pointer;

//Used for debugging
void print_args(char* str[]){
    int i=0;
    while(str[i] != NULL){
        printf("%s\n",str[i++]);
    }

}

//This checks if command terminates with & to order a background working command, return 1 if so, and 0 if it does not contain &
int checks_for_Ampercent(char* str[],int len){
    if(strcmp(str[len-1],"&") == 0){
        str[len-1] = '\0';
        return 1;
    }
    return 0;
}

//This method divide the input string into smaller substrings representing the arguments of the command
int divide_string(char input[],char* arr[]){
    int i=0;
    arr[i] = strtok(input," ");
    while(arr[i] != NULL){
        i++;
        arr[i] = strtok(NULL," ");
    }
    return i;
}

//This method loops over a string until it finds the newLine character and then replaces it with a null character
int delete_newLine(char str[]){
    int i=0;
    while(str[i] != '\n'){
        i++;
    }
    str[i] = '\0';
    return i;
}

//Write to the log file
void write_to_log(int sig){
    __pid_t pid;
    int status;
    file_pointer = fopen("LOGFILE.log","a");
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(file_pointer,"ID= %d  Status= %d  Time:%s",pid,status,asctime (timeinfo));
    fclose(file_pointer);
}

int main(){
    //variables definitions
    char inputStr[inLen]; //stores input command given by the user
    char* args[argLen]; //stores all the arguments of the input command eg. args[0]="ls",args[1]=[-a]..
    int VerLen,VerArg; //Those values are meant to test whether the input is out of the variables bounds
    file_pointer = fopen("LOGFILE.log","w");
    fprintf(file_pointer,"Log file:\n");
    fclose(file_pointer);

    TakeInput:
    //Taking the input and preparing it for the operations
    fgets(inputStr,inLen,stdin); //Takes input
    VerLen = delete_newLine(inputStr); //Removes the newLine character from the end of the input string
    VerArg = divide_string(inputStr,args); //Divide the string into smaller substrings & forming the arguments array of strings
    //Checks if command length is out of bound
    if(VerLen >= inLen || VerArg >= argLen){
        printf("Invalid command\n");
        goto TakeInput;
    }
    //checks if empty command
    if(VerLen < 1 || VerArg < 1){
        printf("No command\n");
        goto TakeInput;
    }
    //checks if it is the exit command
    if(strcmp(args[0],"exit") == 0){
        goto Terminate;
    }
    //Implements the change directory command
    if(strcmp(args[0],"cd") == 0){
        if(VerArg < 2){
            chdir("/home/");
        }
        else{
            int val = chdir(args[1]);
            if(val < 0 ){
                perror("Faild to change directory!\n");
            }
        }
        goto TakeInput;
    }
    int Flag = checks_for_Ampercent(args,VerArg);
    //forking main process to a parent and a child
    int id = fork();
    if(id != 0){
        signal(SIGCHLD,write_to_log);
        if(!Flag){
            wait();
        }
        goto TakeInput;
    }
    else{
        //Executes the commands given by the user, prints error message if command not found.Terminates child process after finishing.
        int valid = execvp(args[0],args);
        if(valid < 0 ){
            perror("Command not found!\n");
        }
        exit(0);
    }
    Terminate:
    return 0;
}
