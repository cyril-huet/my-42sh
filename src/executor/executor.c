#include "executor.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void start_child(const struct command *command, int input_fd,
                        int output_fd)
{
    if (input_fd >= 0 && dup2(input_fd, STDIN_FILENO) < 0)
    {
        perror("my-42sh: dup2");
        exit(1);
    }

    if (output_fd >= 0 && dup2(output_fd, STDOUT_FILENO) < 0)
    {
        perror("my-42sh: dup2");
        exit(1);
    }

    if (input_fd >= 0)
    {
        close(input_fd);
    }

    if (output_fd >= 0)
    {
        close(output_fd);
    }

    execvp(command->arguments[0], command->arguments);

    perror(command->arguments[0]);
    exit(127);
}

static int wait_for_children(pid_t *children, int count)
{
    int result = 1;

    for (int i = 0; i < count; i++)
    {
        int status;

        if (waitpid(children[i], &status, 0) < 0)
        {
            perror("my-42sh: waitpid");
            continue;
        }

        if (i == count - 1 && WIFEXITED(status))
        {
            result = WEXITSTATUS(status);
        }
    }

    return result;
}

int execute_pipeline(const struct pipeline *pipeline)
{
    pid_t *children;
    int input_fd = -1;
    int child_count = 0;

    if (pipeline->count == 0)
    {
        return 0;
    }

    children = malloc(sizeof(pid_t) * pipeline->count);
    if (children == NULL)
    {
        perror("my-42sh: malloc");
        return 1;
    }

    for (int i = 0; i < pipeline->count; i++)
    {
        int pipe_fds[2] = {-1, -1};
        pid_t child;

        if (i < pipeline->count - 1 && pipe(pipe_fds) < 0)
        {
            perror("my-42sh: pipe");

            if (input_fd >= 0)
            {
                close(input_fd);
            }

            free(children);
            return 1;
        }

        child = fork();

        if (child < 0)
        {
            perror("my-42sh: fork");

            if (input_fd >= 0)
            {
                close(input_fd);
            }

            close(pipe_fds[0]);
            close(pipe_fds[1]);
            free(children);
            return 1;
        }

        if (child == 0)
        {
            close(pipe_fds[0]);
            start_child(&pipeline->commands[i], input_fd, pipe_fds[1]);
        }

        children[child_count] = child;
        child_count++;

        if (input_fd >= 0)
        {
            close(input_fd);
        }

        if (i < pipeline->count - 1)
        {
            close(pipe_fds[1]);
            input_fd = pipe_fds[0];
        }
    }

    if (input_fd >= 0)
    {
        close(input_fd);
    }

    int result = wait_for_children(children, child_count);

    free(children);
    return result;
}