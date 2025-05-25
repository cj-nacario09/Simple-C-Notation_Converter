#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX 100

// ===============================
// Node for the Expression Tree
// ===============================
typedef struct Node {
    char value[10];         // The operator or operand value (e.g., "+", "a", "42")
    struct Node* left;      // Left child
    struct Node* right;     // Right child
} Node;

// ===============================
// Token Structure
// ===============================
typedef struct {
    char* value;            // Dynamically allocated token value
    bool isOperator;        // Flag to indicate if token is an operator
} Token;

// ===============================
// Stack for Nodes
// ===============================
typedef struct Stack {
    Node* data[MAX];        // Array to hold stack elements
    int top;                // Index of the top element
} Stack;

// Initialize the stack
void initStack(Stack* s) { s->top = -1; }

// Check if the stack is empty
bool isEmpty(Stack* s) { return s->top == -1; }

// Push a node onto the stack
void push(Stack* s, Node* node) {
    if (s->top >= MAX - 1) {
        printf("Error: Stack overflow\n");
        exit(1);
    }
    s->data[++(s->top)] = node;
}

// Pop a node from the stack
Node* pop(Stack* s) {
    if (isEmpty(s)) {
        printf("Error: Stack underflow\n");
        exit(1);
    }
    return s->data[(s->top)--];
}

// Peek at the top node without popping
Node* peek(Stack* s) {
    return isEmpty(s) ? NULL : s->data[s->top];
}

// Create a new node with a given value
Node* createNode(const char* val) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }
    strncpy(node->value, val, sizeof(node->value) - 1);
    node->value[sizeof(node->value) - 1] = '\0';
    node->left = node->right = NULL;
    return node;
}

// ===============================
// Helper Functions for Parsing
// ===============================

// Check if token is an operator
bool isOperator(const char* token) {
    return strlen(token) == 1 && strchr("+-*/", token[0]);
}

// Check if token is an operand (variable or number)
bool isOperand(const char* token) {
    for (int i = 0; token[i]; i++) {
        if (!isalnum(token[i])) return false;
    }
    return strlen(token) > 0;
}

// Check if token is valid (operator or operand)
bool isValidToken(const char* token) {
    return isOperand(token) || isOperator(token);
}


// Tokenize the input string into an array of tokens
int tokenize(char* input, Token* tokens, int maxTokens) {
    int tokenCount = 0;
    char* token = strtok(input, " ");
    while (token && tokenCount < maxTokens) {
        if (!isValidToken(token)) {
            printf("Error: Invalid token '%s'\n", token);
            return -1;
        }
        tokens[tokenCount].value = strdup(token);
        if (!tokens[tokenCount].value) {
            printf("Error: Memory allocation failed\n");
            return -1;
        }
        tokens[tokenCount].isOperator = isOperator(token);
        tokenCount++;
        token = strtok(NULL, " ");
    }
    if (tokenCount == 0) {
        printf("Error: No valid tokens found\n");
        return -1;
    }
    return tokenCount;
}

// ==========================
// Check if the prefix expression is valid
// Valid prefix: #operands = #operators + 1
// ==========================
// Validate prefix expression: #operands = #operators + 1
int validatePrefix(Token* tokens, int tokenCount) {
    int operandCount = 0;
    int operatorCount = 0;
    int balance = 0;

    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].isOperator) {
            operatorCount++;
            balance -= 1;
        } else {
            operandCount++;
            balance += 1;
        }
    }

    if (operandCount != operatorCount + 1 || balance != 1) {
        printf("Error: Invalid prefix expression - incorrect number of operands/operators\n");
        return 0;
    }

    return 1;
}

// ==========================
// Build an expression tree recursively from prefix tokens
// Pre-order: Root -> Left -> Right
// ==========================
Node* buildTreeFromPrefix(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) {
        return NULL;
    }

    // Create current node
    Node* node = createNode(tokens[*index].value);
    (*index)++;

    // If it's an operator, build left and right subtrees recursively
    if (isOperator(node->value)) {
        node->left = buildTreeFromPrefix(tokens, index, tokenCount);
        node->right = buildTreeFromPrefix(tokens, index, tokenCount);
    }

    return node;
}

// ==========================
// Inorder traversal: Left, Root, Right
// Used for printing infix notation (with parentheses)
// ==========================
void inorder(Node* root) {
    if (root) {
        if (isOperator(root->value)) printf("( ");
        inorder(root->left);
        printf("%s ", root->value);
        inorder(root->right);
        if (isOperator(root->value)) printf(")");
    }
}

// ==========================
// Postorder traversal: Left, Right, Root
// Used for postfix conversion
// ==========================
void postorder(Node* root) {
    if (root) {
        postorder(root->left);
        postorder(root->right);
        printf("%s ", root->value);
        if (root->left || root->right) printf(" ");
    }
}

// ==========================
// Recursively free memory allocated for the tree
// ==========================
void freeTree(Node* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// ==========================
// Free all dynamically allocated token strings
// ==========================
void freeTokens(Token* tokens, int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        free(tokens[i].value);
    }
}

// =====================================
// Main Program Entry Point
// =====================================
int main(int argc, char *argv[]) {
    char input[100];
    Token tokens[100];

    // Argument check
    if (argc != 3) {
        printf("Error: Please provide a prefix expression and conversion type\n");
        printf("Usage: %s <prefix_expression> <conversion_type>\n", argv[0]);
        printf("Conversion types: 'postfix' or 'infix'\n");
        return 1;
    }

    // Copy input safely
    strncpy(input, argv[1], sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';

    // Tokenize input
    int tokenCount = tokenize(input, tokens, 100);
    if (tokenCount < 0) {
        return 1;
    }

    // Validate prefix expression structure
    if (!validatePrefix(tokens, tokenCount)) {
        freeTokens(tokens, tokenCount);
        return 1;
    }

    // Build expression tree from prefix tokens
    int index = 0;
    Node* root = buildTreeFromPrefix(tokens, &index, tokenCount);
    if (index != tokenCount) {
        printf("Error: Invalid prefix expression - extra tokens\n");
        freeTree(root);
        freeTokens(tokens, tokenCount);
        return 1;
    }

    // Print based on conversion type
    printf("\n");
    if (strcmp(argv[2], "postfix") == 0) {
        printf("Postfix Expression: ");
        postorder(root);
    } else if (strcmp(argv[2], "infix") == 0) {
        printf("Infix Expression: ");
        inorder(root);
    } else {
        printf("Error: Invalid conversion type. Use 'postfix' or 'infix'\n");
        freeTree(root);
        freeTokens(tokens, tokenCount);
        return 1;
    }
    printf("\n");

    // Cleanup memory
    freeTree(root);
    freeTokens(tokens, tokenCount);
    return 0;
}