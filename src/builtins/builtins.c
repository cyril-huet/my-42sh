#define _POSIX_C_SOURCE 200809L

#include "builtins.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
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

static int is_name_start(char character)
{
    if (character >= 'a' && character <= 'z')
    {
        return 1;
    }

    if (character >= 'A' && character <= 'Z')
    {
        return 1;
    }

    if (character == '_')
    {
        return 1;
    }

    return 0;
}

static int is_name_character(char character)
{
    if (is_name_start(character) == 1)
    {
        return 1;
    }

    if (character >= '0' && character <= '9')
    {
        return 1;
    }

    return 0;
}

static char *copy_part(const char *text, int length)
{
    char *copy = malloc(sizeof(char) * (length + 1));

    if (copy == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        copy[i] = text[i];
    }

    copy[length] = '\0';
    return copy;
}

static int valid_name(const char *name)
{
    int index = 0;

    if (is_name_start(name[0]) == 0)
    {
        return 0;
    }

    while (name[index] != '\0')
    {
        if (is_name_character(name[index]) == 0)
        {
            return 0;
        }

        index++;
    }

    return 1;
}

static int find_equal(const char *text)
{
    int index = 0;

    while (text[index] != '\0')
    {
        if (text[index] == '=')
        {
            return index;
        }

        index++;
    }

    return -1;
}

static int run_echo(const struct command *command)
{
    int first_argument = 1;
    int newline = 1;

    if (command->count > 1
        && same_text(command->arguments[1], "-n") == 1)
    {
        first_argument = 2;
        newline = 0;
    }

    for (int i = first_argument; i < command->count; i++)
    {
        if (i > first_argument)
        {
            printf(" ");
        }

        printf("%s", command->arguments[i]);
    }

    if (newline == 1)
    {
        printf("\n");
    }

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

static int update_pwd(void)
{
    char path[PATH_MAX];

    if (getcwd(path, sizeof(path)) == NULL)
    {
        perror("my-42sh: cd");
        return 1;
    }

    if (setenv("PWD", path, 1) < 0)
    {
        perror("my-42sh: cd");
        return 1;
    }

    return 0;
}

static int run_cd(const struct command *command)
{
    const char *path;

    if (command->count > 2)
    {
        fprintf(stderr, "my-42sh: cd: too many arguments\n");
        return 1;
    }

    if (command->count == 1)
    {
        path = getenv("HOME");

        if (path == NULL)
        {
            fprintf(stderr, "my-42sh: cd: HOME is not set\n");
            return 1;
        }
    }
    else
    {
        path = command->arguments[1];
    }

    if (chdir(path) < 0)
    {
        perror("my-42sh: cd");
        return 1;
    }

    return update_pwd();
}

static int run_export(const struct command *command)
{
    if (command->count == 1)
    {
        return run_env();
    }

    for (int i = 1; i < command->count; i++)
    {
        int equal = find_equal(command->arguments[i]);
        int name_length = equal;
        char *name;
        const char *value = "";

        if (equal < 0)
        {
            name_length = 0;

            while (command->arguments[i][name_length] != '\0')
            {
                name_length++;
            }
        }

        name = copy_part(command->arguments[i], name_length);
        if (name == NULL)
        {
            fprintf(stderr, "my-42sh: export: memory allocation failed\n");
            return 1;
        }

        if (valid_name(name) == 0)
        {
            fprintf(stderr, "my-42sh: export: invalid variable name\n");
            free(name);
            return 1;
        }

        if (equal >= 0)
        {
            value = command->arguments[i] + equal + 1;
        }

        if (setenv(name, value, 1) < 0)
        {
            perror("my-42sh: export");
            free(name);
            return 1;
        }

        free(name);
    }

    return 0;
}

static int run_unset(const struct command *command)
{
    for (int i = 1; i < command->count; i++)
    {
        if (valid_name(command->arguments[i]) == 0)
        {
            fprintf(stderr, "my-42sh: unset: invalid variable name\n");
            return 1;
        }

        if (unsetenv(command->arguments[i]) < 0)
        {
            perror("my-42sh: unset");
            return 1;
        }
    }

    return 0;
}

static int run_exit(void)
{
    return 2;
}

int is_builtin(const struct command *command)
{
    if (command->count == 0)
    {
        return 0;
    }

    if (same_text(command->arguments[0], "echo") == 1
        || same_text(command->arguments[0], "pwd") == 1
        || same_text(command->arguments[0], "env") == 1
        || same_text(command->arguments[0], "cd") == 1
        || same_text(command->arguments[0], "export") == 1
        || same_text(command->arguments[0], "unset") == 1
        || same_text(command->arguments[0], "exit") == 1)
    {
        return 1;
    }

    return 0;
}

int is_exit_builtin(const struct command *command)
{
    if (command->count == 0)
    {
        return 0;
    }

    return same_text(command->arguments[0], "exit");
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

    if (same_text(command->arguments[0], "cd") == 1)
    {
        return run_cd(command);
    }

    if (same_text(command->arguments[0], "export") == 1)
    {
        return run_export(command);
    }

    if (same_text(command->arguments[0], "unset") == 1)
    {
        return run_unset(command);
    }

    if (same_text(command->arguments[0], "exit") == 1)
    {
        return run_exit();
    }

    return 1;
}