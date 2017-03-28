/* msh.c CS240 - Krings - Shell program
 * Cameron Parrish - parr3147
 * 02/22/2017
 * Developed on Mac OSX 10.12.3 Sierra
 * Tested on above machine and Wormulon
 */

// user defined global variables
char * history[20][10];             // stores the history of the commands typed
#define BUFFER_SIZE 256             //
char line[BUFFER_SIZE];             //get command line
char* args[10];                     //user command
char * alias[10][2];                // stores the aliases defined by the user
int argcount;                       //arg count
int commands = 0;

typedef int bool;
# define true 1
# define false 0

// user includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

// function prototypes
void parse();
void execute();
void read_file(char *[]);
void add_to_history();
void print_history();   // prints the history array
void msh_cd(char **);   // executes a cd command
void substitute();
void add_path();
void add_alias();
void delete_alias();
void check_aliases();

int main(int argc , char * argv[]){

    if(argc > 1){
      read_file(argv);
      }

    while(1){

        printf("?:");                 //print shell prompt

        int i , j;
        parse();
        execute();

      }
}

void parse(){
  int counter = 0;
  for(counter = 0 ; counter < 10 ; counter++){
    args[counter] = NULL;
  }

  int i , j;

  int curr = getchar();                        // initiate curr with first char from stdin
  int y = 0;                                   // counter to keep track of number of strings in input line

  while( curr >= 32 && curr <= 127){           // checks if char is within range to be valid input
      static char tmp_line[BUFFER_SIZE];       // char buffer to hold word from input
      static char * tmp_ptr = tmp_line;
      int x = 0;                               // initialized every time the loop is executed

      while( curr == 32)                          // used to ignore white space in front of first valid char
      {
          curr = getchar();
      }

      while( curr > 32 && curr < 127 )         // while char is not space or carriage return
      {
          tmp_line[x] = curr;
          curr = getchar();
          x++;                                 // keeps track of chars in tmp string
      }

      char * new_string = malloc( (x+1) * sizeof(char) );
      int z = 0;

      for(z = 0; z < x ; z++)
      {
          new_string[z] = tmp_line[z];
      }

      new_string[x] = '\0';                    // terminates string with NULL terminator
      args[y] = new_string;
      y++;

  }
  // this function will update the history array
  add_to_history();
}


void execute(){

    if(strcmp(args[0], "exit\0") == 0){             // check if command is exit
        exit(0);
    }
    if(strcmp(args[0], "cd\0") == 0){         //check if command is exit
        msh_cd(args);
    }

    if(strcmp(args[0], "history\0") == 0){         //check if command is exit
        print_history();
        return;
    }
    int i;
    if(strcmp(args[0], "!!\0") == 0){         //check if command is !!
        for(i = 0 ; i < 10 ; i++){
          args[i] = history[1][i];
        }
    }

    if(strcmp(args[0], "export\0") == 0){         //check if command is exit
        add_path();
        return;
    }

    if(strcmp(args[0], "alias\0") == 0){         //check if command is exit
        add_alias();
    }

    if(strcmp(args[0], "unalias\0") == 0){         //check if command is exit
        delete_alias();
    }

    // check_aliases();

    //checks to see if a carat is present to represent substitution
    const char * carat = "^";
      if(strstr(history[0][0] , carat)){
        substitute();
      }

    int pid = fork();                            // fork child

    if(pid ==0 )                                // if fork is child
    {
        execvp( *args , args );
        fprintf(stderr, "Command not found\n");
    }
    else
    {                                          // if fork is parent
        wait(NULL);
        // printf("Child exited\n");
    }
}

void read_file(char * argv[]){
  FILE * fin;
  fin = fopen(argv[1] , "r");
  if( fin == NULL){
    printf("%s" , "The file did not open properly\n");
    exit(0);
  }
  int curr = getc(fin);
  int count = 0;
  while(curr != EOF){
      count = 0;
      char * buffer = malloc(100 * sizeof(char));
      while(curr >= 31 && curr <= 127){
        buffer[count] = curr;
        count++;
        curr = getc(fin);
      }
      int i;
      for(i = count ; i >= 0 ; i--){
        ungetc(buffer[i] , stdin);
      }
        parse();
        execute();
        curr = getc(fin);
        free(buffer);
  }
  fclose(fin);
}

void add_to_history(){
  // the following code will store the commands to the history array
  int i , j;
  for(i = 19 ; i > 0 ; i--){
    for(j = 0 ; j < 10 ; j++){
      history[i][j] = history[i-1][j];
    }
  }
  for(j = 0 ; j < 10 ; j++){
    history[0][j] = args[j];
  }
}

void msh_cd(char **args){
  if (args[1] == NULL)
  {
    fprintf(stderr, "expected argument missing from cd command\n");
  }
  else
  {
    chdir(args[1]);
  }
}

