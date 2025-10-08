#include <stdio.h>
#include <stdlib.h>
#include "modules/ast/ast.h"
#include "modules/intermediate/intermediate.h"

ASTNode* root = NULL;

int main() {
    printf("Testing minimal intermediate code generation...\n");
    
    // Create a simple AST node
    root = create_ast_node(PROGRAM_NODE, 0, NULL);
    if (!root) {
        printf("Failed to create AST node\n");
        return 1;
    }
    
    printf("Created AST node: %p\n", (void*)root);
    
    // Generate intermediate code
    printf("About to call generate_intermediate_code...\n");
    IRCode* ir_code = generate_intermediate_code(root);
    printf("generate_intermediate_code returned: %p\n", (void*)ir_code);
    
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
