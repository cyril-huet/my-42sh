#include <stdio.h>

#include "shell.h"
#include "variables.h"

int main(int argc, char **argv)
{
    struct shell_state state;

    shell_state_init(&state, argv[0], argc - 1);

    if (argc == 1)
    {
        return shell_run(&state);
    }

    if (argc == 2)
    {
        return shell_run_line(argv[1], &state);
    }

    fprintf(stderr, "Usage: %s [command]\n", argv[0]);
    return 1;
}