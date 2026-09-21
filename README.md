# my-42sh

A small Unix shell written in C99.

This project is an educational shell developed to understand how a command interpreter works internally: lexical analysis, parsing, processes, pipes, redirections and builtins.

## Features

- Interactive shell mode
- Execute external commands
- Command arguments
- Single and double quotes
- Pipelines with `|`
- Logical operators with `&&` and `||`
- Command sequences with `;`
- Background commands with `&`
- Input redirection with `<`
- Output redirection with `>`
- Append redirection with `>>`
- Subshells with `( ... )`
- Environment variable expansion
- Basic syntax error messages
- Signal handling

## Builtins

The shell currently provides:

- `echo`
- `pwd`
- `env`
- `cd`
- `export`
- `unset`
- `exit`

## Build

```sh
make
```

This creates the `my-42sh` executable.

## Usage

Start the interactive shell:

```sh
./my-42sh
```

Execute one command directly:

```sh
./my-42sh "echo hello"
```

Examples:

```sh
./my-42sh "echo hello | wc -w"
./my-42sh "cat < input.txt"
./my-42sh "echo hello > output.txt"
./my-42sh "true && echo success"
./my-42sh "(cd /tmp; pwd)"
```

## Tests

Run the complete test suite:

```sh
make test
```

The tests cover:

- Lexer tokens
- Quotes and operators
- Parser commands
- Pipelines
- Redirections
- Logical operators
- Background commands
- Syntax errors
- Shell integration tests

## Formatting

Format the source files:

```sh
make format
```

Check the formatting without modifying files:

```sh
make check-format
```

## Project structure

```text
include/
    builtins.h
    executor.h
    lexer.h
    parser.h
    shell.h
    variables.h

src/
    builtins/
    executor/
    lexer/
    parser/
    shell/
    main.c

tests/
    test_lexer.c
    test_parser.c
    test_integration.sh
```

## Limitations

This is an educational shell and is not intended to implement every POSIX shell feature.

It does not currently provide features such as:

- Heredocs
- Filename globbing
- Command substitution
- Full job control
- Complete POSIX compatibility

## License

This project is distributed under the MIT License.