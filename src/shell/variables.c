#include "variables.h"

#include <stdlib.h>

static int is_name_start(char character)
{
    if (character >= 'a' && character <= 'z')
    {
        return 1;
    }

    if (character >= 'A' && character <= 'Z')
    {
        return 1;
    }

    if (character == '_')
    {
        return 1;
    }

    return 0;
}

static int is_name_character(char character)
{
    if (is_name_start(character) == 1)
    {
        return 1;
    }

    if (character >= '0' && character <= '9')
    {
        return 1;
    }

    return 0;
}

static int add_character(char **result, int *length, int *capacity,
                         char character)
{
    int new_capacity;
    char *new_result;

    if (*length + 1 >= *capacity)
    {
        new_capacity = *capacity * 2;
        new_result = realloc(*result, sizeof(char) * new_capacity);

        if (new_result == NULL)
        {
            return -1;
        }

        *result = new_result;
        *capacity = new_capacity;
    }

    (*result)[*length] = character;
    (*length)++;
    (*result)[*length] = '\0';

    return 0;
}

static int add_text(char **result, int *length, int *capacity,
                    const char *text)
{
    int index = 0;

    while (text[index] != '\0')
    {
        if (add_character(result, length, capacity, text[index]) < 0)
        {
            return -1;
        }

        index++;
    }

    return 0;
}

static char *copy_variable_name(const char *text, int start, int length)
{
    char *name = malloc(sizeof(char) * (length + 1));

    if (name == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        name[i] = text[start + i];
    }

    name[length] = '\0';
    return name;
}

static int expand_text(const char *text, char **expanded)
{
    char *result;
    int capacity = 16;
    int length = 0;
    int index = 0;

    result = malloc(sizeof(char) * capacity);
    if (result == NULL)
    {
        return -1;
    }

    result[0] = '\0';

    while (text[index] != '\0')
    {
        if (text[index] == '$'
            && is_name_start(text[index + 1]) == 1)
        {
            int start = index + 1;
            int name_length = 0;
            char *name;
            char *value;

            while (is_name_character(text[start + name_length]) == 1)
            {
                name_length++;
            }

            name = copy_variable_name(text, start, name_length);
            if (name == NULL)
            {
                free(result);
                return -1;
            }

            value = getenv(name);
            if (value != NULL
                && add_text(&result, &length, &capacity, value) < 0)
            {
                free(name);
                free(result);
                return -1;
            }

            free(name);
            index = start + name_length;
        }
        else
        {
            if (add_character(&result, &length, &capacity, text[index])
                < 0)
            {
                free(result);
                return -1;
            }

            index++;
        }
    }

    *expanded = result;
    return 0;
}

int expand_token_list(struct token_list *list)
{
    for (int i = 0; i < list->count; i++)
    {
        char *expanded;

        if (list->items[i].type != TOKEN_WORD)
        {
            continue;
        }

        if (expand_text(list->items[i].value, &expanded) < 0)
        {
            return -1;
        }

        free(list->items[i].value);
        list->items[i].value = expanded;
    }

    return 0;
}