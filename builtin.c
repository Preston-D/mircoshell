#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include "builtin.h"
#include <string.h>
#include <dirent.h>

// Prototypes
static void exitProgram(char **args, int argcp);
static void cd(char **args, int argpcp);
static void pwd(char **args, int argcp);

/* builtIn
 ** built in checks each built in command the given command, if the given command
 * matches one of the built in commands, that command is called and builtin returns 1.
 *If none of the built in commands match the wanted command, builtin returns 0;
 */
int builtIn(char **args, int argcp)
{
  int result = 0;

  if (strcmp(args[0], "exit") == 0)
  {
    exitProgram(args, argcp);
    result = 1;
  }
  else if (strcmp(args[0], "pwd") == 0)
  {
    pwd(args, argcp);
    result = 1;
  }
  else if (strcmp(args[0], "cd") == 0)
  {
    cd(args, argcp);
    result = 1;
  }

  return result;
}

// This function is called when the "exit" command is entered.
// It takes two arguments: args, a pointer to an array of command-line arguments,
// and argcp, an integer representing the number of arguments.
static void exitProgram(char **args, int argcp)
{
  int status = 0;
  // Convert the second argument to an integer using atoi() and use it as the exit status.
  if (argcp > 1)
  {
    status = atoi(args[1]);
  }
  exit(status);
}

// This function is called when the "pwd" command is entered.
// It takes two arguments: args, a pointer to an array of command-line arguments,
// and argcp, an integer representing the number of arguments.
static void pwd(char **args, int argpc)
{
  if (argpc > 1)
  {
    fprintf(stderr, "pwd: too many arguments\n");
    return;
  }
  char cwd[PATH_MAX];
  // Get the current working directory and prints it.
  if (getcwd(cwd, sizeof(cwd)) != NULL)
  {
    printf("%s\n", cwd);
  }
  else
  {
    perror("pwd");
  }
}

// This function is called when the "cd" command is entered.
// It takes two arguments: args, a pointer to an array of command-line arguments,
// and argcp, an integer representing the number of arguments.
static void cd(char **args, int argcp)
{
  if (argcp > 2)
  {
    fprintf(stderr, "cd: too many arguments\n");
    return;
  }
  const char *path = argcp == 1 ? getenv("HOME") : args[1];
  // Change the current working directory to the specified path.
  if (chdir(path) == -1)
  {
    perror("cd");
  }
}
