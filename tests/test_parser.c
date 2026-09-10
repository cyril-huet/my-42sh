#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

static int parse_line(const char *line, struct token_list *tokens,
                      struct command_list *commands)
{
    token_list_init(tokens);
    command_list_init(commands);

    if (lexer_run(line, tokens) < 0)
    {
        return -1;
    }

    return parser_run(tokens, commands);
}

static int test_simple_command(void)
{
    struct token_list tokens;
    struct command_list commands;
    struct command *command;
    int result;

    if (parse_line("echo hello", &tokens, &commands) < 0)
    {
        return 0;
    }

    command = &commands.pipelines[0].commands[0];

    result = commands.count == 1 && commands.pipelines[0].count == 1
        && command->count == 2 && strcmp(command->arguments[0], "echo") == 0
        && strcmp(command->arguments[1], "hello") == 0;

    token_list_free(&tokens);
    command_list_free(&commands);
    return result;
}

static int test_pipeline(void)
{
    struct token_list tokens;
    struct command_list commands;
    int result;

    if (parse_line("echo hello | wc -w", &tokens, &commands) < 0)
    {
        return 0;
    }

    result = commands.count == 1 && commands.pipelines[0].count == 2
        && commands.pipelines[0].commands[0].count == 2
        && commands.pipelines[0].commands[1].count == 2;

    token_list_free(&tokens);
    command_list_free(&commands);
    return result;
}

static int test_redirections(void)
{
    struct token_list tokens;
    struct command_list commands;
    struct command *command;
    int result;

    if (parse_line("cat < input > output >> log", &tokens, &commands) < 0)
    {
        return 0;
    }

    command = &commands.pipelines[0].commands[0];

    result = command->redirection_count == 3
        && command->redirections[0].type == REDIRECTION_INPUT
        && command->redirections[1].type == REDIRECTION_OUTPUT
        && command->redirections[2].type == REDIRECTION_APPEND
        && strcmp(command->redirections[0].file, "input") == 0
        && strcmp(command->redirections[1].file, "output") == 0
        && strcmp(command->redirections[2].file, "log") == 0;

    token_list_free(&tokens);
    command_list_free(&commands);
    return result;
}

static int test_logical_operators(void)
{
    struct token_list tokens;
    struct command_list commands;
    int result;

    if (parse_line("true && echo yes || echo no; pwd", &tokens, &commands) < 0)
    {
        return 0;
    }

    result = commands.count == 4 && commands.operator_count == 3
        && commands.operators[0] == LOGICAL_AND
        && commands.operators[1] == LOGICAL_OR
        && commands.operators[2] == LOGICAL_SEQUENCE;

    token_list_free(&tokens);
    command_list_free(&commands);
    return result;
}

static int test_background(void)
{
    struct token_list tokens;
    struct command_list commands;
    int result;

    if (parse_line("sleep 1 & echo done", &tokens, &commands) < 0)
    {
        return 0;
    }

    result = commands.count == 2 && commands.pipelines[0].background == 1
        && commands.operator_count == 1
        && commands.operators[0] == LOGICAL_SEQUENCE;

    token_list_free(&tokens);
    command_list_free(&commands);
    return result;
}

static int parser_rejects(const char *line)
{
    struct token_list tokens;
    struct command_list commands;
    int result;

    token_list_init(&tokens);
    command_list_init(&commands);

    if (lexer_run(line, &tokens) < 0)
    {
        token_list_free(&tokens);
        command_list_free(&commands);
        return 1;
    }

    result = parser_run(&tokens, &commands) < 0;

    token_list_free(&tokens);
    command_list_free(&commands);
    return result;
}

static int test_invalid_commands(void)
{
    return parser_rejects("| echo hello") && parser_rejects("echo hello |")
        && parser_rejects("echo hello >") && parser_rejects("echo hello &&");
}

static void print_result(const char *name, int result)
{
    if (result == 1)
    {
        printf("[OK] %s\n", name);
    }
    else
    {
        printf("[FAIL] %s\n", name);
    }
}

int main(void)
{
    int failed = 0;
    int result;

    result = test_simple_command();
    print_result("simple command", result);
    failed += result == 0;

    result = test_pipeline();
    print_result("pipeline", result);
    failed += result == 0;

    result = test_redirections();
    print_result("redirections", result);
    failed += result == 0;

    result = test_logical_operators();
    print_result("logical operators", result);
    failed += result == 0;

    result = test_background();
    print_result("background command", result);
    failed += result == 0;

    result = test_invalid_commands();
    print_result("invalid commands", result);
    failed += result == 0;

    if (failed != 0)
    {
        return 1;
    }

    return 0;
}