#include <stdio.h>

#include "builtins.h"
#include "executor.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char **argv)
{
    struct token_list tokens;
    struct command command;
    int result;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        return 1;
    }

    token_list_init(&tokens);
    command_init(&command);

    if (lexer_run(argv[1], &tokens) < 0)
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
