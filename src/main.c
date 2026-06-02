#include <stdio.h>

#include "lexer.h"

static void print_tokens(const struct token_list *list)
{
    for (int i = 0; i < list->count; i++)
    {
        printf("%s\n", list->items[i].value);
    }
}

int main(int argc, char **argv)
{
    struct token_list list;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        return 1;
    }

    token_list_init(&list);

    if (lexer_run(argv[1], &list) < 0)
    {
        fprintf(stderr, "my-42sh: memory allocation failed\n");
        return 1;
    }

    print_tokens(&list);
    token_list_free(&list);

    return 0;
}
