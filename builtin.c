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
#include <dirent.h>
#include <time.h>
#include <utime.h>

#define MAX_BUFFER 1024

extern char **environ;

// Prototypes
static void exitProgram(char **args, int argcp);
static void cd(char **args, int argpcp);
static void pwd(char **args, int argcp);

static void ls(char **args, int argcp);
static void cp(char **args, int argcp);
static void touch(char **args, int argcp);
static void statFunc(char **args, int argcp);
static void tail(char **args, int argcp);
static void env(char **args, int argcp);

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
  else if (strcmp(args[0], "ls") == 0)
  {
    ls(args, argcp);
    result = 1;
  }
  else if (strcmp(args[0], "cp") == 0)
  {
    cp(args, argcp);
    result = 1;
  }
  else if (strcmp(args[0], "touch") == 0)
  {
    touch(args, argcp);
    result = 1;
  }
  else if (strcmp(args[0], "stat") == 0)
  {
    statFunc(args, argcp);
    result = 1;
  }
  else if (strcmp(args[0], "tail") == 0)
  {
    tail(args, argcp);
    result = 1;
  }
  else if (strcmp(args[0], "env") == 0)
  {
    env(args, argcp);
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

static void ls(char **args, int argcp)
{
  // Check if number of arguments is valid
  if (argcp > 2)
  {
    fprintf(stderr, "ls: too many arguments\n");
    return;
  }

  int long_format = (argcp == 2 && strcmp(args[1], "-l") == 0);

  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (long_format)
      {
        struct stat file_stat;
        if (stat(dir->d_name, &file_stat) == -1)
        {
          perror("ls -l");
          continue;
        }
        printf("%llu %lld %ld %s\n", file_stat.st_ino, file_stat.st_size, file_stat.st_mtime, dir->d_name);
      }
      else
      {
        printf("%s\n", dir->d_name);
      }
    }
    closedir(d);
  }
}

static void cp(char **args, int argcp)
{
  // Check if number of arguments is valid
  if (argcp != 3)
  {
    fprintf(stderr, "cp: incorrect number of arguments\n");
    return;
  }

  char *sourceFile = args[1];
  char *destinationFile = args[2];

  int sourceFD, destFD;
  char buffer[MAX_BUFFER];
  ssize_t bytes;

  // Open source file
  sourceFD = open(sourceFile, O_RDONLY);
  if (sourceFD == -1)
  {
    perror("cp: cannot open source file");
    return;
  }

  // Open destination file
  destFD = open(destinationFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (destFD == -1)
  {
    perror("cp: cannot open destination file");
    close(sourceFD);
    return;
  }

  // Copy data from source file to destination file
  while ((bytes = read(sourceFD, buffer, MAX_BUFFER)) > 0)
  {
    if (write(destFD, buffer, bytes) != bytes)
    {
      perror("cp: write error");
      break;
    }
  }

  if (bytes == -1)
  {
    perror("cp: read error");
  }

  close(sourceFD);
  close(destFD);
}

static void touch(char **args, int argcp)
{
  // Check if number of arguments is valid
  if (argcp != 2)
  {
    fprintf(stderr, "touch: incorrect number of arguments\n");
    return;
  }

  char *file = args[1];

  // Open the file with the O_CREAT flag to create it if it does not exist
  int fd = open(file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd == -1)
  {
    perror("touch: cannot open file");
    return;
  }

  close(fd);

  // Update the access and modification times of the file to the current time
  struct utimbuf newTimes;
  newTimes.actime = time(NULL);
  newTimes.modtime = time(NULL);

  if (utime(file, &newTimes) == -1)
  {
    perror("touch: cannot update file times");
  }
}

// static void statCommand(char **args, int argcp)
// {
//   // Check if number of arguments is valid
//   if (argcp != 2)
//   {
//     fprintf(stderr, "stat: incorrect number of arguments\n");
//     return;
//   }

//   char *file = args[1];
//   struct stat fileStat;

//   if (stat(file, &fileStat) == -1)
//   {
//     perror("stat");
//     return;
//   }

//   printf("File: %s\n", file);
//   printf("Size: %lld\n", fileStat.st_size);
//   printf("Inode: %ld\n", fileStat.st_ino);
//   printf("Access time: %ld\n", fileStat.st_atime);
//   printf("Modification time: %ld\n", fileStat.st_mtime);
//   printf("Change time: %ld\n", fileStat.st_ctime);
// }

static void statFunc(char **args, int argcp)
{
  struct stat fileStat;
  for (int i = 1; i < argcp; i++)
  {
    if (stat(args[i], &fileStat) < 0)
    {
      printf("Cannot perform stat on the file/directory: %s\n", args[i]);
      continue;
    }
    printf("Information for %s\n", args[i]);
    printf("---------------------------\n");
    printf("File Size: \t\t%lld bytes\n", fileStat.st_size);
    printf("Number of Links: \t%hu\n", fileStat.st_nlink);
    printf("File inode: \t\t%llu\n", fileStat.st_ino);

    printf("File Permissions: \t");
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    printf("File Owner ID: \t%d\n", fileStat.st_uid);
    printf("File Group ID: \t%d\n", fileStat.st_gid);
    printf("Last file access: \t%s", ctime(&fileStat.st_atime));
    printf("Last file modification: \t%s", ctime(&fileStat.st_mtime));
  }
}

static void tail(char **args, int argcp)
{
  // Check if number of arguments is valid
  if (argcp != 2)
  {
    fprintf(stderr, "tail: incorrect number of arguments\n");
    return;
  }

  char *file = args[1];
  FILE *fp = fopen(file, "r");
  if (fp == NULL)
  {
    perror("tail: cannot open file");
    return;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  // Use a circular buffer to hold the last 10 lines
  char *lines[10] = {NULL};
  int current = 0;

  while ((read = getline(&line, &len, fp)) != -1)
  {
    if (lines[current] != NULL)
      free(lines[current]);
    lines[current] = malloc(sizeof(char) * (read + 1));
    strncpy(lines[current], line, read);
    lines[current][read] = '\0';
    current = (current + 1) % 10;
  }

  // Print the last 10 lines
  for (int i = 0; i < 10; ++i)
  {
    current = (current + 1) % 10;
    if (lines[current] != NULL)
    {
      printf("%s", lines[current]);
      free(lines[current]);
    }
  }

  if (line)
    free(line);
  fclose(fp);
}

// This function is called when the "env" command is entered.
// It takes two arguments: args, a pointer to an array of command-line arguments,
// and argcp, an integer representing the number of arguments.
static void env(char **args, int argcp)
{
  if (argcp > 2)
  {
    fprintf(stderr, "env: too many arguments\n");
    return;
  }

  if (argcp == 1)
  {
    // Print all environment variables
    char **env = environ;
    while (*env)
    {
      printf("%s\n", *env);
      env++;
    }
  }
  else
  {
    // Set a new environment variable
    char *name = strtok(args[1], "=");
    char *value = strtok(NULL, "=");
    if (value == NULL)
    {
      fprintf(stderr, "env: incorrect argument format\n");
      return;
    }
    if (setenv(name, value, 1) == -1)
    {
      perror("env");
    }
  }
}
