
/*
 * expr_tree.c
 *
 * A dynamically allocated tree to handle arbitrary arithmetic
 * expressions
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "expr_tree.h"

#define LEFT 0
#define RIGHT 1

struct _expr_tree_node
{
  ExprNodeType type;
  union
  {
    struct _expr_tree_node *child[2];
    double value;
  } n;
};

/*
 * Convert an ExprNodeType into a printable character
 *
 * Parameters:
 *   ent    The ExprNodeType to convert
 *
 * Returns: A single character representing the ent
 */
static char ExprNodeType_to_char(ExprNodeType ent)
{
  switch (ent)
  {
  case VALUE:
    return 'V';
  case UNARY_NEGATE:
    return '-';
  case OP_ADD:
    return '+';
  case OP_SUB:
    return '-';
  case OP_MUL:
    return '*';
  case OP_DIV:
    return '/';
  case OP_POWER:
    return '^';
  default:
    assert(0);
  }
}

// Documented in .h file
ExprTree ET_value(double value)
{
  ExprTree tree = malloc(sizeof(struct _expr_tree_node));
  assert(tree != NULL);
  tree->type = VALUE;
  tree->n.value = value;
  return tree;
}

// Documented in .h file
ExprTree ET_node(ExprNodeType op, ExprTree left, ExprTree right)
{
  ExprTree tree = malloc(sizeof(struct _expr_tree_node));
  assert(tree != NULL);
  tree->type = op;
  tree->n.child[LEFT] = left;
  tree->n.child[RIGHT] = right;
  return tree;
}

// Documented in .h file
void ET_free(ExprTree tree)
{
  if (tree == NULL)
  {
    return;
  }
  if (tree->type != VALUE)
  {
    ET_free(tree->n.child[LEFT]);
    ET_free(tree->n.child[RIGHT]);
  }
  free(tree);
}

// Documented in .h file
int ET_count(ExprTree tree)
{
  if (tree == NULL)
  {
    return 0;
  }
  if (tree->type == VALUE)
  {
    return 1;
  }
  return 1 + ET_count(tree->n.child[LEFT]) + ET_count(tree->n.child[RIGHT]);
}
// Documented in .h file
int ET_depth(ExprTree tree)
{
  if (tree == NULL)
  {
    return 0;
  }
  if (tree->type == VALUE)
  {
    return 1;
  }
  int left = ET_depth(tree->n.child[LEFT]);
  int right = ET_depth(tree->n.child[RIGHT]);
  return 1 + (left > right ? left : right);
}

// Documented in .h file
double ET_evaluate(ExprTree tree)
{
  if (tree == NULL)
  {
    return 0;
  }
  if (tree->type == VALUE)
  {
    return tree->n.value;
  }
  double left = ET_evaluate(tree->n.child[LEFT]);
  double right = ET_evaluate(tree->n.child[RIGHT]);
  switch (tree->type)
  {
  case UNARY_NEGATE:
    return -left;
  case OP_ADD:
    return left + right;
  case OP_SUB:
    return left - right;
  case OP_MUL:
    return left * right;
  case OP_DIV:
    return left / right;
  case OP_POWER:
    return pow(left, right);
  default:
    assert(0);
  }
}

/*
 * Convert an ExprTree into a printable ASCII string stored in buf
 *
 * Parameters:
 *   tree     The tree
 *   buf      The buffer
 *   buf_sz   Size of buffer, in bytes
 *
 * If it takes more characters to represent tree than are allowed in
 * buf, this function places as many characters as will fit into buf,
 * followed by a '$' character to indicate the result was truncated.
 * Buf will always be terminated with a \0 and in no case will this
 * function write characters beyond the end of buf.
 *
 * Returns: The number of characters written to buf, not counting the
 * \0 terminator.
 *
 *   The number of characters written to buf, not counting the '\0' terminator.
 *   If the string was truncated due to insufficient buffer space, the last
 *   character will be '$'.
 */
// size_t ET_tree2string(ExprTree tree, char *buf, size_t buf_sz);
// Write a number to the buffer, handling truncation
// Returns the number of characters written
static size_t writeNumberToBuffer(double number, char *buf, size_t buf_sz)
{
  size_t length;

  // Check if the number is an integer (has no fractional part)
  if (fmod(number, 1.0) == 0.0)
  {
    length = snprintf(buf, buf_sz, "%.0f", number); // Format as an integer
  }
  else
  {
    // Handle non-integer numbers
    char temp[buf_sz];
    snprintf(temp, buf_sz, "%f", number); // Format as a decimal with one decimal place

    // Truncate the string at the first non-zero character from the end
    char *truncatePos = temp + strlen(temp) - 1;
    while (*truncatePos == '0')
    {
      truncatePos--;
    }
    truncatePos[1] = '\0';

    int count = 0;
    for (char *ptr = temp; *ptr; ptr++)
    {
      if (*ptr == '.')
        count = strlen(temp) - (ptr - temp) - 1;
    }

    if (count < 2)
      length = snprintf(buf, buf_sz, "%.1f", number); // Format as a decimal with one decimal place
    else
      length = snprintf(buf, buf_sz, "%s", temp);
  }

  return length;
}

