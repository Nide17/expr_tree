/*
 * et_test.c
 *
 * Test cases for ExprTree
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> // strlen
#include <ctype.h>  // isblank
#include <math.h>   // fabs
#include <stdbool.h>

#include "expr_tree.h"

// Checks that value is true; if not, prints a failure message and
// returns 0 from this function
#define test_assert(value)                                         \
  {                                                                \
    if (!(value))                                                  \
    {                                                              \
      printf("FAIL %s[%d]: %s\n", __FUNCTION__, __LINE__, #value); \
      return 0;                                                    \
    }                                                              \
  }

/*
 * Tests the ET_value, ET_node, and ET_free functions.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_node_free()
{
  ExprTree tree = NULL;

  test_assert(ET_depth(tree) == 0);

  tree = ET_value(0);
  test_assert(ET_depth(tree) == 1);
  ET_free(tree);

  tree = ET_value(23400000);
  test_assert(ET_depth(tree) == 1);
  ET_free(tree);

  tree = ET_value(-1000);
  test_assert(ET_depth(tree) == 1);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_value(3));
  test_assert(ET_depth(tree) == 2);
  ET_free(tree);

  tree = ET_node(OP_MUL, ET_node(OP_ADD, ET_value(1), ET_value(2)), ET_node(OP_SUB, ET_value(4), ET_value(3)));
  test_assert(ET_depth(tree) == 3);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_value(4)));
  test_assert(ET_depth(tree) == 3);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_value(5))));
  test_assert(ET_depth(tree) == 4);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_node(OP_ADD, ET_value(5), ET_value(6)))));
  test_assert(ET_depth(tree) == 5);
  ET_free(tree);

  return 1;
}

/*
 * Tests the ET_depth function.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_depth()
{
  ExprTree tree = NULL;
  int depth;

  tree = ET_value(0);
  test_assert(ET_depth(tree) == 1);
  ET_free(tree);

  tree = ET_value(23400000);
  test_assert(ET_depth(tree) == 1);
  ET_free(tree);

  tree = ET_value(-1000);
  test_assert(ET_depth(tree) == 1);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_value(3));
  test_assert(ET_depth(tree) == 2);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_value(4)));
  depth = ET_depth(tree);
  ET_free(tree);
  test_assert(depth == 3);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_value(5))));
  depth = ET_depth(tree);
  ET_free(tree);
  test_assert(depth == 4);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_node(OP_ADD, ET_value(5), ET_value(6)))));
  depth = ET_depth(tree);
  ET_free(tree);
  test_assert(depth == 5);

  // 2^(1.5 × 2) ÷ (−1.7 + (6 − 0.3))
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_value(2), ET_node(OP_MUL, ET_value(1.5), ET_value(2))), ET_node(OP_ADD, ET_value(-1.7), ET_node(OP_SUB, ET_value(6), ET_value(0.3))));
  depth = ET_depth(tree);
  ET_free(tree);
  test_assert(depth == 4);

  return 1;
}

/*
 * Tests the ET_count function.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_count()
{
  ExprTree tree = NULL;
  int count = 0;

  tree = ET_value(23400000);
  count = ET_count(tree);
  ET_free(tree);
  test_assert(count == 1);

  tree = ET_value(-1000);
  count = ET_count(tree);
  ET_free(tree);
  test_assert(count == 1);

  tree = ET_node(OP_ADD, ET_value(1), ET_value(3));
  count = ET_count(tree);
  ET_free(tree);
  test_assert(count == 3);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_value(4)));
  count = ET_count(tree);
  ET_free(tree);
  test_assert(count == 5);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_value(5))));
  count = ET_count(tree);
  ET_free(tree);
  test_assert(count == 7);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_node(OP_ADD, ET_value(5), ET_value(6)))));
  count = ET_count(tree);
  ET_free(tree);
  test_assert(count == 9);

  // 2^(1.5 × 2) ÷ (−1.7 + (6 − 0.3))
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_value(2), ET_node(OP_MUL, ET_value(1.5), ET_value(2))), ET_node(OP_ADD, ET_value(-1.7), ET_node(OP_SUB, ET_value(6), ET_value(0.3))));
  count = ET_count(tree);
  ET_free(tree);
  test_assert(count == 11);

  return 1;
}

/*
 * Tests the ET_evaluate function.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_evaluate()
{
  ExprTree tree = NULL;
  int value = 0;

  tree = ET_value(-1000);
  value = ET_evaluate(tree);
  ET_free(tree);
  test_assert(value == -1000);

  tree = ET_node(OP_ADD, ET_value(1), ET_value(3));
  value = ET_evaluate(tree);
  ET_free(tree);
  test_assert(value == 4);

  tree = ET_node(OP_MUL, ET_value(5), ET_node(OP_SUB, ET_value(10), ET_value(3)));
  test_assert(ET_count(tree) == 5);
  test_assert(fabs(ET_evaluate(tree) - 35) < 1e-6);
  ET_free(tree);

  tree = ET_value(23400000);
  test_assert(ET_evaluate(tree) == 23400000);
  ET_free(tree);

  tree = ET_value(-1000);
  test_assert(ET_evaluate(tree) == -1000);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_value(3));
  test_assert(ET_evaluate(tree) == 4);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_value(4)));
  test_assert(ET_evaluate(tree) == 8);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_value(5))));
  test_assert(ET_evaluate(tree) == 13);
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_node(OP_ADD, ET_value(3), ET_node(OP_ADD, ET_value(4), ET_node(OP_ADD, ET_value(5), ET_value(6)))));
  test_assert(ET_evaluate(tree) == 19);
  ET_free(tree);

  // (6.5 * (4 + 3)) ==> 45.5
  tree = ET_node(OP_MUL, ET_value(6.5), ET_node(OP_ADD, ET_value(4), ET_value(3)));
  test_assert(ET_evaluate(tree) == 45.5);
  ET_free(tree);

  // -0.125 ==> -0.125
  tree = ET_value(-0.125);
  test_assert(ET_evaluate(tree) == -0.125);
  ET_free(tree);

  // (--0.125) ==> 0.125
  tree = ET_node(UNARY_NEGATE, ET_value(-0.125), NULL);
  test_assert(ET_evaluate(tree) == 0.125);
  ET_free(tree);

  // (-(--0.125)) ==> -0.125
  tree = ET_node(UNARY_NEGATE, ET_node(UNARY_NEGATE, ET_value(-0.125), NULL), NULL);
  test_assert(ET_evaluate(tree) == -0.125);
  ET_free(tree);

  // ((2 ^ 3) / (1.3 + 2.7)) ==> 2
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_value(2), ET_value(3)), ET_node(OP_ADD, ET_value(1.3), ET_value(2.7)));
  test_assert(ET_evaluate(tree) == 2);
  ET_free(tree);

  // ((20 - 10) - (30 - 25)) ==> 5
  tree = ET_node(OP_SUB, ET_node(OP_SUB, ET_value(20), ET_value(10)), ET_node(OP_SUB, ET_value(30), ET_value(25)));
  test_assert(ET_evaluate(tree) == 5);
  ET_free(tree);

  // 2^(1.5 × 2) ÷ (−1.7 + (6 − 0.3)) ==> 2
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_value(2), ET_node(OP_MUL, ET_value(1.5), ET_value(2))), ET_node(OP_ADD, ET_value(-1.7), ET_node(OP_SUB, ET_value(6), ET_value(0.3))));
  test_assert(ET_evaluate(tree) == 2);
  ET_free(tree);

  // 2^3 / (1.3 + 2.7) ==> 2
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_value(2), ET_value(3)), ET_node(OP_ADD, ET_value(1.3), ET_value(2.7)));
  value = ET_evaluate(tree);
  ET_free(tree);
  test_assert(fabs(value - 2) < 1e-6);
  test_assert(value == 2);

  return 1;
}

/*
 * Helper function for test_tree2string that tests a single tree.
 * 
 * Parameters:
 *  tree: the tree to test
 *  expected_str: the expected string representation of the tree
 * 
 * Returns: 
 *  true if the test passes, false otherwise 
*/
bool test_tree2string_once(ExprTree tree, const char *expected_str)
{
  char buf[39];
  double result;

  size_t length = ET_tree2string(tree, buf, sizeof(buf));
  result = ET_evaluate(tree);
  printf("%s ==> %g: length = %ld\n", buf, result, length);
  return strcmp(buf, expected_str) == 0;
}

