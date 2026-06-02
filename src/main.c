#include <stdio.h>

#include "shell.h"

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        return shell_run();
    }

    if (argc == 2)
    {
        return shell_run_line(argv[1]);
    }

    fprintf(stderr, "Usage: %s [command]\n", argv[0]);
    return 1;
}
