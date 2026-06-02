#include "lexer.h"

#include <stdlib.h>

static int is_space_character(char character)
{
    if (character == ' ' || character == '\t')
    {
        return 1;
    }

    return 0;
}

static char *copy_word(const char *line, int start, int length)
{
    char *word = malloc(sizeof(char) * (length + 1));

    if (word == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        word[i] = line[start + i];
    }

    word[length] = '\0';
    return word;
}

static int increase_capacity(struct token_list *list)
{
    int new_capacity;
    struct token *new_items;

    if (list->capacity == 0)
    {
        new_capacity = 4;
    }
    else
    {
        new_capacity = list->capacity * 2;
    }

    new_items = realloc(list->items, sizeof(struct token) * new_capacity);

    if (new_items == NULL)
    {
        return -1;
    }

    list->items = new_items;
    list->capacity = new_capacity;
    return 0;
}

static int add_token(struct token_list *list, const char *line, int start,
                     int length)
{
    char *word;

    if (list->count == list->capacity)
    {
        if (increase_capacity(list) < 0)
        {
            return -1;
        }
    }

    word = copy_word(line, start, length);
    if (word == NULL)
    {
        return -1;
    }

    list->items[list->count].value = word;
    list->count++;
    return 0;
}

void token_list_init(struct token_list *list)
{
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void token_list_free(struct token_list *list)
{
    for (int i = 0; i < list->count; i++)
    {
        free(list->items[i].value);
    }

    free(list->items);

    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

int lexer_run(const char *line, struct token_list *list)
{
    int index = 0;
    int start;

    while (line[index] != '\0')
    {
        while (line[index] != '\0' && is_space_character(line[index]) == 1)
        {
            index++;
        }

        if (line[index] == '\0')
        {
            break;
        }

        start = index;

        while (line[index] != '\0' && is_space_character(line[index]) == 0)
        {
            index++;
        }

        if (add_token(list, line, start, index - start) < 0)
        {
            token_list_free(list);
            return -1;
        }
    }

    return 0;
}
