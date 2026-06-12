#ifndef LEXER_H
#define LEXER_H

#define TOKEN_WORD 0
#define TOKEN_PIPE 1
#define TOKEN_AND 2
#define TOKEN_OR 3
#define TOKEN_REDIRECTION_INPUT 4
#define TOKEN_REDIRECTION_OUTPUT 5
#define TOKEN_REDIRECTION_APPEND 6
#define TOKEN_SEQUENCE 7

struct token
{
    char *value;
    int type;
};

struct token_list
{
    struct token *items;
    int count;
    int capacity;
};

void token_list_init(struct token_list *list);
void token_list_free(struct token_list *list);
int lexer_run(const char *line, struct token_list *list);

#endif /* ! LEXER_H */