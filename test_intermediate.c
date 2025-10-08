#include <stdio.h>
#include <stdlib.h>
#include "modules/intermediate/intermediate.h"
#include "modules/ast/ast.h"

int main() {
    printf("Testing intermediate code generator...\n");
    
    // Create a simple AST node
    ASTNode* root = create_ast_node(PROGRAM_NODE, 0, NULL);
    if (!root) {
        printf("Failed to create AST node\n");
        return 1;
    }
    
    printf("Created AST node\n");
    
    // Generate intermediate code
    IRCode* ir_code = generate_intermediate_code(root);
    if (!ir_code) {
        printf("Failed to generate intermediate code\n");
        return 1;
    }
    
    printf("Generated intermediate code\n");
    
    // Print the code
    print_ir_code(ir_code);
    
    // Clean up
    free_ir_code(ir_code);
    free_ast_node(root);
    
    printf("Test completed successfully\n");
    return 0;
}
