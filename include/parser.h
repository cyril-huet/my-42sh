#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define REDIRECTION_INPUT 1
#define REDIRECTION_OUTPUT 2
#define REDIRECTION_APPEND 3

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
};

void command_init(struct command *command);
void command_free(struct command *command);

void pipeline_init(struct pipeline *pipeline);
void pipeline_free(struct pipeline *pipeline);

int parser_run(const struct token_list *tokens, struct pipeline *pipeline);

#endif /* ! PARSER_H */