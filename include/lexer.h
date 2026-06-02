#ifndef LEXER_H
#define LEXER_H

struct token
{
    char *value;
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
