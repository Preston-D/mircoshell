/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "argparse.h"
#include "builtin.h"

/* PROTOTYPES */

void processline(char *line);
ssize_t getinput(char **line, size_t *size);

/* main
 * This function is the main entry point to the program.  This is essentially
 * the primary read-eval-print loop of the command interpreter.
 */

int main()
{
  char *line = NULL;
  size_t size = 0;
  ssize_t len;

  while ((len = getinput(&line, &size)) != -1)
  {
    processline(line);
  }

  free(line);
  return EXIT_SUCCESS;
}

/* getinput
 * line     A pointer to a char* that points at a buffer of size *size or NULL.
 * size     The size of the buffer *line or 0 if *line is NULL.
 * returns  The length of the string stored in *line.
 *
 * This function prompts the user for input, e.g., %myshell%.  If the input fits in the buffer
 * pointed to by *line, the input is placed in *line.  However, if there is not
 * enough room in *line, *line is freed and a new buffer of adequate space is
 * allocated.  The number of bytes allocated is stored in *size.
 * Hint: There is a standard i/o function that can make getinput easier than it sounds.
 */
ssize_t getinput(char **line, size_t *size)
{

  printf("myshell> ");
  return getline(line, size, stdin);
}

/* processline
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 * processline only forks when the line is not empty, and the line is not trying to run a built in command
 */
void processline(char *line)
{
  // Check whether the line is empty
  if (line[0] != '\0' && line[0] != '\n')
  {
    int argcp;
    char **args = argparse(line, &argcp);

    // Check whether arguments are built-in commands
    // If not built-in, fork to execute the command.
    if (!builtIn(args, argcp))
    {
      pid_t cpid = fork();
      if (cpid == -1)
      {
        perror("fork");
      }
      else if (cpid == 0)
      {
        // Child process
        if (execvp(args[0], args) == -1)
        {
          perror(args[0]);
        }
        exit(EXIT_FAILURE);
      }
      else
      {
        // Parent process
        int status;
        waitpid(cpid, &status, 0);
      }
    }

    // Free allocated memory
    for (int i = 0; i < argcp; i++)
    {
      free(args[i]);
    }
    free(args);
  }
}
