# Notation Conversion Project

This project, developed as a final project for Discrete Structures 2 by BSCS 2A students, implements a C program to convert mathematical expressions between **infix**, **prefix**, and **postfix** notations. It uses an expression tree to facilitate the conversion process.

## Team Members
- Louise Brutas
- Christian Morga
- Carl Joseph Nacario

## Project Overview
The program takes a mathematical expression as input, along with its input type (infix, prefix, or postfix) and the desired output type. It constructs an expression tree from the input and uses tree traversals (preorder, inorder, postorder) to output the expression in the requested notation.

### Features
- Supports conversion between infix, prefix, and postfix notations.
- Validates input expressions for correctness.
- Handles operators (`+`, `-`, `*`, `/`) and alphanumeric operands.
- Provides detailed error messages for invalid inputs.
- Includes a help guide (`--help`) and usage guide (`--guide`).

## Requirements
- C compiler (e.g., `gcc`)
- Standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `stdbool.h`)

## How to Run
### Linux
```bash
gcc -o program notation_conversion.c
./program "<expression>" <input_type> <output_type>
```

### Windows
```bash
gcc -o program.exe notation_conversion.c
program.exe "<expression>" <input_type> <output_type>
```

### Input Types
- `infix`: e.g., `( a + b ) * c`
- `prefix`: e.g., `* + a b c`
- `postfix`: e.g., `a b + c *`

### Output Types
- `infix`: Outputs expression with parentheses as needed.
- `prefix`: Outputs operator followed by operands.
- `postfix`: Outputs operands followed by operator.

### Examples
```bash
./program "a + b * c" infix postfix
# Output: Postfix Expression: a b c * +

./program "+ a * b c" prefix infix
# Output: Infix Expression: ( a + ( b * c ) )

./program "a b c * +" postfix infix
# Output: Infix Expression: ( a + ( b * c ) )
```

### Help and Guide
- Run `./program --help` for detailed usage instructions and error explanations.
- Run `./program --guide` for a quick usage guide.

## Token Parsing
The program uses a tokenization process to break down the input expression into manageable components:

1. **Input Splitting**:
   - The `tokenize` function splits the input string by spaces using `strtok`.
   - Each token is validated using `isValidToken`, which checks if it’s an operand (alphanumeric), operator (`+`, `-`, `*`, `/`), or parenthesis.
   - Tokens are stored in a `Token` structure, which includes the token’s value and a boolean (`isOperator`) indicating whether it’s an operator.

2. **Validation**:
   - Tokens are checked for validity using `isOperand` (ensures alphanumeric characters) and `isOperator` (checks for valid operators).
   - Invalid tokens trigger an error message and program termination.

3. **Memory Management**:
   - Each token’s value is dynamically allocated using `strdup` to ensure safe storage.
   - The `freeTokens` function deallocates memory to prevent leaks.

**Example**:
For input `"a + b * c"`:
- Tokens: `["a", "+", "b", "*", "c"]`
- `isOperator` flags: `[false, true, false, true, false]`

## Expression Tree Construction
The program builds an expression tree to represent the input expression, which is then traversed to produce the output notation. The construction process varies by input type:

### Infix (`buildTreeFromInfix`)
- **Algorithm**: Uses two stacks—one for operators/parentheses (`ops`) and one for tree nodes (`nodes`).
- **Process**:
  1. Tokens are processed sequentially:
     - **Operands**: Create a tree node and push it onto the `nodes` stack.
     - **Opening parenthesis (`(`)**: Pushed onto the `ops` stack.
     - **Closing parenthesis (`)`)**: Pop operators until the matching `(` is found, creating tree nodes by combining two operands with an operator.
     - **Operators**: Pop higher or equal precedence operators from `ops`, create tree nodes, and push the current operator onto `ops`.
  2. After processing all tokens, remaining operators in `ops` are popped to complete the tree.
- **Error Handling**:
  - Checks for unbalanced parentheses.
  - Ensures sufficient operands for each operator.
  - Validates that the expression doesn’t start or end with an operator.
- **Example**: For `( a + b ) * c`:
  - Tree: Root is `*`, with left child `+` (having children `a` and `b`) and right child `c`.

### Prefix (`buildTreeFromPrefix`)
- **Algorithm**: Recursively processes tokens to build the tree.
- **Process**:
  1. Start at the first token and create a node.
  2. If the token is an operator, recursively build left and right subtrees.
  3. If the token is an operand, return the node as a leaf.
  4. The `validatePrefixStructure` function ensures each operator has exactly two operands.
- **Example**: For `+ a * b c`:
  - Tree: Root is `+`, with left child `a` and right child `*` (having children `b` and `c`).

### Postfix (`buildTreeFromPostfix`)
- **Algorithm**: Uses a stack to build the tree iteratively.
- **Process**:
  1. For each token:
     - **Operand**: Create a node and push it onto the stack.
     - **Operator**: Pop two nodes (right then left child), create an operator node, and push it back onto the stack.
  2. The `validatePostfix` function ensures there are enough operands for each operator, with exactly one node remaining at the end.
- **Example**: For `a b c * +`:
  - Tree: Root is `+`, with left child `a` and right child `*` (having children `b` and `c`).

## Tree Traversals
The constructed expression tree is traversed to generate the output:
- **Infix (`inorder`)**: Left-root-right traversal, adding parentheses for operator nodes with children.
- **Prefix (`preorder`)**: Root-left-right traversal.
- **Postfix (`postorder`)**: Left-right-root traversal.

## Error Handling
- **Invalid Tokens**: Detected during tokenization.
- **Unbalanced Parentheses**: Checked in infix processing.
- **Invalid Expression Format**: Validated for prefix and postfix inputs.
- **Stack Overflow/Underflow**: Checked during stack operations.
- **Memory Allocation**: Errors trigger program termination.

## Notes
- Expressions must be space-separated (e.g., `a + b` not `a+b`).
- Use double quotes around expressions in the command line to avoid shell issues.
- Operands must be alphanumeric (e.g., `a`, `b1`, `X99`).
- The program assumes well-formed input for simplicity; complex expressions may require explicit parentheses in infix notation.

## License
This project is licensed under the MIT License.