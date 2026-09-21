#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

enum redirection_type
{
    REDIRECTION_INPUT,
    REDIRECTION_OUTPUT,
    REDIRECTION_APPEND
};

enum logical_operator
{
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_SEQUENCE
};

struct redirection
{
    int type;
    char *file;
};

struct command
{
    char **arguments;
    int count;
    int capacity;
    struct redirection *redirections;
    int redirection_count;
    int redirection_capacity;
};

struct pipeline
{
    struct command *commands;
    int count;
    int capacity;
    int background;
};

struct command_list
{
    struct pipeline *pipelines;
    int count;
    int capacity;
    int *operators;
    int operator_count;
    int operator_capacity;
};

void command_init(struct command *command);
void command_free(struct command *command);

void pipeline_init(struct pipeline *pipeline);
void pipeline_free(struct pipeline *pipeline);

void command_list_init(struct command_list *list);
void command_list_free(struct command_list *list);

int parser_run(const struct token_list *tokens, struct command_list *list);

#endif /* ! PARSER_H */