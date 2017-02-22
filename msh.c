/* msh.c CS240 - Krings - Shell program
 * Cameron Parrish - parr3147
 * 02/22/2017
 * Developed on Mac OSX 10.12.3 Sierra
 * Tested on above machine and Wormulon
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define BUFFER_SIZE 256

void msh_cd(char **);

int main(){
    char line[BUFFER_SIZE];        //get command line
    char* argv[10];                //user command
    int argc;                      //arg count

    while(1){

    printf("?:");                 //print shell prompt

    int counter = 0;
    for(counter = 0 ; counter < 10 ; counter++){
      argv[counter] = NULL;
    }

    int curr = getchar();                        // initiate curr with first char from stdin
    int y = 0;                                   // counter to keep track of number of strings in input line

    while( curr >= 32 && curr <= 127){           // checks if char is within range to be valid input
        static char tmp_line[BUFFER_SIZE];       // char buffer to hold word from input
        static char * tmp_ptr = tmp_line;
        int x = 0;

        if( curr == 32)                          // used to ignore white space
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
        argv[y] = new_string;
        y++;

    }

    if(strcmp(argv[0], "exit\0") == 0){         //check if command is exit
        return 0;
    }
    if(strcmp(argv[0], "cd\0") == 0){         //check if command is exit
        msh_cd(argv);
    }

    int pid= fork();                            // fork child

    if(pid ==0 )                                // if fork is child
    {
        execvp( *argv , argv );
        fprintf(stderr, "Command not found\n");
    }
    else
    {                                          // if fork is parent
        wait(NULL);
        printf("Child exited\n");
    }

    }
}

void msh_cd(char **argv)
{
  if (argv[1] == NULL)
  {
    fprintf(stderr, "expected argument missing from cd command\n");
  }
  else
  {
    if (chdir(argv[1]) != 0)
    {
      perror("msh");
    }
  }
}
