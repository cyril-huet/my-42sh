#include "parser.h"

#include <stdlib.h>

static char *copy_text(const char *text)
{
    int length = 0;
    char *copy;

    while (text[length] != '\0')
    {
        length++;
    }

    copy = malloc(sizeof(char) * (length + 1));
    if (copy == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        copy[i] = text[i];
    }

    copy[length] = '\0';
    return copy;
}

static int increase_capacity(struct command *command)
{
    int new_capacity;
    char **new_arguments;

    if (command->capacity == 0)
    {
        new_capacity = 4;
    }
    else
    {
        new_capacity = command->capacity * 2;
    }

    new_arguments = realloc(command->arguments, sizeof(char *) * new_capacity);

    if (new_arguments == NULL)
    {
        return -1;
    }

    command->arguments = new_arguments;
    command->capacity = new_capacity;
    return 0;
}

static int add_argument(struct command *command, const char *value)
{
    char *copy;

    if (command->count + 1 >= command->capacity)
    {
        if (increase_capacity(command) < 0)
        {
            return -1;
        }
    }

    copy = copy_text(value);
    if (copy == NULL)
    {
        return -1;
    }

    command->arguments[command->count] = copy;
    command->count++;
    command->arguments[command->count] = NULL;

    return 0;
}

void command_init(struct command *command)
{
    command->arguments = NULL;
    command->count = 0;
    command->capacity = 0;
}

void command_free(struct command *command)
{
    for (int i = 0; i < command->count; i++)
    {
        free(command->arguments[i]);
    }

    free(command->arguments);

    command->arguments = NULL;
    command->count = 0;
    command->capacity = 0;
}

int parser_run(const struct token_list *tokens, struct command *command)
{
    for (int i = 0; i < tokens->count; i++)
    {
        if (add_argument(command, tokens->items[i].value) < 0)
        {
            command_free(command);
            return -1;
        }
    }

    return 0;
}
