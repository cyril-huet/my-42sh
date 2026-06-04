#include "parser.h"

#include <stdlib.h>

static int same_text(const char *first, const char *second)
{
    int index = 0;

    while (first[index] != '\0' && second[index] != '\0')
    {
        if (first[index] != second[index])
        {
            return 0;
        }

        index++;
    }

    if (first[index] == '\0' && second[index] == '\0')
    {
        return 1;
    }

    return 0;
}

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

static int increase_command_capacity(struct command *command)
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

    new_arguments =
        realloc(command->arguments, sizeof(char *) * new_capacity);

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
        if (increase_command_capacity(command) < 0)
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

static int increase_redirection_capacity(struct command *command)
{
    int new_capacity;
    struct redirection *new_redirections;

    if (command->redirection_capacity == 0)
    {
        new_capacity = 2;
    }
    else
    {
        new_capacity = command->redirection_capacity * 2;
    }

    new_redirections = realloc(
        command->redirections, sizeof(struct redirection) * new_capacity);

    if (new_redirections == NULL)
    {
        return -1;
    }

    command->redirections = new_redirections;
    command->redirection_capacity = new_capacity;
    return 0;
}

static int add_redirection(struct command *command, int type, const char *file)
{
    char *copy;

    if (command->redirection_count == command->redirection_capacity)
    {
        if (increase_redirection_capacity(command) < 0)
        {
            return -1;
        }
    }

    copy = copy_text(file);
    if (copy == NULL)
    {
        return -1;
    }

    command->redirections[command->redirection_count].type = type;
    command->redirections[command->redirection_count].file = copy;
    command->redirection_count++;

    return 0;
}

static int increase_pipeline_capacity(struct pipeline *pipeline)
{
    int new_capacity;
    struct command *new_commands;

    if (pipeline->capacity == 0)
    {
        new_capacity = 2;
    }
    else
    {
        new_capacity = pipeline->capacity * 2;
    }

    new_commands =
        realloc(pipeline->commands, sizeof(struct command) * new_capacity);

    if (new_commands == NULL)
    {
        return -1;
    }

    pipeline->commands = new_commands;
    pipeline->capacity = new_capacity;
    return 0;
}

static int add_command(struct pipeline *pipeline, struct command *command)
{
    if (pipeline->count == pipeline->capacity)
    {
        if (increase_pipeline_capacity(pipeline) < 0)
        {
            return -1;
        }
    }

    pipeline->commands[pipeline->count] = *command;
    pipeline->count++;

    command_init(command);
    return 0;
}

static int redirection_type(const char *value)
{
    if (same_text(value, "<") == 1)
    {
        return REDIRECTION_INPUT;
    }

    if (same_text(value, ">") == 1)
    {
        return REDIRECTION_OUTPUT;
    }

    return 0;
}

void command_init(struct command *command)
{
    command->arguments = NULL;
    command->count = 0;
    command->capacity = 0;
    command->redirections = NULL;
    command->redirection_count = 0;
    command->redirection_capacity = 0;
}

void command_free(struct command *command)
{
    for (int i = 0; i < command->count; i++)
    {
        free(command->arguments[i]);
    }

    for (int i = 0; i < command->redirection_count; i++)
    {
        free(command->redirections[i].file);
    }

    free(command->arguments);
    free(command->redirections);

    command->arguments = NULL;
    command->count = 0;
    command->capacity = 0;
    command->redirections = NULL;
    command->redirection_count = 0;
    command->redirection_capacity = 0;
}

void pipeline_init(struct pipeline *pipeline)
{
    pipeline->commands = NULL;
    pipeline->count = 0;
    pipeline->capacity = 0;
}

void pipeline_free(struct pipeline *pipeline)
{
    for (int i = 0; i < pipeline->count; i++)
    {
        command_free(&pipeline->commands[i]);
    }

    free(pipeline->commands);

    pipeline->commands = NULL;
    pipeline->count = 0;
    pipeline->capacity = 0;
}

int parser_run(const struct token_list *tokens, struct pipeline *pipeline)
{
    struct command current;

    command_init(&current);

    for (int i = 0; i < tokens->count; i++)
    {
        int type = redirection_type(tokens->items[i].value);

        if (same_text(tokens->items[i].value, "|") == 1)
        {
            if (current.count == 0
                || add_command(pipeline, &current) < 0)
            {
                command_free(&current);
                pipeline_free(pipeline);
                return -1;
            }
        }
        else if (type != 0)
        {
            if (i + 1 >= tokens->count
                || same_text(tokens->items[i + 1].value, "|") == 1
                || redirection_type(tokens->items[i + 1].value) != 0
                || same_text(tokens->items[i + 1].value, ">>") == 1)
            {
                command_free(&current);
                pipeline_free(pipeline);
                return -1;
            }

            if (add_redirection(&current, type, tokens->items[i + 1].value)
                < 0)
            {
                command_free(&current);
                pipeline_free(pipeline);
                return -1;
            }

            i++;
        }
        else if (same_text(tokens->items[i].value, ">>") == 1)
        {
            command_free(&current);
            pipeline_free(pipeline);
            return -1;
        }
        else if (add_argument(&current, tokens->items[i].value) < 0)
        {
            command_free(&current);
            pipeline_free(pipeline);
            return -1;
        }
    }

    if (current.count > 0)
    {
        if (add_command(pipeline, &current) < 0)
        {
            command_free(&current);
            pipeline_free(pipeline);
            return -1;
        }
    }
    else if (tokens->count > 0)
    {
        pipeline_free(pipeline);
        return -1;
    }

    return 0;
}