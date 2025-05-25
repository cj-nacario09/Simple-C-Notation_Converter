//DISCRETE STRUCTURE 2 - FINAL PROJECT "NOTATION CONVERTION FOR INFIX, PREFIX AND POSTFIX"
// BSCS 2A

/*
MEMBERS: 
    BRUTAS,
    MORGA,
    NACARIO,
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX 100  // Maximum stack size

// Node for expression tree
typedef struct Node {
    char value[10];           // Stores operator or operand
    struct Node* left;        // Left child
    struct Node* right;       // Right child
} Node;

// Token structure: holds a string and whether it is an operator
typedef struct {
    char* value;
    bool isOperator;
} Token;

// Stack structure used for building trees
typedef struct Stack {
    Node* data[MAX];
    int top;
} Stack;

// ----------- Stack Operations -----------

// Initializes the stack
void initStack(Stack* s) { s->top = -1; }

// Checks if the stack is empty
bool isEmpty(Stack* s) { return s->top == -1; }

// Push a node onto the stack, with overflow error check
void push(Stack* s, Node* node) {
    if (s->top >= MAX - 1) {
        printf("Error: Stack overflow\n");
        exit(1);
    }
    s->data[++(s->top)] = node;
}

// Pops a node from the stack, with underflow error check
Node* pop(Stack* s) {
    if (isEmpty(s)) {
        printf("Error: Stack underflow\n");
        exit(1);
    }
    return s->data[(s->top)--];
}

// Peeks at the top node of the stack without removing it
Node* peek(Stack* s) {
    return isEmpty(s) ? NULL : s->data[s->top];
}

// Creates a new expression tree node
Node* createNode(const char* val) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }
    strncpy(node->value, val, sizeof(node->value) - 1);
    node->value[sizeof(node->value) - 1] = '\0'; // Ensure null termination
    node->left = node->right = NULL;
    return node;
}

// Checks if a token is an operator
bool isOperator(const char* token) {
    return strlen(token) == 1 && strchr("+-*/", token[0]);
}

// Checks if a token is a valid operand (alphanumeric string)
bool isOperand(const char* token) {
    for (int i = 0; token[i]; i++) {
        if (!isalnum(token[i])) return false;
    }
    return strlen(token) > 0;
}

// Verifies if a token is valid (operand, operator, or parenthesis)
bool isValidToken(const char* token) {
    return isOperand(token) || isOperator(token) ||
           strcmp(token, "(") == 0 || strcmp(token, ")") == 0;
}

