#ifndef VARIABLES_H
#define VARIABLES_H

#include "lexer.h"

struct shell_state
{
    int last_status;
    int shell_pid;
    int last_background;
    int argument_count;
    const char *program_name;
};

void shell_state_init(struct shell_state *state, const char *program_name,
                      int argument_count);

int expand_token_list(struct token_list *list, const struct shell_state *state);

#endif /* ! VARIABLES_H */