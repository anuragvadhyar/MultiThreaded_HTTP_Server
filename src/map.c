#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "map.h"

TreeNode *createNode(char *key, char *val) {
    TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
    new_node->key = key;
    new_node->val = val;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

TreeNode *add_route(TreeNode *root, char *key, char *val) {
    if (root == NULL) {
        printf("works here\n");
        return createNode(key, val);
    }
    if (strcmp(root->key, key) <= 0) {
        root->left = add_route(root->left, key, val);
    } else {
        root->right = add_route(root->right, key, val);
    }
    return root;
}

void all_routes(int *fd, TreeNode *root) {
    if (!root) {
        return;
    }
    all_routes(fd, root->left);
    write(*fd, root->key, strlen(root->key));
    write(*fd, "\n", 1);
    all_routes(fd, root->right);
}

char *find_route(TreeNode *root, char *key) {
    if (root == NULL) {
        return NULL;
    }
    if (strcmp(root->key, key) == 0) {
        return root->val;
    }
    if (strcmp(root->key, key) < 0) {
        return find_route(root->left, key);
    } else {
        return find_route(root->right, key);
    }
}