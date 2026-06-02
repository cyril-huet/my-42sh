#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int execute_command(const struct command *command)
{
    int status;
    pid_t child;

    if (command->count == 0)
    {
        return 0;
    }

    child = fork();

    if (child < 0)
    {
        perror("my-42sh: fork");
        return 1;
    }

    if (child == 0)
    {
        execvp(command->arguments[0], command->arguments);
        perror(command->arguments[0]);
        exit(127);
    }

    if (waitpid(child, &status, 0) < 0)
    {
        perror("my-42sh: waitpid");
        return 1;
    }

    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }

    return 1;
}
