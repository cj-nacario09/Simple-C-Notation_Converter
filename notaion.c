#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Define structure of a tree node
typedef struct Node {
    char data;
    struct Node *left, *right;
} Node;

// Stack for tree nodes
typedef struct Stack {
    Node* data[100];
    int top;
} Stack;

// Initialize stack
void initStack(Stack *s) {
    s->top = -1;
}

// Push node onto stack
void push(Stack *s, Node* node) {
    s->data[++(s->top)] = node;
}

// Pop node from stack
Node* pop(Stack *s) {
    return s->data[(s->top)--];
}

// Check if character is operator
int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// Create a new node
Node* createNode(char data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

// Build expression tree from postfix expression
Node* buildTree(char postfix[]) {
    Stack s;
    initStack(&s);

    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];

        // If operand, create node and push
        if (isalnum(c)) {
            Node* node = createNode(c);
            push(&s, node);
        }
        // If operator, pop two nodes and create new node
        else if (isOperator(c)) {
            Node* node = createNode(c);
            node->right = pop(&s);
            node->left = pop(&s);
            push(&s, node);
        }
    }

    return pop(&s);  // Root of the expression tree
}

// Traversal functions
void inorder(Node* root) {
    if (root) {
        if (isOperator(root->data)) printf("(");
        inorder(root->left);
        printf("%c", root->data);
        inorder(root->right);
        if (isOperator(root->data)) printf(")");
    }
}

void preorder(Node* root) {
    if (root) {
        printf("%c", root->data);
        preorder(root->left);
        preorder(root->right);
    }
}

void postorder(Node* root) {
    if (root) {
        postorder(root->left);
        postorder(root->right);
        printf("%c", root->data);
    }
}

// Free memory
void freeTree(Node* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

int main() {
    char postfix[100];
    printf("Enter postfix expression (single characters, no spaces): ");
    scanf("%s", postfix);

    Node* root = buildTree(postfix);

    printf("\nInfix Expression: ");
    inorder(root);

    printf("\nPrefix Expression: ");
    preorder(root);

    printf("\nPostfix Expression: ");
    postorder(root);

    freeTree(root);
    return 0;
}
