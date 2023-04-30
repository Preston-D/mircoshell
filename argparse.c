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

  while (*line != '\0')
  {
    if (isspace(*line))
    {
      in_word = FALSE;
    }
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
  *argcp = argCount(line);
  char **args = (char **)malloc((*argcp + 1) * sizeof(char *));

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
    args[i] = (char *)malloc((arg_len + 1) * sizeof(char));
    strncpy(args[i], line, arg_len);
    args[i][arg_len] = '\0';
    line += arg_len;
  }
  args[*argcp] = NULL;
  return args;
}
