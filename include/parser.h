#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct command
{
    char **arguments;
    int count;
    int capacity;
};

void command_init(struct command *command);
void command_free(struct command *command);
int parser_run(const struct token_list *tokens, struct command *command);

#endif /* ! PARSER_H */
