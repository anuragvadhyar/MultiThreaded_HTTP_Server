#ifndef MAP_H
#define MAP_H

typedef struct node {
    char *key;
    char *val;
    struct node *left;
    struct node *right;
} TreeNode;

TreeNode *createNode(char *key, char *val);
TreeNode *add_route(TreeNode *root, char *key, char *val);
void all_routes(int *fd, TreeNode *root);
char *find_route(TreeNode *root, char *key);

#endif // MAP_H