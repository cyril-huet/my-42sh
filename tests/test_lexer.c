#include <stdio.h>
#include <string.h>

#include "lexer.h"

static int token_is(const struct token_list *list, int index, int type,
                    const char *value)
{
    if (index >= list->count)
    {
        return 0;
    }

    if (list->items[index].type != type)
    {
        return 0;
    }

    if (strcmp(list->items[index].value, value) != 0)
    {
        return 0;
    }

    return 1;
}

static int test_words(void)
{
    struct token_list list;
    int result;

    token_list_init(&list);

    if (lexer_run("echo hello", &list) < 0)
    {
        token_list_free(&list);
        return 0;
    }

    result = list.count == 2 && token_is(&list, 0, TOKEN_WORD, "echo")
        && token_is(&list, 1, TOKEN_WORD, "hello");

    token_list_free(&list);
    return result;
}

static int test_quotes(void)
{
    struct token_list list;
    int result;

    token_list_init(&list);

    if (lexer_run("echo \"hello world\" 'test'", &list) < 0)
    {
        token_list_free(&list);
        return 0;
    }

    result = list.count == 3 && token_is(&list, 0, TOKEN_WORD, "echo")
        && token_is(&list, 1, TOKEN_WORD, "hello world")
        && token_is(&list, 2, TOKEN_WORD, "test");

    token_list_free(&list);
    return result;
}

static int test_pipes(void)
{
    struct token_list list;
    int result;

    token_list_init(&list);

    if (lexer_run("echo hello | wc -w", &list) < 0)
    {
        token_list_free(&list);
        return 0;
    }

    result = list.count == 5 && token_is(&list, 0, TOKEN_WORD, "echo")
        && token_is(&list, 1, TOKEN_WORD, "hello")
        && token_is(&list, 2, TOKEN_PIPE, "|")
        && token_is(&list, 3, TOKEN_WORD, "wc")
        && token_is(&list, 4, TOKEN_WORD, "-w");

    token_list_free(&list);
    return result;
}

static int test_logical_operators(void)
{
    struct token_list list;
    int result;

    token_list_init(&list);

    if (lexer_run("true && echo yes || echo no", &list) < 0)
    {
        token_list_free(&list);
        return 0;
    }

    result = list.count == 7 && token_is(&list, 1, TOKEN_AND, "&&")
        && token_is(&list, 4, TOKEN_OR, "||");

    token_list_free(&list);
    return result;
}

static int test_redirections(void)
{
    struct token_list list;
    int result;

    token_list_init(&list);

    if (lexer_run("cat < input > output >> log", &list) < 0)
    {
        token_list_free(&list);
        return 0;
    }

    result = list.count == 7 && token_is(&list, 1, TOKEN_REDIRECTION_INPUT, "<")
        && token_is(&list, 3, TOKEN_REDIRECTION_OUTPUT, ">")
        && token_is(&list, 5, TOKEN_REDIRECTION_APPEND, ">>");

    token_list_free(&list);
    return result;
}

static int test_sequence_and_background(void)
{
    struct token_list list;
    int result;

    token_list_init(&list);

    if (lexer_run("sleep 1 & echo done; pwd", &list) < 0)
    {
        token_list_free(&list);
        return 0;
    }

    result = list.count == 7 && token_is(&list, 2, TOKEN_BACKGROUND, "&")
        && token_is(&list, 5, TOKEN_SEQUENCE, ";");

    token_list_free(&list);
    return result;
}

static int test_empty_line(void)
{
    struct token_list list;
    int result;

    token_list_init(&list);

    if (lexer_run("   ", &list) < 0)
    {
        token_list_free(&list);
        return 0;
    }

    result = list.count == 0;

    token_list_free(&list);
    return result;
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

    result = test_words();
    print_result("words", result);
    failed += result == 0;

    result = test_quotes();
    print_result("quotes", result);
    failed += result == 0;

    result = test_pipes();
    print_result("pipes", result);
    failed += result == 0;

    result = test_logical_operators();
    print_result("logical operators", result);
    failed += result == 0;

    result = test_redirections();
    print_result("redirections", result);
    failed += result == 0;

    result = test_sequence_and_background();
    print_result("sequence and background", result);
    failed += result == 0;

    result = test_empty_line();
    print_result("empty line", result);
    failed += result == 0;

    if (failed != 0)
    {
        return 1;
    }

    return 0;
}