/*
 * File:   fliflttest.c
 * Author: Gergely Csépány (gcsepany@flyseye.net)
 *
 * Created on November 26, 2012, 12:36 PM
 */

/* this is to be counted from 1 to 4, for human legibility */
#define NUMBER_OF_DIFFERENT_SYNTAXES 4

#include "fliflttest.h"

extern fli_status * fli;

/* global variables for argtable */
struct arg_lit *info1, *home3, *help4;
struct arg_int *setfilter2;
struct arg_end *end1, *end2, *end3, *end4;
void **argtable[NUMBER_OF_DIFFERENT_SYNTAXES + 1];
const char* progname = "fliflttest";

int main(int argc, char** argv) {
  int i, ret;
  int nerrors[NUMBER_OF_DIFFERENT_SYNTAXES + 1];

  /* argtable setup */
  /* SYNTAX 1: [--info] */
  info1 = arg_lit1(NULL, "info", "Show information about the FLI filterwheel");
  end1 = arg_end(20);
  void * argtable1[] = {info1, end1};
  argtable[1] = argtable1;

  /* SYNTAX 2: [--set-filter]  */
  setfilter2 = arg_int1(NULL, "set-filter", "N", "Set the filter N");
  end2 = arg_end(20);
  void * argtable2[] = {setfilter2, end2};
  argtable[2] = argtable2;

  /* SYNTAX 3: [--home]  */
  home3 = arg_lit1(NULL, "home", "Homes the filterwheel");
  end3 = arg_end(20);
  void * argtable3[] = {home3, end3};
  argtable[3] = argtable3;

  /* SYNTAX 4: [--help]*/
  help4 = arg_lit1(NULL, "help", "Print this help");
  end4 = arg_end(20);
  void * argtable4[] = {help4, end4};
  argtable[4] = argtable4;

  /* verify all argtable[] entries were allocated successfully */
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    if (arg_nullcheck(argtable[i]) != 0) {
      printf("%s: insufficient memory\n", progname);
      return EXIT_FAILURE;
    }
  }

  /* parse all argument possibilities */
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    nerrors[i] = arg_parse(argc, argv, argtable[i]);
  }

  /* select the right command */
  /* --info */
  if (nerrors[1] == 0) {
    if (info1->count > 0) {
      ret = filter_info();
      if (ret) return ret;
      return 0;
    }
    /* --set-filter */
  } else if (nerrors[2] == 0) {
    if (setfilter2->count > 0) {
      ret = filter_set_filter(setfilter2->ival[0]);
      if (ret) return ret;
      return 0;
    }
    /* --home */
  } else if (nerrors[3] == 0) {
    if (home3->count > 0) {
      ret = filter_home(1);
      if (ret) return ret;
      return 0;
    }
    /* --help */
  } else if (nerrors[4] == 0) {
    if (help4) {
      ret = argtable_help(progname, NUMBER_OF_DIFFERENT_SYNTAXES, argtable);
      if (ret) return ret;
      return 0;
    }
    /* incorrect or partially incorrect argument syntaxes */
  } else {
    if (setfilter2->count > 0) {
      arg_print_errors(stdout, end2, progname);
      printf("usage: %s ", progname);
      arg_print_syntax(stdout, argtable2, "\n");
    } else {
      printf("%s: unable to parse arguments, see syntax below:\n", progname);
      ret = argtable_help(progname, NUMBER_OF_DIFFERENT_SYNTAXES, argtable);
      if (ret) return ret;
      return 0;
    }
    return EXIT_FAILURE;
  }

  /* no command line options at all */
  printf("Try '%s --help' for more information.\n", progname);
  return (EXIT_SUCCESS);
}

/**
 * Homes the filterwheel.
 *
 * @param standalone, 0 or 1, indicates if this function is called on its own
 * @return Zero on success, non-zero on failure.
 */
int filter_home(int standalone) {
  long status;
  int err;

  if (standalone) {
    err = filter_init();
    if (err) return err;
  }

  /* Home the filter by setting the position to -1 */
  CALLFLIAPI(FLISetFilterPos(fli->active_filter, -1), "FLISetFilterPos(-1)");

  /* check if it has really arrived at home */
  CALLFLIAPI(FLIGetDeviceStatus(fli->active_filter, &status), "FLIGetDeviceStatus");

  /* the 0x80 status means that it's at home */
  if (status == 0x80) {
    fli->current_filter = 1;
  } else {
    fprintf(stderr, "filter has failed to home\n");
    return -1;
  }

  if (standalone) {
    err = filter_fini();
    if (err) return err;
  }

  return (0);
}

