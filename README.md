# my-42sh

[![Language](https://img.shields.io/badge/language-C99-00599C)](https://en.wikipedia.org/wiki/C99)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

`my-42sh` is a small Unix shell written in C99.

This educational project was built to understand how a command interpreter works internally, from lexical analysis to process execution.

## Overview

The shell is implemented from scratch using standard C and POSIX functions.

It covers several important system programming concepts:

- lexical analysis
- command parsing
- process creation with `fork`
- external command execution with `execvp`
- pipes between processes
- input and output redirections
- environment variables
- builtin commands
- signal handling
- background processes
- subshell execution
- syntax error handling

The code intentionally stays simple and readable, with explicit control flow and small data structures.

## Features

- Interactive shell mode
- Execute external programs
- Command arguments
- Single and double quotes
- Pipelines with `|`
- Logical operators with `&&` and `||`
- Command sequences with `;`
- Background commands with `&`
- Input redirection with `<`
- Output redirection with `>`
- Append redirection with `>>`
- Basic subshells with `( ... )`
- Environment variable expansion
- Basic syntax error messages
- Signal handling for interactive use

## Builtins

The following builtin commands are available:

- `echo`
- `pwd`
- `env`
- `cd`
- `export`
- `unset`
- `exit`

## How it works

```mermaid
flowchart LR
    A[Command line] --> B[Lexer]
    B --> C[Parser]
    C --> D[Shell state]
    D --> E[Executor]
    E --> F[Processes and pipes]
```

The shell follows four main steps:

1. The lexer converts the input line into tokens.
2. The parser builds commands, pipelines and operators.
3. The shell expands variables and chooses how to execute each command.
4. The executor creates processes and connects their file descriptors.

## Requirements

- A C99 compiler
- A POSIX-compatible system
- `make`
- `clang-format` for formatting checks

No external library is required.

## Build

Clone the repository and build the shell:

```sh
git clone https://github.com/cyril-huet/my-42sh.git
cd my-42sh
make
```

The executable is created at:

```text
./my-42sh
```

## Usage

Start the interactive shell:

```sh
./my-42sh
```

The shell displays the following prompt:

```text
my-42sh$
```

A command can also be passed directly:

```sh
./my-42sh "echo hello"
```

## Examples

Execute a pipeline:

```sh
./my-42sh "echo hello world | wc -w"
```

Use a redirection:

```sh
./my-42sh "echo hello > output.txt"
```

Append to a file:

```sh
./my-42sh "echo another line >> output.txt"
```

Use logical operators:

```sh
./my-42sh "true && echo success"
./my-42sh "false || echo fallback"
```

Use a sequence:

```sh
./my-42sh "cd /tmp; pwd"
```

Use a subshell:

```sh
./my-42sh "(cd /tmp; pwd)"
```

Use environment variables:

```sh
./my-42sh "echo $HOME"
./my-42sh "echo $?"
```

## Tests

The project contains lexer tests, parser tests and shell integration tests.

Run the complete test suite:

```sh
make test
```

The tests cover:

- words and quoted words
- pipes and logical operators
- redirections
- command sequences
- background commands
- parser errors
- builtin commands
- real shell execution

## Formatting

Format all C source files and headers:

```sh
make format
```

Check the formatting without modifying files:

```sh
make check-format
```

Remove generated files:

```sh
make fclean
```

## Project structure

```text
my-42sh/
├── include/
│   ├── builtins.h
│   ├── executor.h
│   ├── lexer.h
│   ├── parser.h
│   ├── shell.h
│   └── variables.h
├── src/
│   ├── builtins/
│   │   └── builtins.c
│   ├── executor/
│   │   └── executor.c
│   ├── lexer/
│   │   └── lexer.c
│   ├── parser/
│   │   └── parser.c
│   ├── shell/
│   │   ├── shell.c
│   │   └── variables.c
│   └── main.c
├── tests/
│   ├── test_integration.sh
│   ├── test_lexer.c
│   └── test_parser.c
├── .clang-format
├── .gitignore
├── LICENSE
├── Makefile
└── README.md
```

## Limitations

`my-42sh` is an educational shell and is not intended to provide complete POSIX shell compatibility.

The following features are not implemented:

- heredocs
- filename globbing
- command substitution
- complete job control
- advanced shell grammar
- complete POSIX compatibility

## License

This project is distributed under the MIT License.