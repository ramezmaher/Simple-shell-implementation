#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#define inLen 1000
#define argLen 50

//Create log file
FILE * file_pointer;

char* ind[1000000];
int temp;

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

void write_log(int sig){
    file_pointer = fopen("LOGFILE.log","a");
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(file_pointer,"Process %s has terminated -- Date&Time:%s\n",ind[temp],asctime (timeinfo));
    fclose(file_pointer);
}
int main(){
    file_pointer = fopen("LOGFILE.log","w");
    fprintf(file_pointer,"Log file:\n");
    fclose(file_pointer);
    //variables definitions
    char inputStr[inLen]; //stores input command given by the user
    char* args[argLen]; //stores all the arguments of the input command eg. args[0]="ls",args[1]=[-a]..
    int VerLen,VerArg; //Those values are meant to test whether the input is out of the variables bounds
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
            chdir("/home/ramez");
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
    //pipe for the communication between parent and child
    int p[2];
    if(pipe(p) == -1){
        perror("Error while opening the pipe!\n");
        return 1;
    }

    //forking main process to a parent and a child 
    int id = fork();
    if(id != 0){
        signal(SIGCHLD,write_log);
        if(!Flag){
            wait(NULL);
        }
        int cid;
        read(p[0], &cid,sizeof(int));
        ind[cid] = inputStr;
        temp = cid;
        goto TakeInput;
    }
    else{
        pid_t pid = getpid();
        write(p[1], &pid,sizeof(int));
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
