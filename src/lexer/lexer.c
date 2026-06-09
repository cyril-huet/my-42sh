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

static int text_length(const char *text)
{
    int length = 0;

    while (text[length] != '\0')
    {
        length++;
    }

    return length;
}

static char *copy_word(const char *word)
{
    int length = text_length(word);
    char *copy = malloc(sizeof(char) * (length + 1));

    if (copy == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        copy[i] = word[i];
    }

    copy[length] = '\0';
    return copy;
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

static int add_token(struct token_list *list, const char *word)
{
    char *copy;

    if (list->count == list->capacity)
    {
        if (increase_capacity(list) < 0)
        {
            return -1;
        }
    }

    copy = copy_word(word);
    if (copy == NULL)
    {
        return -1;
    }

    list->items[list->count].value = copy;
    list->count++;
    return 0;
}

static int read_word(const char *line, int *index, char *word)
{
    char quote = '\0';
    int length = 0;
    int started = 0;

    while (line[*index] != '\0')
    {
        char current = line[*index];

        if (quote != '\0')
        {
            if (current == quote)
            {
                quote = '\0';
            }
            else
            {
                word[length] = current;
                length++;
            }

            (*index)++;
            continue;
        }

        if (current == '\'' || current == '"')
        {
            quote = current;
            started = 1;
            (*index)++;
            continue;
        }

        if (current == '|' || current == '&' || current == '<'
            || current == '>')
        {
            if (started == 0)
            {
                word[0] = current;
                length = 1;
                (*index)++;

                if ((current == '|' || current == '&')
                    && line[*index] == current)
                {
                    word[length] = current;
                    length++;
                    (*index)++;
                }

                if (current == '>' && line[*index] == '>')
                {
                    word[length] = '>';
                    length++;
                    (*index)++;
                }

                word[length] = '\0';
                return 1;
            }

            break;
        }

        if (is_space_character(current) == 1)
        {
            break;
        }

        word[length] = current;
        length++;
        started = 1;
        (*index)++;
    }

    if (quote != '\0')
    {
        return -1;
    }

    word[length] = '\0';
    return started;
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
    int line_size = text_length(line);

    while (line[index] != '\0')
    {
        char *word;
        int result;

        while (is_space_character(line[index]) == 1)
        {
            index++;
        }

        if (line[index] == '\0')
        {
            break;
        }

        word = malloc(sizeof(char) * (line_size + 1));
        if (word == NULL)
        {
            token_list_free(list);
            return -1;
        }

        result = read_word(line, &index, word);
        if (result < 0)
        {
            free(word);
            token_list_free(list);
            return -1;
        }

        if (result == 1 && add_token(list, word) < 0)
        {
            free(word);
            token_list_free(list);
            return -1;
        }

        free(word);
    }

    return 0;
}