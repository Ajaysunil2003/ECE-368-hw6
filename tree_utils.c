#include <stdlib.h>
#include <string.h>
#include "tree_utils.h"

typedef struct Stack {
    TreeNode** items;
    int top;
    int capacity;
} Stack;

Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) return NULL;
    
    stack->items = (TreeNode**)malloc(capacity * sizeof(TreeNode*));
    if (!stack->items) {
        free(stack);
        return NULL;
    }
    
    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

void push(Stack* stack, TreeNode* node) {
    if (stack->top < stack->capacity - 1) {
        stack->items[++stack->top] = node;
    }
}

TreeNode* pop(Stack* stack) {
    if (stack->top >= 0) {
        return stack->items[stack->top--];
    }
    return NULL;
}

void freeStack(Stack* stack) {
    if (stack) {
        free(stack->items);
        free(stack);
    }
}

TreeNode* createNode(char cut_type, int id, int width, int height) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->cut_type = cut_type;
    node->rect.id = id;
    node->rect.width = width;
    node->rect.height = height;
    node->rect.x = 0;
    node->rect.y = 0;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

int buildTree(TreeNode** root, FILE* input_file, int* nodes_processed) {
    char line[256];
    *nodes_processed = 0;
    
    // Count lines first
    int lineCount = 0;
    long pos = ftell(input_file);
    while (fgets(line, sizeof(line), input_file) != NULL) {
        lineCount++;
    }
    fseek(input_file, pos, SEEK_SET);
    
    // Create stack
    Stack* stack = createStack(lineCount);
    if (!stack) {
        return -1;
    }

    // Process each line
    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Remove newline
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        TreeNode* node = NULL;
        if (line[0] == 'V' || line[0] == 'H') {
            // Create cut node
            if (len != 1) {
                freeStack(stack);
                return -1;
            }
            
            node = createNode(line[0], 0, 0, 0);
            if (!node) {
                freeStack(stack);
                return -1;
            }

            // Pop right and left children
            node->right = pop(stack);
            node->left = pop(stack);
            
            if (!node->right || !node->left) {
                freeTree(node);
                freeStack(stack);
                return -1;
            }
        } else {
            // Parse block node
            int id, width, height;
            if (sscanf(line, "%d(%d,%d)", &id, &width, &height) != 3 || 
                width <= 0 || height <= 0 || id <= 0) {
                freeStack(stack);
                return -1;
            }
            
            node = createNode('B', id, width, height);
            if (!node) {
                freeStack(stack);
                return -1;
            }
        }

        push(stack, node);
        (*nodes_processed)++;
    }

    // The last node on the stack should be the root
    *root = pop(stack);
    
    // Check if stack is empty (should be for a valid tree)
    TreeNode* extra = pop(stack);
    if (extra != NULL) {
        freeTree(*root);
        *root = NULL;
        freeStack(stack);
        return -1;
    }

    freeStack(stack);
    return 0;
}

void preOrderTraversal(TreeNode* root, FILE* output_file) {
    if (root == NULL) {
        return;
    }

    // Process current node
    if (root->cut_type == 'B') {
        fprintf(output_file, "%d(%d,%d)\n", 
            root->rect.id, root->rect.width, root->rect.height);
    } else {
        fprintf(output_file, "%c\n", root->cut_type);
    }

    // Process left and right subtrees
    preOrderTraversal(root->left, output_file);
    preOrderTraversal(root->right, output_file);
}

void postOrderTraversal(TreeNode* root, FILE* output_file) {
    if (root == NULL) {
        return;
    }

    // Process left and right subtrees first
    postOrderTraversal(root->left, output_file);
    postOrderTraversal(root->right, output_file);
    
    // Then process current node
    if (root->cut_type == 'B') {
        fprintf(output_file, "%d(%d,%d)\n",
            root->rect.id, root->rect.width, root->rect.height);
    } else {
        fprintf(output_file, "%c(%d,%d)\n",
            root->cut_type, root->rect.width, root->rect.height);
    }
}

void calculateDimensions(TreeNode* root) {
    if (root == NULL || root->cut_type == 'B') {
        return;
    }

    // Calculate dimensions of subtrees first
    calculateDimensions(root->left);
    calculateDimensions(root->right);

    if (root->cut_type == 'V') {
        // For vertical cut, add widths and take max height
        root->rect.width = root->left->rect.width + root->right->rect.width;
        root->rect.height = (root->left->rect.height > root->right->rect.height) ?
                           root->left->rect.height : root->right->rect.height;
    } else if (root->cut_type == 'H') {
        // For horizontal cut, add heights and take max width
        root->rect.width = (root->left->rect.width > root->right->rect.width) ?
                          root->left->rect.width : root->right->rect.width;
        root->rect.height = root->left->rect.height + root->right->rect.height;
    }
}

void calculateCoordinates(TreeNode* root, int x, int y) {
    if (root == NULL) {
        return;
    }

    root->rect.x = x;
    root->rect.y = y;

    if (root->cut_type == 'V') {
        calculateCoordinates(root->left, x, y);
        calculateCoordinates(root->right, x + root->left->rect.width, y);
    } else if (root->cut_type == 'H') {
        calculateCoordinates(root->right, x, y);
        calculateCoordinates(root->left, x, y + root->right->rect.height);
    }
}

void storeBlocksInOrder(TreeNode* root, TreeNode** blocks, int* count) {
    if (root == NULL) {
        return;
    }
    
    if (root->cut_type == 'B') {
        blocks[*count] = root;
        (*count)++;
        return;
    }
    
    storeBlocksInOrder(root->left, blocks, count);
    storeBlocksInOrder(root->right, blocks, count);
}

int countBlocks(TreeNode* root) {
    if (root == NULL) {
        return 0;
    }
    if (root->cut_type == 'B') {
        return 1;
    }
    return countBlocks(root->left) + countBlocks(root->right);
}

void writeCoordinates(TreeNode* root, FILE* output_file) {
    int block_count = countBlocks(root);
    
    // Create array for blocks
    TreeNode** blocks = malloc(block_count * sizeof(TreeNode*));
    if (blocks == NULL) {
        return;
    }

    // Store blocks in order
    int count = 0;
    storeBlocksInOrder(root, blocks, &count);

    // Write coordinates
    for (int i = 0; i < count; i++) {
        fprintf(output_file, "%d((%d,%d)(%d,%d))\n",
            blocks[i]->rect.id,
            blocks[i]->rect.width, blocks[i]->rect.height,
            blocks[i]->rect.x, blocks[i]->rect.y);
    }

    free(blocks);
}

void freeTree(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}