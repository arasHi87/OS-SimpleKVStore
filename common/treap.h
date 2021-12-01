#ifndef _TREAP_H_
#define _TREAP_H_
#define MAX_LEN 1024

typedef struct Node
{
    char key[MAX_LEN], val[MAX_LEN];
    int pri;
    struct Node *left, *right;
} Node;

int cmp(char *x, char *y);
Node *init(char *key, char *val);
Node *find(Node *root, char *key);
Node *insert(Node *root, char *key, char *val);
Node *destroy(Node *root, char *key);

#endif