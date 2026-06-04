#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct command
{
    char **arguments;
    int count;
    int capacity;
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