// Write an expression to the buffer, handling truncation
// Returns the number of characters written
static size_t writeExprToBuffer(ExprTree tree, char *buf, size_t buf_sz)
{
  size_t length = 0;

  if (tree->n.child[LEFT] == NULL)
  {
    // Leaf node: write the operator character
    length = snprintf(buf, buf_sz, "%c", ExprNodeType_to_char(tree->type));
  }
  else
  {
    char leftBuffer[buf_sz / 2];
    size_t leftLength = ET_tree2string(tree->n.child[LEFT], leftBuffer, buf_sz / 2);

    if (tree->type == UNARY_NEGATE)
    {
      if (tree->n.child[LEFT]->type == UNARY_NEGATE)
      {
        // Omit extra parentheses for consecutive unary negations
        length = snprintf(buf, buf_sz, "--%s", leftBuffer);
      }
      else
      {
        // Add parentheses for unary negation
        length = snprintf(buf, buf_sz, "(-%s)", leftBuffer);
      }
    }

    if (tree->n.child[RIGHT] == NULL)
    {
      // Unary operator or leaf node with a left child
      length = snprintf(buf, buf_sz, "(%c%s)", ExprNodeType_to_char(tree->type), leftBuffer);
    }
    else
    {
      char rightBuffer[buf_sz / 2];
      size_t rightLength = ET_tree2string(tree->n.child[RIGHT], rightBuffer, buf_sz / 2);

      // Check if parentheses are needed around the left expression
      if (tree->n.child[LEFT]->type != VALUE && tree->n.child[LEFT]->type != UNARY_NEGATE)
      {
        // Add parentheses
        char tempBuffer[buf_sz / 2];
        snprintf(tempBuffer, buf_sz / 2, "(%s)", leftBuffer);
        strcpy(leftBuffer, tempBuffer);
        leftLength += 2;
      }

      // Check if parentheses are needed around the right expression
      if (tree->n.child[RIGHT]->type != VALUE)
      {
        if (tree->type != UNARY_NEGATE && (tree->type == OP_ADD || tree->type == OP_SUB || (tree->type == OP_MUL || tree->type == OP_DIV)))
        {
          // Add parentheses
          char tempBuffer[buf_sz / 2];
          snprintf(tempBuffer, buf_sz / 2, "(%s)", rightBuffer);
          strcpy(rightBuffer, tempBuffer);
          rightLength += 2;
        }

        // Handle OP_POWER
        if (tree->type == OP_POWER)
        {
          // Add parentheses for the power operator
          char tempBuffer[buf_sz / 2];
          snprintf(tempBuffer, buf_sz / 2, "(%s)", rightBuffer);
          strcpy(rightBuffer, tempBuffer);
          rightLength += 2;
        }
      }

      // Check if there's enough space in the buffer
      if (length + leftLength + rightLength + 3 >= buf_sz)
      {
        // Handle truncation
        length = snprintf(buf, buf_sz, "%.*s$", (int)(buf_sz - 2), leftBuffer);
      }
      else
      {
        if (tree->n.child[LEFT]->type != VALUE || tree->n.child[RIGHT]->type != VALUE)
        {
          // Add parentheses around the entire expression
          length = snprintf(buf, buf_sz, "(%s %c %s)", leftBuffer, ExprNodeType_to_char(tree->type), rightBuffer);
        }
        else
        {
          length = snprintf(buf, buf_sz, "%s %c %s", leftBuffer, ExprNodeType_to_char(tree->type), rightBuffer);
        }
      }
    }
  }

  return length;
}

// Convert an ExprTree into a printable ASCII string stored in buf
// Returns the number of characters written to buf, not counting the '\0' terminator.
// If the string was truncated due to insufficient buffer space, the last character will be '$'.
size_t ET_tree2string(ExprTree tree, char *buf, size_t buf_sz)
{
  // Check if the tree is NULL
  if (tree == NULL)
  {
    // You can return an error indicator or a default string
    strncpy(buf, "Error: NULL tree", buf_sz - 1);
    buf[buf_sz - 1] = '\0';
    return strlen(buf);
  }

  // Check if the buffer size is insufficient
  if (buf_sz < 2)
  {
    buf[0] = '\0';
    return 0;
  }

  // Initialize the length variable to keep track of characters written
  size_t length = 0;

  if (tree->type == VALUE)
  {
    length = writeNumberToBuffer(tree->n.value, buf, buf_sz);
  }
  else
  {
    length = writeExprToBuffer(tree, buf, buf_sz);

    // Check if the length exceeds the buffer size and append '$' if needed
    if (length >= buf_sz)
    {
      buf[buf_sz - 1] = '$';
      // Adjust the length to the buffer size, excluding the truncation indicator
      length = buf_sz - 1;
    }
  }

  // printf("length: %zu buf_sz: %zu\n", length, buf_sz);
  return length;
}

// EACH OPERATION IS WITHIN PARENTHESES -  HANDLE THAT
// TEST EVALUATE VALUES