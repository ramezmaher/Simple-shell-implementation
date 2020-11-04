#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define inLen 1000
#define argLen 50

//Used for debugging
void print_args(char* str[]){
    int i=0;
    while(str[i] != NULL){
        printf("%s\n",str[i++]);
    }

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

int main(){

    //variables definitions 
    char inputStr[inLen]; //stores input command given by the user 
    char* args[argLen]; //stores all the arguments of the input command eg. args[0]="ls",args[1]=[-a]..
    int VerLen,VerArg; //Those values are meant to test whether the input is out of the variables bounds

    //Taking the input and preparing it for the operations
    fgets(inputStr,inLen,stdin); //Takes input 
    VerLen = delete_newLine(inputStr); //Removes the newLine character from the end of the input string 
    VerArg = divide_string(inputStr,args); //Divide the string into smaller substrings & forming the arguments array of strings
    if(VerLen >= inLen || VerArg >= argLen){
        printf("Invalid command\n");
        return 0;
    } // checks if the input is out of the size bound
    

    return 0;
}