/*
 * Tests the ET_tree2string function.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */

int test_tree2string()
{
  ExprTree tree = NULL;

  // only one value of 1 char
  tree = ET_value(1000000000000000000);
  test_assert(test_tree2string_once(tree, "1000000000000000000"));
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_value(3));
  test_assert(test_tree2string_once(tree, "(1 + 3)"));
  ET_free(tree);

  tree = ET_node(OP_MUL, ET_value(5), ET_node(OP_SUB, ET_value(10), ET_value(3)));
  test_assert(test_tree2string_once(tree, "(5 * (10 - 3))"));
  ET_free(tree);

  tree = ET_node(OP_POWER, ET_value(2), ET_value(3));
  test_assert(test_tree2string_once(tree, "(2 ^ 3)"));
  ET_free(tree);

  tree = ET_node(OP_DIV, ET_value(3), ET_value(0));
  test_assert(test_tree2string_once(tree, "(3 / 0)"));
  ET_free(tree);

  // -0.5 (as a value)
  double value1 = -0.5;
  tree = ET_value(value1);
  test_assert(test_tree2string_once(tree, "-0.5"));
  ET_free(tree);

  // (6.5 * (4 + 3))
  tree = ET_node(OP_ADD, ET_value(4), ET_value(3));
  tree = ET_node(OP_MUL, ET_value(6.5), tree);
  test_assert(test_tree2string_once(tree, "(6.5 * (4 + 3))"));
  ET_free(tree);

  // -0.125 (as a value)
  double value = -0.125;
  tree = ET_value(value);
  test_assert(test_tree2string_once(tree, "-0.125"));

  // -(-0.125) (using value and unary negation) -> (--0.125) ==> 0.125
  tree = ET_node(UNARY_NEGATE, tree, NULL);
  test_assert(test_tree2string_once(tree, "(--0.125)"));

  // double unary negation: (- (- (-0.125))) -> (-(--0.125)) ==> -0.125
  tree = ET_node(UNARY_NEGATE, tree, NULL);
  test_assert(test_tree2string_once(tree, "(-(--0.125))"));
  ET_free(tree);

  // ((20 - 10) - (30 - 25))
  tree = ET_node(OP_SUB, ET_value(20), ET_value(10));
  tree = ET_node(OP_SUB, tree, ET_node(OP_SUB, ET_value(30), ET_value(25)));
  test_assert(test_tree2string_once(tree, "((20 - 10) - (30 - 25))"));
  ET_free(tree);

  // ((1 + 2) * (3 - 4))
  tree = ET_node(OP_SUB, ET_value(3), ET_value(4));
  tree = ET_node(OP_MUL, ET_node(OP_ADD, ET_value(1), ET_value(2)), tree);
  test_assert(test_tree2string_once(tree, "((1 + 2) * (3 - 4))"));
  ET_free(tree);

  // (( 2 ^ 3) / (1.3 + 2.7))
  tree = ET_node(OP_ADD, ET_value(1.3), ET_value(2.7));
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_value(2), ET_value(3)), tree);
  test_assert(test_tree2string_once(tree, "((2 ^ 3) / (1.3 + 2.7))"));
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(-2), ET_value(3));
  tree = ET_node(OP_MUL, tree, ET_node(OP_DIV, ET_node(OP_SUB, ET_value(-4), ET_value(1)), ET_value(2)));
  test_assert(test_tree2string_once(tree, "((-2 + 3) * ((-4 - 1) / 2))"));
  ET_free(tree);

  tree = ET_node(OP_SUB, ET_value(4), ET_value(1));
  tree = ET_node(OP_MUL, tree, ET_value(2));
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_node(OP_ADD, ET_value(2), ET_value(3)), ET_value(2)), tree);
  test_assert(test_tree2string_once(tree, "(((2 + 3) ^ 2) / ((4 - 1) * 2))"));
  ET_free(tree);

  tree = ET_node(OP_SUB, ET_value(2), ET_value(1));
  tree = ET_node(OP_MUL, tree, ET_value(4));
  tree = ET_node(OP_DIV, ET_node(OP_MUL, ET_value(5), ET_node(OP_ADD, ET_value(3), ET_value(7))), tree);
  test_assert(test_tree2string_once(tree, "((5 * (3 + 7)) / ((2 - 1) * 4))"));
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(-4), ET_value(1));
  tree = ET_node(OP_DIV, tree, ET_value(2));
  tree = ET_node(OP_MUL, ET_node(OP_ADD, ET_value(2), ET_node(OP_POWER, ET_value(-3), ET_value(2))), tree);
  test_assert(test_tree2string_once(tree, "((2 + (-3 ^ 2)) * ((-4 + 1) / 2))"));
  ET_free(tree);

  // ((2 ^ (1.5 * 2)) / (-1.7 + (6 - 0.3)))
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_value(2), ET_node(OP_MUL, ET_value(1.5), ET_value(2))), ET_node(OP_ADD, ET_value(-1.7), ET_node(OP_SUB, ET_value(6), ET_value(0.3))));
  test_assert(test_tree2string_once(tree, "((2 ^ (1.5 * 2)) / (-1.7 + (6 - 0.3))$"));
  ET_free(tree);

  // (((2 + 1) ^ (1.5 * 2)) / (-1.7 + (6 - 0.3))
  tree = ET_node(OP_DIV, ET_node(OP_POWER, ET_node(OP_ADD, ET_value(2), ET_value(1)), ET_node(OP_MUL, ET_value(1.5), ET_value(2))), ET_node(OP_ADD, ET_value(-1.7), ET_node(OP_SUB, ET_value(6), ET_value(0.3))));
  test_assert(test_tree2string_once(tree, "(((2 + 1) ^ (1.5 * 2)) / (-1.7 + (6 -$"));
  ET_free(tree);

  return 1;
}

int main()
{
  int passed = 0;
  int num_tests = 0;

  num_tests++;
  passed += test_node_free();

  num_tests++;
  passed += test_depth();

  num_tests++;
  passed += test_count();

  num_tests++;
  passed += test_evaluate();

  num_tests++;
  passed += test_tree2string();

  printf("Passed %d/%d test cases\n", passed, num_tests);
  fflush(stdout);
  return 0;
}
