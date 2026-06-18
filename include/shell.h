#ifndef SHELL_H
#define SHELL_H

#include "variables.h"

int shell_run(struct shell_state *state);
int shell_run_line(const char *line, struct shell_state *state);

#endif /* ! SHELL_H */