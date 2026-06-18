#include "shell.h"

#include <stdio.h>

#include "builtins.h"
#include "executor.h"
#include "lexer.h"
#include "parser.h"
#include "variables.h"

#define MAX_LINE_LENGTH 4096

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

static int execute_one_pipeline(struct pipeline *pipeline)
{
    if (pipeline->count == 1
        && pipeline->commands[0].redirection_count == 0
        && is_builtin(&pipeline->commands[0]) == 1)
    {
        return execute_builtin(&pipeline->commands[0]);
    }

    return execute_pipeline(pipeline);
}

int shell_run_line(const char *line)
{
    struct token_list tokens;
    struct command_list commands;
    int result = 0;

    token_list_init(&tokens);
    command_list_init(&commands);

    if (lexer_run(line, &tokens) < 0)
    {
        fprintf(stderr, "my-42sh: lexer error\n");
        token_list_free(&tokens);
        return 1;
    }

    if (expand_token_list(&tokens) < 0)
    {
        fprintf(stderr, "my-42sh: variable expansion error\n");
        token_list_free(&tokens);
        return 1;
    }

    if (parser_run(&tokens, &commands) < 0)
    {
        fprintf(stderr, "my-42sh: parser error\n");
        token_list_free(&tokens);
        return 1;
    }

    for (int i = 0; i < commands.count; i++)
    {
        result = execute_one_pipeline(&commands.pipelines[i]);

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

    command_list_free(&commands);
    token_list_free(&tokens);

    return result;
}

int shell_run(void)
{
    char line[MAX_LINE_LENGTH];
    int result = 0;

    while (1)
    {
        printf("my-42sh$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        remove_newline(line);

        if (line[0] == '\0')
        {
            continue;
        }

        result = shell_run_line(line);

        if (result == 2)
        {
            break;
        }
    }

    return result;
}