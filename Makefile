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
      src/shell/shell.c

OBJ = $(SRC:.c=.o)


# Build the shell
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# Format the source files
format:
	find include src tests -type f \( -name "*.h" -o -name "*.c" \) -exec clang-format -i {} +

check-format:
	clang-format --dry-run -Werror $(FORMAT_FILES)

# Remove generated files
clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all format check-format clean fclean re
