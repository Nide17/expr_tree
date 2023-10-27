## expr_tree

__INTRODUCTION__

The "expr_tree" is a C program for creating and evaluating expression trees. It provides the capability to handle arbitrary mathematical expressions through dynamically allocated trees. This program allows to build and manipulate expression trees for evaluation and conversion to human-readable string representations. The program is implemented in C and uses the system functions to achieve the assignment's objectives.

__DESCRIPTION__

This "expr_tree" program enables the creation and manipulation of expression trees, which consist of nodes representing values and mathematical operations. The following node types are supported:

- VALUE: Represents a leaf node with a numeric value.
- UNARY_NEGATE: Represents a unary negation operation.
- OP_ADD: Represents addition.
- OP_SUB: Represents subtraction.
- OP_MUL: Represents multiplication.
- OP_DIV: Represents division.
- OP_POWER: Represents exponentiation.

The program provides functions for creating, evaluating, and converting expression trees into human-readable strings.

**FUNCTIONS**

```ExprTree ET_value(double value)```
Creates a value node on the tree, representing a leaf node with a numeric value.

```ExprTree ET_node(ExprNodeType op, ExprTree left, ExprTree right)```
Creates an interior node on the tree, representing an arithmetic operation.

```void ET_free(ExprTree tree)```
Destroys an expression tree, freeing all allocated memory.

```int ET_count(ExprTree tree)```
Returns the number of nodes in the tree, including both leaf and interior nodes.

```int ET_depth(ExprTree tree)```
Returns the maximum depth of the tree.

```double ET_evaluate(ExprTree tree)```
Evaluates an expression tree and returns the computed value.

```size_t writeValueToBuffer(double number, char *buf, size_t buf_sz)```
A helper function to write a double value to a buffer.

```size_t ET_tree2string(ExprTree tree, char *buf, size_t buf_sz)```
Converts an expression tree into a printable ASCII string stored in a buffer.

Example:
```c
#include <stdio.h>
#include "expr_tree.h"

int main() {
    // Create an expression tree: 3.0 + 4.0
    ExprTree tree = ET_node(OP_ADD, ET_value(3.0), ET_value(4.0));

    // Evaluate the expression
    double result = ET_evaluate(tree);
    printf("Result: %f\n", result);

    // Convert the expression to a string
    char buf[100];
    size_t length = ET_tree2string(tree, buf, sizeof(buf));
    printf("Expression: %s\n", buf);

    // Free the memory used by the tree
    ET_free(tree);

    return 0;
} 
```

__USAGE__

To use and test the "expr_tree" program, you first need to compile it using the **make** command. This command uses a C compiler, such as GCC and runs the command **gcc -Wall -Werror -g -fsanitize=address expr_tree.c expr_tree.h et_test.c -lm -o et_test**. After compilation, you can run it by typing **"./et_test"** to the console. For further testing, add test cases to the "et_test.c" file by making with expression trees, performing operations, evaluating expressions, and converting them to strings.

__IMPORTANCE__

The "expr_tree" program can be used in working with math problems on computers. It can calculate math problems, like adding or multiplying numbers, and it can also show us the math problems in a way we can understand. This is handy for things like making calculators or solving math puzzles.

__KEYWORDS__

<mark>ISSE</mark>     <mark>CMU</mark>     <mark>Assignment8</mark>     <mark>expr_tree</mark>     <mark>C Programming</mark>     <mark>Recursion</mark>

__AUTHOR__

howdy@sleepymoose.net (Howdy Pierce)

__CONTRIBUTOR__

parmenin (Niyomwungeri Parmenide ISHIMWE) at CMU-Africa - MSIT

__DATE__

 October 28, 2023