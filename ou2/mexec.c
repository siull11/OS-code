/*
Mandatory assignment 1, Pipes
This program reads in a number of commands and arguments from
an file or stdin. For each command it startes a new process.
The different processes get linked up with pipes, the prior
processes's stdout goes to the followings stdin. After the
pipes are connected the given command with its arguments gets
executed one for each child in order. Meaning that the first
command gets executed by the first child in the pipe chain
and the last command get executed by the last child, who
writes to stdout. It is dynamic meaning that any number of
commands can be executed, as long as there is storage for it
on the system.

Author: Simon Ullerstam
CS-user: ens19sum@cs.umu.se
Date: 22-09-2020
*/

// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Constants
#define READ_END 0
#define WRITE_END 1
#define BUFF_SIZE 1024

// Struct
typedef struct {
  char **cmd;
  int nCmd;
} cmds;

// Function declaration
cmds readFile(FILE *input);
void execute(cmds c);
int **createPipes(int n);
void connectPipes(int **fd, int child, int nChild);
char **parseLine(char *line);

/* main function:
  Reads form stdin or given file.
  Calls on readFile() with that file.
  Closes the file.
  Calls on the execute function with cmds from readFile.
  Deallocates all memory used and returns.

  Arguments:
  Only one or zero arguments allowed.
*/
int main(int argc, char *argv[]) {
  // Variable
  FILE *input;

  // Check the amount of arguments
  if (argc > 2) {
    perror("To many arguments. Usage: ./mexec [FILE]");
    exit(EXIT_FAILURE);
  } else if (argc == 2) {
    // If one argument is given open file, set as input
    input = fopen(argv[1], "r");
    if (input == NULL) {
      perror("File not found");
      exit(EXIT_FAILURE);
    }
  } else {
    // Else, if no arguments are given set stdin as input
    input = stdin;
  }


  // Read then close input
  cmds c = readFile(input);
  fclose(input);


  // Execute all the commands
  execute(c);


  // Deallocate remaining memory and return
  for (int i = 0; i < c.nCmd; i++) {
    free(c.cmd[i]);
  } 
  free(c.cmd);
  return 0;
}


/* readFile function:
  It reads each row with a maxlength of BUFF_SIZE form the opened file, removes
  all '\n', counts, allocates memory for the rows and finally stores the rows
  and the amount of them in a struct, cmds.
  
  Argument: 
  The opened file to read from.

  Retruns:
  The struct storing an array of strings and the amount of strings in the array.
*/
cmds readFile(FILE *input) {
  // Variables
  cmds c = {NULL, 0};
  char buffer[BUFF_SIZE];


  // Read all lines from input
  while (fgets(buffer, BUFF_SIZE, input)) {


    // Remove \n
    strtok(buffer, "\n");


    // Allocate memory for commands
    c.cmd = realloc(c.cmd, sizeof(char*) * (c.nCmd+1));
    if (c.cmd == NULL) {
      perror("Allocation failed");
      exit(EXIT_FAILURE);
    }
    c.cmd[c.nCmd] = malloc(sizeof(buffer));
    if (c.cmd[c.nCmd] == NULL) {
      perror("Allocation failed");
      exit(EXIT_FAILURE);
    } 


    // Copy over buffer to commands, increment nCmd
    if (strcpy(c.cmd[c.nCmd++], buffer) == NULL) {
      perror("String failed to copy");
      exit(EXIT_FAILURE);
    }  
  }
  

  return c;
}


/* execute function:
  Uses createPipes() to create pipes (filedescriptors for pipes).
  Forks once for each command in the struct c.
  Uses connectPipes() to connect the pipes in children processes.
  Gives every child a unique int value, ID.
  Uses parseLine() to split up the arguments in each row from c, in the child.
  Executes all the childs with the correct command and arguments.
  Closes all remaining filedescriptors and deallocates their memory.
  Waits for all children to complete the commands.

  Argument:
  The struct storing all commands and the amount of them.
*/
void execute(cmds c) {
  // Variables
  pid_t pid;
  int status;
  int child;
  int **fd = createPipes(c.nCmd-1);


  // Loop through all the commands
  for (child = 0; child < c.nCmd; child++) {


    // Create child
    pid = fork();
    if (pid < 0) {
      perror("Fork failed");
      exit(EXIT_FAILURE);
    }


    // If in child
    if (pid == 0) {
      
      // If there is more then one cmd, connect and close pipes
      if (c.nCmd > 1) {
        connectPipes(fd, child, c.nCmd);
      }

      // Break the loop so every child get an unique child value
      break;
    }
  }


  // Go into childs split up cmd and execute
  if (pid == 0 && c.nCmd != 0) {

    // Split up cmd into arg
    char **arg = parseLine(c.cmd[child]);

    // Execute
    if (execvp(arg[0], arg) < 0) {
      perror("Command could not be executed");
      exit(EXIT_FAILURE);
    }
  }


  // Close and deallocate all pipes, fd in parent
  for (int i = 0; i < c.nCmd-1; i++) {
    close(fd[i][READ_END]);
    close(fd[i][WRITE_END]);
    free(fd[i]);
  }
  free(fd);


  // Wait for all childs to finish
  for (int i = 0; i < c.nCmd; i++) {
    wait(&status);
    if (WEXITSTATUS(status) != 0) {
      perror("Child failed");
      exit(EXIT_FAILURE);
    }
  }
}