/**
 * Set the filter #position of the filterwheel
 *
 * @param position, integer, number of the filter to be set
 * @return Zero on success, non-zero on failure.
 */
int filter_set_filter(int position) {
  int err, ret;
  long status, filter_count;

  err = filter_init();
  if (err) return err;

  CALLFLIAPI(FLIGetFilterCount(fli->active_filter, &filter_count), "FLIGetFilterCount");

  if (position > filter_count || position < 1) {
    fprintf(stderr, "invalid filter position: %d\n", position);
    return -1;
  }

  /* the filter has not been initialized yet, so we start with a homing */
  if (fli->current_filter == -1) {
    ret = filter_home(0);
    if (ret) return ret;
  }

  /* Set the filter */
  CALLFLIAPI(FLISetFilterPos(fli->active_filter, (long) (position - 1)), "FLISetFilterPos(position-1)");

  /* check if it has really arrived at the designated position */
  CALLFLIAPI(FLIGetDeviceStatus(fli->active_filter, &status), "FLIGetDeviceStatus");

  /* 0 means it's in position, 0x80 means it's at home */
  if (status == 0 || (status == 0x80 && position == 1)) {
  } else {
    fprintf(stderr, "filter has failed to go to position\n");
    return -1;
  }

  fli->current_filter = position;

  err = filter_fini();
  if (err) return err;

  return (0);
}

/**
 * Unlocks and closes the filterwheel handle; and frees all the argtable arrays.
 *
 * @return Zero on success, non-zero on failure.
 */
int filter_fini() {
  int ret, i;
  if (fli->active_filter != FLI_INVALID_DEVICE) {
    ret = fli_unlock_and_close_device(&fli->active_filter);
    if (ret) return ret;
  }
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    arg_freetable(argtable[i], sizeof (argtable[i]) / sizeof (argtable[i][0]));
  }
  return (0);
}

/**
 * Scans for FLI filterwheels, and if exactly one is found, opens and lock it.
 *
 * @return Zero on success, non-zero on failure.
 */
int filter_init() {
  int ret;

  /* already initialized, do not need to do anything */
  if (fli->num_filters == 1 && fli->active_filter != FLI_INVALID_DEVICE) {
    return 0;
  }

  /* no active filter, scan for them */
  if (fli->num_filters == 0) {
    ret = fli_scan();
    if (ret) return ret;
  }

  if (fli->num_filters == 1 && fli->active_filter == FLI_INVALID_DEVICE) {
    ret = fli_open_and_lock_first_filter(&fli->active_filter);
    if (ret) return ret;
  } else {
    if (fli->num_filters == 0) {
      fprintf(stderr, "Could not find any filterwheels\n");
    } else {
      fprintf(stderr, "Found %d filterwheels, but fliflttest can only handle 1 filterwheel ATM\n", fli->num_filters);
    }
    return -1;
  }
  return (0);
}

/**
 * Prints information about the filterwheel.
 *
 * @return Zero on success, non-zero on failure.
 */
int filter_info() {
  long fwrev, hwrev, filter_count;
  char * serial;
  char * model;
  char * lib_version;
  int err;

  err = filter_init();
  if (err) return err;

  lib_version = (char*) malloc((128) * sizeof (char));
  model = (char*) malloc((128) * sizeof (char));
  serial = (char*) malloc((128) * sizeof (char));

  CALLFLIAPI(FLIGetFWRevision(fli->active_filter, &fwrev), "FLIGetFWRevision");
  CALLFLIAPI(FLIGetHWRevision(fli->active_filter, &hwrev), "FLIGetHWRevision");
  CALLFLIAPI(FLIGetSerialString(fli->active_filter, serial, 128), "FLIGetSerialString");
  CALLFLIAPI(FLIGetModel(fli->active_filter, model, 128), "FLIGetModel");
  CALLFLIAPI(FLIGetLibVersion(lib_version, 128), "FLIGetLibVersion");
  CALLFLIAPI(FLIGetFilterCount(fli->active_filter, &filter_count), "FLIGetFilterCount");

  fprintf(stdout, "filterwheel FW revision: 0x%04lx\n", fwrev);
  fprintf(stdout, "filterwheel HW revision: 0x%04lx\n", hwrev);
  fprintf(stdout, "filterwheel serial number: %s\n", serial);
  fprintf(stdout, "filterwheel model: %s\n", model);
  fprintf(stdout, "FLI lib version: %s\n", lib_version);
  fprintf(stdout, "filterwheel, number of filters: %ld\n", filter_count);

  err = filter_fini();
  if (err) return err;

  return (0);
}

