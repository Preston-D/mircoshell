// argparse.c
// This file parses arguments sent to myshell
// It is called in myshell.c to parse inputs.
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

// argCount is a helper function that takes in a String and returns the number of "words" in the string,
// assuming that whitespace is the only possible delimiter.
static int argCount(char *line)
{
  int count = 0;
  int in_word = FALSE;

  if (line != NULL)
  {
    // Iterate through the characters in the string
    while (*line != '\0')
    {
      // If the current character is a whitespace, set in_word to FALSE
      if (isspace(*line))
      {
        in_word = FALSE;
      }
      else
      {
        // If not in a word, increment the count and set in_word to TRUE
        if (!in_word)
        {
          count++;
        }
        in_word = TRUE;
      }
      line++;
    }
  }

  return count;
}

// Argparse takes in a String and returns an array of strings from the input.
// The arguments in the String are broken up by whitespaces in the string.
// The count of how many arguments there are is saved in the argcp pointer.
char **argparse(char *line, int *argcp)
{
  char **args = NULL;

  // Check if both input pointers (line and argcp) are not NULL before processing
  if (line != NULL && argcp != NULL)
  {
    *argcp = argCount(line);
    // Allocate memory for the argument array, with an additional element for the NULL terminator
    args = (char **)malloc((*argcp + 1) * sizeof(char *));

    // If memory allocation is successful, process the arguments
    if (args != NULL)
    {
      // Iterate through the arguments
      for (int i = 0; i < *argcp; i++)
      {
        // Skip leading whitespaces
        while (isspace(*line))
        {
          line++;
        }

        // Find the length of the current argument
        int arg_len = 0;
        while (line[arg_len] != '\0' && !isspace(line[arg_len]))
        {
          arg_len++;
        }

        // Allocate memory for the current argument string, including space for the NULL terminator
        args[i] = (char *)malloc((arg_len + 1) * sizeof(char));

        // If memory allocation is successful, copy the argument string and add the NULL terminator
        if (args[i] != NULL)
        {
          strncpy(args[i], line, arg_len);
          args[i][arg_len] = '\0';
        }

        // Move the line pointer to the next argument
        line += arg_len;
      }

      // Add the NULL terminator to the argument array
      args[*argcp] = NULL;
    }
  }
  else
  {
    // If argcp is not NULL, set it to 0 to indicate an error
    if (argcp != NULL)
    {
      *argcp = 0;
    }
  }

  return args;
}
