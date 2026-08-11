#include "parser.h"

#include <stdio.h>
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

static const char *token_text(const struct token *token)
{
    if (token == NULL)
    {
        return "end of command";
    }

    if (token->value != NULL)
    {
        return token->value;
    }

    if (token->type == TOKEN_PIPE)
    {
        return "|";
    }

    if (token->type == TOKEN_AND)
    {
        return "&&";
    }

    if (token->type == TOKEN_OR)
    {
        return "||";
    }

    if (token->type == TOKEN_SEQUENCE)
    {
        return ";";
    }

    if (token->type == TOKEN_BACKGROUND)
    {
        return "&";
    }

    if (token->type == TOKEN_REDIRECTION_INPUT)
    {
        return "<";
    }

    if (token->type == TOKEN_REDIRECTION_OUTPUT)
    {
        return ">";
    }

    if (token->type == TOKEN_REDIRECTION_APPEND)
    {
        return ">>";
    }

    return "unknown";
}

static void print_syntax_error(const struct token *token, const char *message)
{
    fprintf(stderr, "my-42sh: syntax error near '%s': %s\n", token_text(token),
            message);
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

    new_redirections = realloc(command->redirections,
                               sizeof(struct redirection) * new_capacity);
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

static int increase_list_capacity(struct command_list *list)
{
    int new_capacity;
    struct pipeline *new_pipelines;

    if (list->capacity == 0)
    {
        new_capacity = 2;
    }
    else
    {
        new_capacity = list->capacity * 2;
    }

    new_pipelines =
        realloc(list->pipelines, sizeof(struct pipeline) * new_capacity);
    if (new_pipelines == NULL)
    {
        return -1;
    }

    list->pipelines = new_pipelines;
    list->capacity = new_capacity;
    return 0;
}

static int add_pipeline(struct command_list *list, struct pipeline *pipeline)
{
    if (list->count == list->capacity)
    {
        if (increase_list_capacity(list) < 0)
        {
            return -1;
        }
    }

    list->pipelines[list->count] = *pipeline;
    list->count++;

    pipeline_init(pipeline);
    return 0;
}

static int increase_operator_capacity(struct command_list *list)
{
    int new_capacity;
    int *new_operators;

    if (list->operator_capacity == 0)
    {
        new_capacity = 2;
    }
    else
    {
        new_capacity = list->operator_capacity * 2;
    }

    new_operators = realloc(list->operators, sizeof(int) * new_capacity);
    if (new_operators == NULL)
    {
        return -1;
    }

    list->operators = new_operators;
    list->operator_capacity = new_capacity;
    return 0;
}

static int add_operator(struct command_list *list, int operator_type)
{
    if (list->operator_count == list->operator_capacity)
    {
        if (increase_operator_capacity(list) < 0)
        {
            return -1;
        }
    }

    list->operators[list->operator_count] = operator_type;
    list->operator_count++;

    return 0;
}

static int finish_command(struct pipeline *pipeline, struct command *command)
{
    if (command->count == 0)
    {
        return -1;
    }

    return add_command(pipeline, command);
}

static int parser_error(struct command_list *list, struct pipeline *pipeline,
                        struct command *command, const struct token *token,
                        const char *message)
{
    print_syntax_error(token, message);

    command_free(command);
    pipeline_free(pipeline);
    command_list_free(list);

    return -1;
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

    command_init(command);
}

void pipeline_init(struct pipeline *pipeline)
{
    pipeline->commands = NULL;
    pipeline->count = 0;
    pipeline->capacity = 0;
    pipeline->background = 0;
}

void pipeline_free(struct pipeline *pipeline)
{
    for (int i = 0; i < pipeline->count; i++)
    {
        command_free(&pipeline->commands[i]);
    }

    free(pipeline->commands);
    pipeline_init(pipeline);
}

void command_list_init(struct command_list *list)
{
    list->pipelines = NULL;
    list->count = 0;
    list->capacity = 0;
    list->operators = NULL;
    list->operator_count = 0;
    list->operator_capacity = 0;
}

void command_list_free(struct command_list *list)
{
    for (int i = 0; i < list->count; i++)
    {
        pipeline_free(&list->pipelines[i]);
    }

    free(list->pipelines);
    free(list->operators);

    command_list_init(list);
}

int parser_run(const struct token_list *tokens, struct command_list *list)
{
    struct pipeline current_pipeline;
    struct command current_command;

    if (tokens == NULL || list == NULL)
    {
        return -1;
    }

    command_list_init(list);
    pipeline_init(&current_pipeline);
    command_init(&current_command);

    if (tokens->count == 0)
    {
        return 0;
    }

    for (int i = 0; i < tokens->count; i++)
    {
        struct token *token = &tokens->items[i];

        if (token->type == TOKEN_PIPE)
        {
            if (finish_command(&current_pipeline, &current_command) < 0)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token, "expected a command before '|'");
            }
        }
        else if (token->type == TOKEN_AND || token->type == TOKEN_OR
                 || token->type == TOKEN_SEQUENCE
                 || token->type == TOKEN_BACKGROUND)
        {
            int operator_type;

            if (current_command.count == 0)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token,
                                    "expected a command before operator");
            }

            if (token->type == TOKEN_BACKGROUND)
            {
                current_pipeline.background = 1;
            }

            if (finish_command(&current_pipeline, &current_command) < 0)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token, "could not finish command");
            }

            if (add_pipeline(list, &current_pipeline) < 0)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token, "not enough memory");
            }

            if (token->type == TOKEN_AND)
            {
                operator_type = LOGICAL_AND;
            }
            else if (token->type == TOKEN_OR)
            {
                operator_type = LOGICAL_OR;
            }
            else
            {
                operator_type = LOGICAL_SEQUENCE;
            }

            if (add_operator(list, operator_type) < 0)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token, "not enough memory");
            }
        }
        else if (token->type == TOKEN_REDIRECTION_INPUT
                 || token->type == TOKEN_REDIRECTION_OUTPUT
                 || token->type == TOKEN_REDIRECTION_APPEND)
        {
            int redirection;

            if (i + 1 >= tokens->count)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token, "expected a file after redirection");
            }

            if (tokens->items[i + 1].type != TOKEN_WORD)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    &tokens->items[i + 1],
                                    "expected a file after redirection");
            }

            if (token->type == TOKEN_REDIRECTION_INPUT)
            {
                redirection = REDIRECTION_INPUT;
            }
            else if (token->type == TOKEN_REDIRECTION_OUTPUT)
            {
                redirection = REDIRECTION_OUTPUT;
            }
            else
            {
                redirection = REDIRECTION_APPEND;
            }

            if (add_redirection(&current_command, redirection,
                                tokens->items[i + 1].value)
                < 0)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token, "not enough memory");
            }

            i++;
        }
        else if (token->type == TOKEN_WORD)
        {
            if (add_argument(&current_command, token->value) < 0)
            {
                return parser_error(list, &current_pipeline, &current_command,
                                    token, "not enough memory");
            }
        }
        else
        {
            return parser_error(list, &current_pipeline, &current_command,
                                token, "unexpected token");
        }
    }

    if (current_command.count > 0)
    {
        if (finish_command(&current_pipeline, &current_command) < 0)
        {
            return parser_error(list, &current_pipeline, &current_command, NULL,
                                "could not finish command");
        }
    }

    if (current_pipeline.count > 0)
    {
        if (tokens->items[tokens->count - 1].type == TOKEN_PIPE)
        {
            return parser_error(list, &current_pipeline, &current_command, NULL,
                                "expected a command after '|'");
        }

        if (add_pipeline(list, &current_pipeline) < 0)
        {
            return parser_error(list, &current_pipeline, &current_command, NULL,
                                "not enough memory");
        }
    }
    else
    {
        int last_type = tokens->items[tokens->count - 1].type;

        if (last_type == TOKEN_AND || last_type == TOKEN_OR)
        {
            return parser_error(list, &current_pipeline, &current_command, NULL,
                                "expected a command after operator");
        }

        if (last_type == TOKEN_SEQUENCE || last_type == TOKEN_BACKGROUND)
        {
            if (list->operator_count > 0)
            {
                list->operator_count--;
            }
        }
        else
        {
            return parser_error(list, &current_pipeline, &current_command, NULL,
                                "expected a command");
        }
    }

    return 0;
}