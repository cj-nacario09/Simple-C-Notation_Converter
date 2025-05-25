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

// Check if token is valid (operator, operand, or parenthesis)
bool isValidToken(const char* token) {
    return isOperand(token) || isOperator(token) || strcmp(token, "(") == 0 || strcmp(token, ")") == 0;
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

// Validate the postfix expression for correctness
int validatePostfix(Token* tokens, int tokenCount) {
    int operandCount = 0;

    for (int i = 0; i < tokenCount; i++) {
        if (!tokens[i].isOperator) {
            operandCount++;
        } else {
            if (operandCount < 2) {
                printf("Error: Invalid postfix expression - insufficient operands for operator '%s'\n", tokens[i].value);
                return 0;
            }
            operandCount--;
        }
    }

    if (operandCount != 1) {
        printf("Error: Invalid postfix expression - too %s operands\n",
               operandCount > 1 ? "many" : "few");
        return 0;
    }

    return 1;
}

// Build an expression tree from postfix tokens
Node* buildTree(Token* tokens, int tokenCount) {
    Stack s;
    initStack(&s);

    for (int i = 0; i < tokenCount; i++) {
        Node* node = createNode(tokens[i].value);

        if (!tokens[i].isOperator) {
            push(&s, node);
        } else {
            if (isEmpty(&s) || peek(&s)->right) {
                printf("Error: Invalid postfix expression - stack underflow\n");
                freeTree(node);
                while (!isEmpty(&s)) freeTree(pop(&s));
                return NULL;
            }
            node->right = pop(&s);
            if (isEmpty(&s)) {
                printf("Error: Invalid postfix expression - stack underflow\n");
                freeTree(node);
                return NULL;
            }
            node->left = pop(&s);
            push(&s, node);
        }
    }

    if (isEmpty(&s)) {
        printf("Error: Invalid postfix expression - no result\n");
        return NULL;
    }
    Node* root = pop(&s);
    if (!isEmpty(&s)) {
        printf("Error: Invalid postfix expression - too many operands\n");
        freeTree(root);
        while (!isEmpty(&s)) freeTree(pop(&s));
        return NULL;
    }
    return root;
}

// =====================================
// Tree Traversal Functions
// =====================================

// Inorder traversal to print infix notation
void inorder(Node* root) {
    if (root) {
        if (isOperator(root->value)) printf("( ");
        inorder(root->left);
        printf("%s ", root->value);
        if (root->left || root->right)
        inorder(root->right);
        if (isOperator(root->value)) printf(")");
    }
}

// Preorder traversal to print prefix notation
void preorder(Node* root) {
    if (root) {
        printf("%s ", root->value);
        if (root->left || root->right)
        preorder(root->left);
        preorder(root->right);
    }
}

// Free memory used by the expression tree
void freeTree(Node* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// Free memory used by tokens
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

    // Ensure correct usage
    if (argc != 3) {
        printf("Error: Please provide a postfix expression and conversion type\n");
        printf("Usage: %s <postfix_expression> <conversion_type>\n", argv[0]);
        printf("Conversion types: 'infix' or 'prefix'\n");
        return 1;
    }

    // Copy expression from command-line argument
    strncpy(input, argv[1], sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';

    // Tokenize input
    int tokenCount = tokenize(input, tokens, 100);
    if (tokenCount < 0) {
        return 1;
    }

    // Validate postfix format
    if (!validatePostfix(tokens, tokenCount)) {
        freeTokens(tokens, tokenCount);
        return 1;
    }

    // Build expression tree from tokens
    Node* root = buildTree(tokens, tokenCount);
    if (!root) {
        freeTokens(tokens, tokenCount);
        return 1;
    }

    printf("\n");
    if (strcmp(argv[2], "infix") == 0) {
        printf("Infix Expression: ");
        inorder(root);
    } else if (strcmp(argv[2], "prefix") == 0) {
        printf("Prefix Expression: ");
        preorder(root);
    } else {
        printf("Error: Invalid conversion type. Use 'infix' or 'prefix'\n");
        freeTree(root);
        freeTokens(tokens, tokenCount);
        return 1;
    }
    printf("\n");

    // Cleanup
    freeTree(root);
    freeTokens(tokens, tokenCount);
    return 0;
}