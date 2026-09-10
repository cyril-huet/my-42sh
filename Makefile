# Program name
NAME = my-42sh

# Compiler and options
CC = cc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -Wvla
INCLUDES = -Iinclude

# Source and object files
SRC = src/main.c \
      src/lexer/lexer.c \
      src/parser/parser.c \
      src/executor/executor.c \
      src/builtins/builtins.c \
      src/shell/shell.c \
      src/shell/variables.c

OBJ = $(SRC:.c=.o)


# Build the shell
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

TEST_FORMAT_FILES = $(shell find include src tests -type f \( -name "*.h" -o -name "*.c" \))

# Format the source files
format:
	find include src tests -type f \( -name "*.h" -o -name "*.c" \) -exec clang-format -i {} +

check-format:
	clang-format --dry-run -Werror $(TEST_FORMAT_FILES)


# Build and run tests
lexer-tests: tests/test_lexer.o src/lexer/lexer.o 
	$(CC) $(CFLAGS) tests/test_lexer.o \
		src/lexer/lexer.o  -o lexer-tests

parser-tests: tests/test_parser.o src/parser/parser.o \
	src/lexer/lexer.o 
	$(CC) $(CFLAGS) tests/test_parser.o \
		src/parser/parser.o src/lexer/lexer.o \
		-o parser-tests

# Integration tests
integration-test: $(NAME)
	sh tests/test_integration.sh ./$(NAME)

# Run every test
test: lexer-tests parser-tests integration-test
	./lexer-tests
	./parser-tests

# Remove generated files
clean:
	rm -f $(OBJ)
	rm -f tests/test_lexer.o tests/test_parser.o
	rm -f lexer-tests parser-tests

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all format check-format clean fclean re
