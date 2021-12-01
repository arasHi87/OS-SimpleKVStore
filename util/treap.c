#include "treap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmp(char *x, char *y)
{
    // -1 -> x = y
    // 0  -> x < y
    // 1  -> x > y
    if (!strcmp(x, y))
        return -1;
    while (*x && *y)
    {
        if (*x != *y)
            return *x > *y;
        x++, y++;
    }
    return *x ? 1 : 0;
}

Node *init(char *key, char *val)
{
    Node *tmp = (Node *)malloc(sizeof(Node));
    tmp->pri = rand();
    tmp->left = tmp->right = NULL;
    strncpy(tmp->key, key, strlen(key));
    strncpy(tmp->val, val, strlen(val));
    return tmp;
}

Node *rightRotate(Node *rt)
{
    Node *x = rt->left, *y = x->right;
    x->right = rt;
    rt->left = y;
    return x;
}

Node *leftRotate(Node *rt)
{
    Node *x = rt->right, *y = x->left;
    x->left = rt;
    rt->right = y;
    return x;
}

Node *find(Node *rt, char *key)
{
    if (rt == NULL || !strcmp(rt->key, key))
        return rt;
    if (!cmp(rt->key, key))
        return find(rt->right, key);
    return find(rt->left, key);
}

Node *insert(Node *rt, char *key, char *val)
{
    if (!rt)
        return init(key, val);
    if (cmp(key, rt->key) <= 0)
    {
        rt->left = insert(rt->left, key, val);
        if (rt->left->pri > rt->pri)
            rt = rightRotate(rt);
    }
    else
    {
        rt->right = insert(rt->right, key, val);
        if (rt->right->pri > rt->pri)
            rt = leftRotate(rt);
    }
    return rt;
}

Node *destroy(Node *rt, char *key)
{
    if (!rt)
        return rt;
    if (!cmp(key, rt->key))
        rt->left = destroy(rt->left, key);
    else if (cmp(key, rt->key) > 0)
        rt->right = destroy(rt->right, key);
    else if (!rt->left)
    {
        Node *tmp = rt->right;
        free(rt), rt = tmp;
    }
    else if (!rt->right)
    {
        Node *tmp = rt->left;
        free(rt), rt = tmp;
    }
    else if (rt->left->pri < rt->right->pri)
        rt = leftRotate(rt), rt->left = destroy(rt->left, key);
    else
        rt = rightRotate(rt), rt->right = destroy(rt->right, key);
    return rt;
}