/* createPipes function:
Allocates memory for and creates n pipes.

Argument:
The amount of pipes to create, n.

Returns:
The filedescriptors to both ends of all pipes.
*/
int **createPipes(int n) {
  // Allocate space for filedescriptor array
  int **fd = malloc(sizeof(int*) * n);
  if (fd == NULL) {
    perror("Allocation failed");
    exit(EXIT_FAILURE);
  }


  // Create and allocate space for n pipes
  for (int i = 0; i < n; i++) {
    fd[i] = malloc(sizeof(int) * 2);
    if (fd[i] == NULL) {
      perror("Allocation failed");
      exit(EXIT_FAILURE);
    }
    if (pipe(fd[i]) < 0) {
      perror("Pipe creation failed");
      exit(EXIT_FAILURE);
    }
  }


  return fd;
}


/* connectPipes function:
Connects the pipes properly for one child and closes all other pipes.
Only meant to be called by children processes.

Arguments:
A filedescriptor array for pipes, fd.
The child ID of which to connect pipes to.
The total number of children processes.
*/
void connectPipes(int **fd, int child, int nChild) {
  // Close all pipes that definitly won't be used
  for (int i = 0; i < nChild-1; i++) {
    if (!(child == i || child == i+1)) {
      close(fd[i][READ_END]);
      close(fd[i][WRITE_END]);
    }
  }


  // Check if it's a fist, middle or last child, close and dup in pipes accordingly
  if (child == 0) { // First
    close(fd[child][READ_END]);
    if (dup2(fd[child][WRITE_END], STDOUT_FILENO) < 0) {
      perror("Filedescriptor duplication failed");
      exit(EXIT_FAILURE);
    }

  } else if (child == nChild-1) { // Last
    close(fd[child-1][WRITE_END]);
    if (dup2(fd[child-1][READ_END], STDIN_FILENO) < 0) {
      perror("Filedescriptor duplication failed");
      exit(EXIT_FAILURE); 
    }

  } else { // Middle
    close(fd[child-1][WRITE_END]);
    close(fd[child][READ_END]);
    if (dup2(fd[child-1][READ_END], STDIN_FILENO) < 0) {
      perror("Filedescriptor duplication failed");
      exit(EXIT_FAILURE);
    }
    if (dup2(fd[child][WRITE_END], STDOUT_FILENO) < 0) {
      perror("Filedescriptor duplication failed");
      exit(EXIT_FAILURE);
    }
  }
}


/* parseLine function:
Parses a string on " ", meaning it possibly results in multiple strings with no spaces.

Argument:
The line "string" to parse, in for om char pointer.

Returnes:
The parsed line, a char pointer pointer.
*/
char **parseLine(char *line) {
  // Variables
  char** parsed = NULL;
  int n = 0;


  // Loop through and split up line
  char *token = strtok(line, " ");
  while (token != NULL) {

    // Reallocate memory for parsed
    parsed = realloc(parsed, sizeof(char*) * (n+1));
    if (parsed == NULL) {
      perror("Allocation failed");
      exit(EXIT_FAILURE);
    }

    // Add token to parsed, increment n
    parsed[n++] = token;

    // Get a new token
    token = strtok(NULL, " ");
  }


  // Reallocate extra space for NULL, set as final element
  parsed = realloc(parsed, sizeof(char*) * (n+1));  
  if (parsed == NULL) {
    perror("Allocation failed");
    exit(EXIT_FAILURE);
  }
  parsed[n] = 0;


  return parsed;
}

