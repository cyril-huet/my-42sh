#include "shell.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "executor.h"
#include "lexer.h"
#include "parser.h"
#include "variables.h"

#define MAX_LINE_LENGTH 4096

static void handle_sigint(int signal_number)
{
    (void)signal_number;
    write(STDOUT_FILENO, "\n", 1);
}

static void install_signal_handlers(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}

static void remove_newline(char *line)
{
    int index = 0;

    while (line[index] != '\0')
    {
        if (line[index] == '\n')
        {
            line[index] = '\0';
            return;
        }

        index++;
    }
}

static void check_background_process(struct shell_state *state)
{
    int status;

    if (state->last_background <= 0)
    {
        return;
    }

    if (waitpid(state->last_background, &status, WNOHANG)
        == state->last_background)
    {
        state->last_background = 0;
    }
}

static int execute_one_pipeline(struct pipeline *pipeline)
{
    if (pipeline->count == 1 && pipeline->commands[0].redirection_count == 0
        && is_builtin(&pipeline->commands[0]) == 1)
    {
        return execute_builtin(&pipeline->commands[0]);
    }

    return execute_pipeline(pipeline);
}
static int find_subshell_bounds(const char *line, int *start, int *end)
{
    int length = 0;

    while (line[length] != '\0')
    {
        length++;
    }

    *start = 0;
    while (*start < length && (line[*start] == ' ' || line[*start] == '\t'))
    {
        (*start)++;
    }

    *end = length - 1;
    while (*end >= *start && (line[*end] == ' ' || line[*end] == '\t'))
    {
        (*end)--;
    }

    if (*start >= *end)
    {
        return 0;
    }

    if (line[*start] != '(' || line[*end] != ')')
    {
        return 0;
    }

    return 1;
}
static char *copy_subshell_line(const char *line, int start, int end)
{
    int length = end - start - 1;
    char *inside = malloc(sizeof(char) * (length + 1));

    if (inside == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        inside[i] = line[start + 1 + i];
    }

    inside[length] = '\0';
    return inside;
}
static int run_subshell(const char *line, struct shell_state *state, int start,
                        int end)
{
    char *inside;
    pid_t child;
    int status;

    inside = copy_subshell_line(line, start, end);
    if (inside == NULL)
    {
        fprintf(stderr, "my-42sh: memory allocation failed\n");
        return 1;
    }

    child = fork();

    if (child < 0)
    {
        perror("my-42sh: fork");
        free(inside);
        return 1;
    }

    if (child == 0)
    {
        struct shell_state child_state = *state;
        int result = shell_run_line(inside, &child_state);

        free(inside);
        exit(result);
    }

    free(inside);

    if (waitpid(child, &status, 0) < 0)
    {
        perror("my-42sh: waitpid");
        return 1;
    }

    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status))
    {
        return 128 + WTERMSIG(status);
    }

    return 1;
}
int shell_run_line(const char *line, struct shell_state *state)
{
    struct token_list tokens;
    struct command_list commands;
    int start;
    int end;
    int result = 0;

    if (find_subshell_bounds(line, &start, &end) == 1)
    {
        return run_subshell(line, state, start, end);
    }

    token_list_init(&tokens);
    command_list_init(&commands);

    if (lexer_run(line, &tokens) < 0)
    {
        fprintf(stderr, "my-42sh: lexer error\n");
        token_list_free(&tokens);
        state->last_status = 1;
        return 1;
    }

    if (expand_token_list(&tokens, state) < 0)
    {
        fprintf(stderr, "my-42sh: variable expansion error\n");
        token_list_free(&tokens);
        state->last_status = 1;
        return 1;
    }

    if (parser_run(&tokens, &commands) < 0)
    {
        fprintf(stderr, "my-42sh: parser error\n");
        token_list_free(&tokens);
        state->last_status = 1;
        return 1;
    }

    for (int i = 0; i < commands.count; i++)
    {
        struct pipeline *pipeline = &commands.pipelines[i];

        if (pipeline->background == 1)
        {
            result =
                execute_pipeline_background(pipeline, &state->last_background);
        }
        else
        {
            result = execute_one_pipeline(pipeline);
        }

        if (result == 2)
        {
            break;
        }

        if (i == commands.count - 1)
        {
            break;
        }

        if (commands.operators[i] == LOGICAL_AND && result != 0)
        {
            break;
        }

        if (commands.operators[i] == LOGICAL_OR && result == 0)
        {
            break;
        }
    }

    state->last_status = result;

    command_list_free(&commands);
    token_list_free(&tokens);

    return result;
}

int shell_run(struct shell_state *state)
{
    char line[MAX_LINE_LENGTH];
    int result = 0;

    install_signal_handlers();

    while (1)
    {
        check_background_process(state);

        printf("my-42sh$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            if (errno == EINTR)
            {
                clearerr(stdin);
                continue;
            }

            printf("\n");
            break;
        }

        remove_newline(line);

        if (line[0] == '\0')
        {
            continue;
        }

        result = shell_run_line(line, state);

        if (result == 2)
        {
            break;
        }
    }

    return result;
}