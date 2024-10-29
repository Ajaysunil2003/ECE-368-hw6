#ifndef TREE_UTILS_H
#define TREE_UTILS_H

#include <stdio.h>

// Structure definitions
typedef struct Rectangle {
    int id;
    int width;
    int height;
    int x;
    int y;
} Rectangle;

typedef struct TreeNode {
    char cut_type;  // 'H' for horizontal, 'V' for vertical, 'B' for block
    Rectangle rect;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Function declarations
TreeNode* createNode(char cut_type, int id, int width, int height);
int buildTree(TreeNode** root, FILE* input_file, int* nodes_processed);
void freeTree(TreeNode* root);

void preOrderTraversal(TreeNode* root, FILE* output_file);
void postOrderTraversal(TreeNode* root, FILE* output_file);
void writeCoordinates(TreeNode* root, FILE* output_file);

void calculateDimensions(TreeNode* root);
void calculateCoordinates(TreeNode* root, int x, int y);

#endif 