#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
  // Display the username
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    printf("%s\n", pw->pw_name);
  }
  else
  {
    printf("Failed to get username.\n");
  }

  // Search for the word "include" in the file activity.c
  FILE *file = fopen("activity.c", "r");
  if (file == NULL)
  {
    printf("Failed to open the file.\n");
    return 1;
  }

  char line[1024];
  const char *word = "include";

  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, word) != NULL)
    {
      printf("%s", line);
    }
  }

  fclose(file);
  return 0;
}
