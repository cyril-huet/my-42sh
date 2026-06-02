# Program name
NAME = my-42sh

# Compiler and options
CC = cc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -Wvla
INCLUDES = -Iinclude

# Source and object files
SRC = src/main.c
OBJ = $(SRC:.c=.o)

# Files checked by clang-format
FORMAT_FILES = $(SRC)

# Build the shell
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# Format the source files
format:
	clang-format -i $(FORMAT_FILES)

check-format:
	clang-format --dry-run -Werror $(FORMAT_FILES)

# Remove generated files
clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all format check-format clean fclean re
