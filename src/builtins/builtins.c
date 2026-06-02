#define _POSIX_C_SOURCE 200809L

#include "builtins.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef PATH_MAX
#    define PATH_MAX 4096
#endif

extern char **environ;

static int same_text(const char *first, const char *second)
{
    int index = 0;

    while (first[index] != '\0' && second[index] != '\0')
    {
        if (first[index] != second[index])
        {
            return 0;
        }

        index++;
    }

    if (first[index] == '\0' && second[index] == '\0')
    {
        return 1;
    }

    return 0;
}

static int run_echo(const struct command *command)
{
    for (int i = 1; i < command->count; i++)
    {
        if (i > 1)
        {
            printf(" ");
        }

        printf("%s", command->arguments[i]);
    }

    printf("\n");
    return 0;
}

static int run_pwd(void)
{
    char path[PATH_MAX];

    if (getcwd(path, sizeof(path)) == NULL)
    {
        perror("my-42sh: pwd");
        return 1;
    }

    printf("%s\n", path);
    return 0;
}

static int run_env(void)
{
    int index = 0;

    while (environ[index] != NULL)
    {
        printf("%s\n", environ[index]);
        index++;
    }

    return 0;
}

int is_builtin(const struct command *command)
{
    if (command->count == 0)
    {
        return 0;
    }

    if (same_text(command->arguments[0], "echo") == 1)
    {
        return 1;
    }

    if (same_text(command->arguments[0], "pwd") == 1)
    {
        return 1;
    }

    if (same_text(command->arguments[0], "env") == 1)
    {
        return 1;
    }

    return 0;
}

int execute_builtin(const struct command *command)
{
    if (same_text(command->arguments[0], "echo") == 1)
    {
        return run_echo(command);
    }

    if (same_text(command->arguments[0], "pwd") == 1)
    {
        return run_pwd();
    }

    if (same_text(command->arguments[0], "env") == 1)
    {
        return run_env();
    }

    return 1;
}
