
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
  case OP_SUB:
  case UNARY_NEGATE:
    return '-';
  case OP_ADD:
    return '+';
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
    return;

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
    return 0;

  if (tree->type == VALUE)
    return 1;

  return 1 + ET_count(tree->n.child[LEFT]) + ET_count(tree->n.child[RIGHT]);
}

// Documented in .h file
int ET_depth(ExprTree tree)
{
  if (tree == NULL)
    return 0;

  if (tree->type == VALUE)
    return 1;

  int left = ET_depth(tree->n.child[LEFT]);
  int right = ET_depth(tree->n.child[RIGHT]);
  return 1 + (left > right ? left : right);
}

// Documented in .h file
double ET_evaluate(ExprTree tree)
{
  if (tree == NULL)
    return 0;

  if (tree->type == VALUE)
    return tree->n.value;

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
static size_t writeValueToBuffer(double number, char *buf, size_t buf_sz)
{
  // Handling the value
  size_t length;
  
  if (fmod(number, 1.0) == 0.0)
  {
    length = snprintf(buf, buf_sz, "%.0f", number);
  }
  else
  {
    char temp[buf_sz];
    snprintf(temp, buf_sz, "%f", number);

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
      {
        count = strlen(temp) - (ptr - temp) - 1;
      }
    }

    if (count < 2)
    {
      length = snprintf(buf, buf_sz, "%.1f", number);
    }
    else
    {
      length = snprintf(buf, buf_sz, "%s", temp);
    }
  }

  return length;
}

size_t ET_tree2string(ExprTree tree, char *buf, size_t buf_sz)
{
  assert(buf != NULL);
  assert(buf_sz > 0);
  assert(tree);

  size_t length = 0;
  char leftBuffer[buf_sz / 2];
  char rightBuffer[buf_sz / 2];

  // Add a flag at the end of the string to indicate truncation
  buf[buf_sz - 1] = '$';

  if (tree->type == VALUE)
  {
    length = writeValueToBuffer(tree->n.value, buf, buf_sz);
  }
  else
  {
    size_t leftLength = ET_tree2string(tree->n.child[LEFT], leftBuffer, buf_sz / 2);

    if (tree->type == UNARY_NEGATE)
      length = snprintf(buf, buf_sz, "(-%s)", leftBuffer);
    else
    {
      size_t rightLength = ET_tree2string(tree->n.child[RIGHT], rightBuffer, buf_sz / 2);

      if (tree->n.child[LEFT]->type != VALUE)
      {
        char tempBuffer[buf_sz / 2];
        snprintf(tempBuffer, buf_sz / 2, "%s", leftBuffer);
        strcpy(leftBuffer, tempBuffer);
        leftLength += 2;
      }

      if (tree->n.child[RIGHT]->type != VALUE)
      {
        char tempBuffer[buf_sz / 2];
        snprintf(tempBuffer, buf_sz / 2, "%s", rightBuffer);
        strcpy(rightBuffer, tempBuffer);
        rightLength += 2;
      }

      length = snprintf(buf, buf_sz, "(%s %c %s)", leftBuffer, ExprNodeType_to_char(tree->type), rightBuffer);
    }
  }

  // Truncation
  if (length >= buf_sz)
  {
    length = buf_sz - 1;
    // Add a flag at the end of the string to indicate truncation
    buf[buf_sz - 2] = '$';
    buf[buf_sz - 1] = '\0';
  }

  return length;
}

// TRUNCATION