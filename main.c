#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree_utils.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s in_file out_file1 out_file2 out_file3\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open input file
    FILE* input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return EXIT_FAILURE;
    }

    // Count number of nodes
    int node_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), input_file) != NULL) {
        node_count++;
    }
    
    // Reset file pointer to beginning
    rewind(input_file);

    // Build tree
    TreeNode* root = NULL;
    int nodes_processed = 0;
    if (buildTree(&root, input_file, &nodes_processed) != 0 || nodes_processed != node_count) {
        fprintf(stderr, "Error building tree\n");
        freeTree(root);
        fclose(input_file);
        return EXIT_FAILURE;
    }
    fclose(input_file);

    // Create and write to output file 1 (pre-order traversal)
    FILE* output_file1 = fopen(argv[2], "w");
    if (output_file1 == NULL) {
        fprintf(stderr, "Error opening output file 1\n");
        freeTree(root);
        return EXIT_FAILURE;
    }
    preOrderTraversal(root, output_file1);
    fclose(output_file1);

    // Calculate dimensions
    calculateDimensions(root);

    // Create and write to output file 2 (dimensions in post-order)
    FILE* output_file2 = fopen(argv[3], "w");
    if (output_file2 == NULL) {
        fprintf(stderr, "Error opening output file 2\n");
        freeTree(root);
        return EXIT_FAILURE;
    }
    postOrderTraversal(root, output_file2);
    fclose(output_file2);

    // Calculate coordinates
    calculateCoordinates(root, 0, 0);

    // Create and write to output file 3 (coordinates)
    FILE* output_file3 = fopen(argv[4], "w");
    if (output_file3 == NULL) {
        fprintf(stderr, "Error opening output file 3\n");
        freeTree(root);
        return EXIT_FAILURE;
    }
    writeCoordinates(root, output_file3);
    fclose(output_file3);

    // Clean up
    freeTree(root);
    return EXIT_SUCCESS;
}