void print_history(){
  int i , j;
  for(i = 0; i < 20 ; i++){
    j = 0;
    if(history[i][0] != NULL){
      printf("%i""%s" , i+1 + commands , ": ");
      for(j = 0 ; j < 10 ; j++){
        if(history[i][j] != NULL){
          printf("%s""%s", history[i][j] , " ");
        }
      }
      printf("%s" , "\n");
    }
  }
}

void substitute(){
  char * buffer = malloc(100 * sizeof(char));     // string buffer
  int i;
  int count = 0;                                  // counts the number of carats
  int first, second;                              // used to store index of each carat
  for(i = 0 ; i < strlen(history[0][0]) ; i++){   // process the substitution command
    if(history[0][i] == 94){                      // count the carats
      count++;
      if(count == 1){                             // if first carat
        first = count;
      }else{                                      // if second carat
        second = count;
      }
    }
  }
  const char carat[2] = "^";
  char * before = strtok(history[0][0] , carat);
  char * after =  strtok(NULL , carat);

  bool found = false;   // indicated you found the first instance of what is being substituted
  i = 0;
  while(!found && i < 10){                 // search array for string to be substituted (bound by array size)
    if( strstr(history[1][i] , before)){
      found = true;
    }
    i++;
  }

  if(found == false){    // checks to see that the replacement can be made
    printf("%s" , "the replacement could not be made\n");
  }else{
    // parts get copied into the string buffer

    // parse part before thing to be replaced
    int first_length , total_length = 0;
    first_length = strcspn(before , history[1][i]);
    first_length--;                                   // decrement so that first char of replacement
    // total_length += first_length;
    int j;                     // string isn't copied into buffer
    for(j = 0; j < first_length; j++){
      buffer[i] = history[1][i][j];
    }
    // put new part into buffer
    total_length += strlen(after);
    int x = 0;
    for(j = first_length; j < total_length; j++){
      buffer[first_length] = after[x];
      x++;
    }
    while(history[1][i][total_length]){
      buffer[total_length] = history[1][i][total_length];
      total_length++;
    }
    // buffer gets pushed onto stdin and sent to the parse function
    // for(i = total_length ; i >= 0 ; i--){
    //
    // }
    printf("%s" , buffer);
    // this will create a new string and add all of the arguments from the last command back to args
    char * new_string = malloc( (total_length+1) * sizeof(char) );
    int z = 0;
    for(z = 0; z < total_length ; z++)
    {
        new_string[z] = buffer[z];
    }

    new_string[x] = '\0';                    // terminates string with NULL terminator
    for(z = 0 ; z < 10 ; z++){
      if(z != i){
        args[z] = history[1][i];
      }else{
        args[z] = new_string;
      }
    }

    // makes sure that the history array is updated with the new command
    add_to_history();
    // execute the last command with the substitution
    execute();
  }

  free(buffer);
}

void add_path(){
  int curr = args[1][0];
  int i = 0;
  while(curr != 58){                        // find the colon
    curr = args[1][i];
    i++;
  }

  char * path = malloc(200);                // buffer to hold new path
  char * PATH = getenv("PATH");               // gets PATH variable from system
  strcat(path , PATH);                      // copies all of path into buffer
  char * buffer = malloc(100);
  // printf("%s" , curr);
  int j = 0;
  while(curr > 32){                        // while curr is valid input characters
    buffer[j] = curr;
    curr = args[1][i];
    i++;
    j++;
  }
  buffer[i] = '\0';
  strcat(path , buffer);
  setenv("PATH" , path , 1);
}

void add_alias(){
  bool empty = false;
  int i = 0;
  while(empty == false && i < 10){
    if(alias[i][0] == NULL){
      empty = true;
      int j;
      for(j = 0 ; j < 10 ; j++){
        alias[i][j] = args[j+1];
      }
    }
    i++;
  }
}
void delete_alias(){

  int i = 0;
  while(i < 10){
    if(strcmp(alias[i][0] , args[1])){
      int j;
      for(j = 0 ; j < 10 ; j++){
        alias[i][j] = NULL;
      }
    }
    i++;
  }
}
void check_aliases(){
  int i,j;

  for(j = 0 ; j < 10 ; j ++){
    for(i = 0 ; i < 10 ; i ++){
      if(strcmp(args[i] , alias[j][0]) == 0){
        int x, y;
        for(x = 10; x >= 0 ; x++){
          if(x != i){
            for(y = strlen(args[x]) ; y >= 0 ; y--){
              ungetc((int)args[x][y] ,stdin);
            }
          }else{
            for(y = strlen(alias[j][0]) ; y >= 0 ; y--){
              ungetc((int)alias[j][0][y] , stdin);
            }
          }
        }
      }
    }
  }
  parse();
  execute();
}
