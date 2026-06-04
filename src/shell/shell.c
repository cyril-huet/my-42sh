#include "shell.h"

#include <stdio.h>

#include "builtins.h"
#include "executor.h"
#include "lexer.h"
#include "parser.h"

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

int shell_run_line(const char *line)
{
    struct token_list tokens;
    struct command command;
    int result;

    token_list_init(&tokens);
    command_init(&command);

    if (lexer_run(line, &tokens) < 0)
    {
        fprintf(stderr, "my-42sh: lexer error\n");
        token_list_free(&tokens);
        return 1;
    }

    if (parser_run(&tokens, &command) < 0)
    {
        fprintf(stderr, "my-42sh: parser error\n");
        token_list_free(&tokens);
        return 1;
    }

    if (is_builtin(&command) == 1)
    {
        result = execute_builtin(&command);
    }
    else
    {
        result = execute_command(&command);
    }

    command_free(&command);
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