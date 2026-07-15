#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define REDIRECTION_INPUT 1
#define REDIRECTION_OUTPUT 2
#define REDIRECTION_APPEND 3

#define LOGICAL_AND 1
#define LOGICAL_OR 2
#define LOGICAL_SEQUENCE 3

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