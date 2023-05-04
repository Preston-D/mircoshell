#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FALSE (0)
#define TRUE (1)

/*
 * argCount is a helper function that takes in a String and returns the number of "words" in the string assuming that whitespace is the only possible delimeter.
 */
static int argCount(char *line)
{
  int count = 0;
  int in_word = FALSE;

  // Iterate through the characters in the input string updating the counter and the in_word flag.
  while (*line != '\0')
  {
    // whitespace
    if (isspace(*line))
    {
      in_word = FALSE;
    }
    // non-whitespace
    else
    {
      if (!in_word)
      {
        count++;
      }
      in_word = TRUE;
    }
    line++;
  }
  return count;
}

/*
 *
 * Argparse takes in a String and returns an array of strings from the input.
 * The arguments in the String are broken up by whitespaces in the string.
 * The count of how many arguments there are is saved in the argcp pointer
 */
char **argparse(char *line, int *argcp)
{
  // Determine the number of arguments in the input string.
  *argcp = argCount(line);
  // Allocates memory for an array of pointers to strings (args), with one additional element to store a NULL pointer at the end.
  char **args = (char **)malloc((*argcp + 1) * sizeof(char *));

  // Iterate through the input string, skipping any leading whitespace characters.
  for (int i = 0; i < *argcp; i++)
  {
    while (isspace(*line))
    {
      line++;
    }
    int arg_len = 0;
    while (line[arg_len] != '\0' && !isspace(line[arg_len]))
    {
      arg_len++;
    }
    // Calculates the length of the argument, allocates memory for the argument.
    args[i] = (char *)malloc((arg_len + 1) * sizeof(char));
    // Copies the argument's characters into the allocated memory.
    strncpy(args[i], line, arg_len);
    args[i][arg_len] = '\0';
    line += arg_len;
  }
  // Set the last element of the args array to NULL and return the array.
  args[*argcp] = NULL;
  return args;
}
