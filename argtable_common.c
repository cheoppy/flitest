/*
 * File:   argtable_common.c
 * Author: Gergely Csépány (gcsepany@flyseye.net)
 *
 * Created on November 26, 2012, 12:55 PM
 */

#include "argtable_common.h"

/**
 * Prints the help based on argtable.
 *
 * @return  Zero on success, non-zero on failure.
 */
int argtable_help(const char * progname, int number_of_syntaxes, void *** argtable) {
  int i;
  for (i = 1; i <= number_of_syntaxes; i++) {
    if (i == 1) {
      printf("Usage: %s", progname);
    } else {
      printf("       %s", progname);
    }
    arg_print_syntax(stdout, argtable[i], "\n");
  }
  printf("Explanation of the options:\n");
  for (i = 1; i <= number_of_syntaxes; i++) {
    arg_print_glossary_gnu(stdout, argtable[i]);
  }
  return 0;
}