// Splits the input string into tokens and identifies operators
int tokenize(char* input, Token* tokens, int maxTokens) {
    int tokenCount = 0;
    char* token = strtok(input, " ");
    while (token && tokenCount < maxTokens) {
        if (!isValidToken(token)) {
            printf("Error: Invalid token '%s'\n", token);
            return -1;
        }
        tokens[tokenCount].value = strdup(token); // Dynamic copy of token
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

// Frees memory allocated for tokens
void freeTokens(Token* tokens, int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        free(tokens[i].value);
    }
}

// Recursively frees memory for the expression tree
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// ----------- Tree Traversal -----------

// Preorder: root-left-right (used for prefix output)
void preorder(Node* root) {
    if (root) {
        printf("%s ", root->value);
        preorder(root->left);
        preorder(root->right);
    }
}

// Inorder: left-root-right (used for infix output with parentheses)
void inorder(Node* root) {
    if (root) {
        if (root->left && root->right) printf("( ");
        inorder(root->left);
        printf("%s ", root->value);
        inorder(root->right);
        if (root->left && root->right) printf(") ");
    }
}

// Postorder: left-right-root (used for postfix output)
void postorder(Node* root) {
    if (root) {
        postorder(root->left);
        postorder(root->right);
        printf("%s ", root->value);
    }
}

// ----------- PREFIX Handling -----------

// Recursively validate prefix structure (each operator must have two children)
bool validatePrefixStructure(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return false;

    if (tokens[*index].isOperator) {
        (*index)++;
        return validatePrefixStructure(tokens, index, tokenCount) &&
               validatePrefixStructure(tokens, index, tokenCount);
    } else {
        (*index)++;
        return true;
    }
}

// Checks if entire prefix expression is valid
int validatePrefix(Token* tokens, int tokenCount) {
    int index = 0;
    if (!validatePrefixStructure(tokens, &index, tokenCount)) return 0;
    return index == tokenCount;
}

// Builds tree from prefix tokens using recursion
Node* buildTreeFromPrefix(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return NULL;
    Node* node = createNode(tokens[*index].value);
    (*index)++;
    if (isOperator(node->value)) {
        node->left = buildTreeFromPrefix(tokens, index, tokenCount);
        node->right = buildTreeFromPrefix(tokens, index, tokenCount);
    }
    return node;
}

// ----------- POSTFIX Handling -----------

// Validates if postfix expression has enough operands for each operator
int validatePostfix(Token* tokens, int tokenCount) {
    int operandCount = 0;
    for (int i = 0; i < tokenCount; i++) {
        if (!tokens[i].isOperator) operandCount++;
        else {
            if (operandCount < 2) return 0;
            operandCount--;
        }
    }
    return operandCount == 1;
}

// Builds tree from postfix tokens using stack
Node* buildTreeFromPostfix(Token* tokens, int tokenCount) {
    Stack s;
    initStack(&s);
    for (int i = 0; i < tokenCount; i++) {
        Node* node = createNode(tokens[i].value);
        if (!tokens[i].isOperator) {
            push(&s, node);
        } else {
            node->right = pop(&s);
            node->left = pop(&s);
            push(&s, node);
        }
    }
    return pop(&s);
}

// ----------- INFIX Handling -----------

// Defines operator precedence for infix expressions
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Converts infix expression string into an expression tree
Node* buildTreeFromInfix(const char* expr) {
    Stack ops, nodes;
    initStack(&ops);
    initStack(&nodes);

    char exprCopy[256];
    strncpy(exprCopy, expr, sizeof(exprCopy) - 1);
    exprCopy[sizeof(exprCopy) - 1] = '\0';

    char* tok = strtok(exprCopy, " ");
    while (tok) {
        if (!isValidToken(tok) && strcmp(tok, "(") != 0 && strcmp(tok, ")") != 0) {
            printf("Error: Invalid token '%s'\n", tok);
            return NULL;
        }
        if (isOperand(tok)) {
            push(&nodes, createNode(tok));
        } else if (strcmp(tok, "(") == 0) {
            push(&ops, createNode(tok));
        } else if (strcmp(tok, ")") == 0) {
            // Process until opening parenthesis
            bool foundOpen = false;
            while (!isEmpty(&ops)) {
                Node* top = pop(&ops);
                if (strcmp(top->value, "(") == 0) {
                    foundOpen = true;
                    free(top);
                    break;
                }
                if (isEmpty(&nodes)) {
                    printf("Error: Too few operands for operator '%s'\n", top->value);
                    free(top);
                    return NULL;
                }
                Node* right = pop(&nodes);
                if (isEmpty(&nodes)) {
                    printf("Error: Too few operands for operator '%s'\n", top->value);
                    free(top);
                    free(right);
                    return NULL;
                }
                Node* left = pop(&nodes);
                top->left = left;
                top->right = right;
                push(&nodes, top);
            }
            if (!foundOpen) {
                printf("Error: Unbalanced parentheses\n");
                return NULL;
            }
        } else if (isOperator(tok)) {
            while (!isEmpty(&ops) && isOperator(peek(&ops)->value) &&
                   precedence(peek(&ops)->value[0]) >= precedence(tok[0])) {
                Node* op = pop(&ops);
                if (isEmpty(&nodes)) {
                    printf("Error: Too few operands for operator '%s'\n", op->value);
                    free(op);
                    return NULL;
                }
                Node* right = pop(&nodes);
                if (isEmpty(&nodes)) {
                    printf("Error: Too few operands for operator '%s'\n", op->value);
                    free(op);
                    free(right);
                    return NULL;
                }
                Node* left = pop(&nodes);
                op->left = left;
                op->right = right;
                push(&nodes, op);
            }
            push(&ops, createNode(tok));
        }
        tok = strtok(NULL, " ");
    }

    // Final merge of remaining operators
    while (!isEmpty(&ops)) {
        Node* op = pop(&ops);
        if (strcmp(op->value, "(") == 0 || strcmp(op->value, ")") == 0) {
            printf("Error: Unbalanced parentheses\n");
            free(op);
            return NULL;
        }
        if (isEmpty(&nodes)) {
            printf("Error: Too few operands for operator '%s'\n", op->value);
            free(op);
            return NULL;
        }
        Node* right = pop(&nodes);
        if (isEmpty(&nodes)) {
            printf("Error: Too few operands for operator '%s'\n", op->value);
            free(op);
            free(right);
            return NULL;
        }
        Node* left = pop(&nodes);
        op->left = left;
        op->right = right;
        push(&nodes, op);
    }

    // Only one tree should remain
    if (nodes.top != 0) {
        printf("Error: Too many operands\n");
        return NULL;
    }

    return pop(&nodes);
}

// ----------- MAIN Function -----------

// Entry point for the program
int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("\nFor Linux:");
        printf("\nUsage: ./<program> \"<expression>\" <input_type> <output_type>\n");

        printf("\nFor Windows:");
        printf("\nUsage: <program.exe> \"<expression>\" <input_type> <output_type>\n");

        printf("\nTypes:\n  infix\n  prefix\n  postfix\n");

        printf("\nExamples:\n");
        printf("  ./program \"a + b * c\" infix postfix\n");
        printf("  ./program \"+ a * b c\" prefix infix\n");
        printf("  ./program \"a b c * +\" postfix infix\n");

        printf("\nErrors and Format Rules:\n");

        printf("\n[ General Errors ]\n");
        printf("  - Error: No valid tokens found\n");
        printf("    ---> Make sure your expression has valid operands/operators separated by spaces.\n");
        printf("  - Error: Invalid token '<token>'\n");
        printf("    ---> Expression contains an invalid character or unrecognized symbol.\n");
        printf("  - Error: Unknown output type\n");
        printf("    ---> The third argument must be one of: infix, prefix, postfix\n");

        printf("\n[ Infix Expression Rules ]\n");
        printf("  - Must be properly parenthesized if necessary (e.g., ( a + b ) * c)\n");
        printf("  - Operators: + - * /\n");
        printf("  - Operands must be alphanumeric (e.g., a, b1, X99)\n");
        printf("  - Error: Infix expression cannot start or end with an operator\n");
        printf("  - Error: Unbalanced parentheses\n");
        printf("  - Error: Too few operands for operator '+' or other operator\n");
        printf("  - Error: Too many operands\n");

        printf("\n[ Prefix Expression Rules ]\n");
        printf("  - Format: <operator> <operand1> <operand2> [...]\n");
        printf("  - Each operator must have exactly two operands\n");
        printf("  - Error: Invalid prefix expression format\n");
        printf("    ---> Too many or too few operands for the given operators\n");

        printf("\n[ Postfix Expression Rules ]\n");
        printf("  - Format: <operand1> <operand2> <operator>\n");
        printf("  - Each operator must apply to the two previous operands\n");
        printf("  - Error: Invalid postfix expression format\n");
        printf("    ---> Too many or too few operands for the given operators\n");

        printf("\nHelpful Tip:\n");
        printf("  All expressions must be space-separated.\n");
        printf("  Use double quotes around expressions to avoid shell issues.\n");

        printf("\n");
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "--guide") == 0) {
        printf("\nFor Linux:");
        printf("\nUsage: ./<program> \"<expression>\" <input_type> <output_type>\n");

        printf("\nFor Windows:");
        printf("\nUsage: <program.exe> \"<expression>\" <input_type> <output_type>\n");

        printf("\nTypes:\n  infix\n  prefix\n  postfix\n");
        printf("\nExample: ./Convert \"a + b * c\" infix postfix\n");
        printf("\nExample: Convert.exe \"a + b * c\" infix postfix\n\n");
        return 0;
    }

    if (argc != 4) {
        printf("\nHi, To convert a Notation please Enter \"--guide\" or \"--help\".\n");
        printf("\nFor Linux:");
        printf("\n\tUsage: ./<program_name> \"--help\".\n");
        printf("\t\t---> For user error and format rules\n");
        printf("\tUsage: ./<program_name> \"--guide\".\n");
        printf("\t\t---> How to run this program\n");

        printf("\nFor Windows:");
        printf("\n\tUsage: <program_name.exe> \"--help\".\n");
        printf("\t\t---> For user error and format rules\n");
        printf("\tUsage: <program_name.exe> \"--guide\".\n");
        printf("\t\t---> How to run this program\n");

        printf("\nHelpful Tip:\n");
        printf("\n\tRead the help and guide first to properly run this program. Thank you!!\n");
        return 1;
    }

    char input[256];
    strncpy(input, argv[1], sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';

    const char* inputType = argv[2];
    const char* outputType = argv[3];
    Node* root = NULL;
    Token tokens[100];
    int tokenCount = 0;

    if (strcmp(inputType, "infix") == 0) {
        // Early check to avoid invalid infix format
        char firstChar = input[0];
        char lastChar = input[strlen(input) - 1];
        if (firstChar == '+' || firstChar == '-' || firstChar == '*' || firstChar == '/' ||
            lastChar == '+' || lastChar == '-' || lastChar == '*' || lastChar == '/') {
            printf("\nError: Infix expression cannot start or end with an operator\n");
            return 1;
        }
        root = buildTreeFromInfix(input);
        if (!root) return 1;
    } else {
        tokenCount = tokenize(input, tokens, 100);
        if (tokenCount < 0) return 1;

        if (strcmp(inputType, "prefix") == 0) {
            if (!validatePrefix(tokens, tokenCount)) {
                printf("\nError: Invalid prefix expression format\n");
                printf("\nThere's seems to be a problem, To convert a Notation please press \"--help\".\n");
                printf("Usage: ./<program_name> \"--help\".\n\n");
                freeTokens(tokens, tokenCount);
                return 1;
            }
            int index = 0;
            root = buildTreeFromPrefix(tokens, &index, tokenCount);
        } else if (strcmp(inputType, "postfix") == 0) {
            if (!validatePostfix(tokens, tokenCount)) {
                printf("\nError: Invalid postfix expression format\n");
                printf("\nThere's seems to be a problem, To convert a Notation please press \"--help\".\n");
                printf("Usage: ./<program_name> \"--help\".\n\n");
                freeTokens(tokens, tokenCount);
                return 1;
            }
            root = buildTreeFromPostfix(tokens, tokenCount);
        }
    }

    // Output conversion based on user's choice
    printf("\n");
    if (strcmp(outputType, "infix") == 0) {
        printf("Infix Expression: ");
        inorder(root);
    } else if (strcmp(outputType, "prefix") == 0) {
        printf("Prefix Expression: ");
        preorder(root);
    } else if (strcmp(outputType, "postfix") == 0) {
        printf("Postfix Expression: ");
        postorder(root);
    } else {
        printf("\nError: Unknown output type\n");
        printf("\nThere's seems to be a problem, To convert a Notation please press \"--help\".\n");
        printf("Usage: ./<program_name> \"--help\".\n\n");
        freeTree(root);
        return 1;
    }
    printf("\n");

    // Cleanup
    freeTree(root);
    if (strcmp(inputType, "prefix") == 0 || strcmp(inputType, "postfix") == 0)
        freeTokens(tokens, tokenCount);
    return 0;
}
