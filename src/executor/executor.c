#include "executor.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int open_redirection(const struct redirection *redirection)
{
    if (redirection->type == REDIRECTION_INPUT)
    {
        return open(redirection->file, O_RDONLY);
    }

    if (redirection->type == REDIRECTION_OUTPUT)
    {
        return open(redirection->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    return open(redirection->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
}

static int apply_redirections(const struct command *command)
{
    for (int i = 0; i < command->redirection_count; i++)
    {
        int file;
        int output;

        file = open_redirection(&command->redirections[i]);
        if (file < 0)
        {
            perror(command->redirections[i].file);
            return -1;
        }

        if (command->redirections[i].type == REDIRECTION_INPUT)
        {
            output = STDIN_FILENO;
        }
        else
        {
            output = STDOUT_FILENO;
        }

        if (dup2(file, output) < 0)
        {
            perror("my-42sh: dup2");
            close(file);
            return -1;
        }

        close(file);
    }

    return 0;
}

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

    if (apply_redirections(command) < 0)
    {
        exit(1);
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
        int pipe_fds[2] = { -1, -1 };
        pid_t child;

        if (i < pipeline->count - 1 && pipe(pipe_fds) < 0)
        {
            perror("my-42sh: pipe");
            free(children);
            return 1;
        }

        child = fork();

        if (child < 0)
        {
            perror("my-42sh: fork");
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