#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"

int is_builtin(const struct command *command);
int execute_builtin(const struct command *command);

#endif /* ! BUILTINS_H */
