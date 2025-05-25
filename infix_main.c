#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
    strcpy(node->value, val);
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

// Get precedence of operators
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// =====================================
// Build Expression Tree from Infix
// =====================================
Node* buildTreeFromInfix(const char* expr) {
    Stack ops, nodes;
    initStack(&ops);
    initStack(&nodes);

    // Make a copy of the input expression to tokenize
    char exprCopy[256];
    strncpy(exprCopy, expr, sizeof(exprCopy) - 1);
    exprCopy[sizeof(exprCopy) - 1] = '\0';

    char* tok = strtok(exprCopy, " ");
    while (tok) {
        // Validate token
        if (!isValidToken(tok) && strcmp(tok, "(") != 0 && strcmp(tok, ")") != 0) {
            printf("Error: Invalid token '%s'\n", tok);
            return NULL;
        }

        if (isOperand(tok)) {
            // Operand: create node and push to nodes stack
            push(&nodes, createNode(tok));
        } else if (strcmp(tok, "(") == 0) {
            // Left parenthesis: push to ops stack
            push(&ops, createNode(tok));
        } else if (strcmp(tok, ")") == 0) {
            // Right parenthesis: pop until matching "("
            while (!isEmpty(&ops) && strcmp(peek(&ops)->value, "(") != 0) {
                Node* op = pop(&ops);
                Node* right = pop(&nodes);
                Node* left = pop(&nodes);
                if (!left || !right) {
                    printf("Error: Stack underflow\n");
                    return NULL;
                }
                op->left = left;
                op->right = right;
                push(&nodes, op);
            }
            if (isEmpty(&ops)) {
                printf("Error: Mismatched parentheses\n");
                return NULL;
            }
            pop(&ops); // discard '('
        } else if (isOperator(tok)) {
            // Operator: pop from ops while precedence is higher or equal
            while (!isEmpty(&ops) && isOperator(peek(&ops)->value) &&
                   precedence(peek(&ops)->value[0]) >= precedence(tok[0])) {
                Node* op = pop(&ops);
                Node* right = pop(&nodes);
                Node* left = pop(&nodes);
                if (!left || !right) {
                    printf("Error: Stack underflow\n");
                    return NULL;
                }
                op->left = left;
                op->right = right;
                push(&nodes, op);
            }
            push(&ops, createNode(tok));
        }

        tok = strtok(NULL, " ");
    }

    // Process any remaining operators in ops stack
    while (!isEmpty(&ops)) {
        Node* op = pop(&ops);
        Node* right = pop(&nodes);
        Node* left = pop(&nodes);
        if (!left || !right) {
            printf("Error: Stack underflow\n");
            return NULL;
        }
        op->left = left;
        op->right = right;
        push(&nodes, op);
    }

    // At this point, nodes stack should have exactly one node (the root)
    if (nodes.top != 0) {
        printf("Error: Mismatched expression\n");
        return NULL;
    }

    return pop(&nodes);
}

// =====================================
// Tree Traversal Functions
// =====================================

// Pre-order traversal: root -> left -> right (used for prefix)
void preorder(Node* root) {
    if (!root) return;
    printf("%s ", root->value);
    preorder(root->left);
    preorder(root->right);
}

// Post-order traversal: left -> right -> root (used for postfix)
void postorder(Node* root) {
    if (!root) return;
    postorder(root->left);
    postorder(root->right);
    printf("%s ", root->value);
}

// In-order traversal: (left -> root -> right), with brackets
void inorder(Node* root) {
    if (!root) return;
    if (root->left && root->right) printf("( ");
    inorder(root->left);
    printf("%s ", root->value);
    inorder(root->right);
    if (root->left && root->right) printf(") ");
}

// Recursively free memory used by tree
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// =====================================
// Main Program Entry Point of Infix
// =====================================

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Error: Please provide an infix expression and conversion type\n");
        printf("Usage: %s \"<infix_expression>\" <conversion_type>\n", argv[0]);
        printf("Conversion types: 'postfix' or 'prefix'\n");
        return 1;
    }

    const char* inputExpr = argv[1];     // Infix expression from user
    const char* conversion = argv[2];    // Desired output format

    // Build expression tree from input
    Node* root = buildTreeFromInfix(inputExpr);
    if (!root) {
        printf("Failed to build expression tree.\n");
        return 1;
    }

    printf("\n");
    // Convert based on user input
    if (strcmp(conversion, "postfix") == 0) {
        printf("Postfix Expression: ");
        postorder(root);
    } else if (strcmp(conversion, "prefix") == 0) {
        printf("Prefix Expression: ");
        preorder(root);
    } else {
        printf("Error: Invalid conversion type. Use 'postfix' or 'prefix'\n");
        freeTree(root);
        return 1;
    }
    printf("\n");

    // Clean up
    freeTree(root);
    return 0;
}
