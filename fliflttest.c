/*
 * File:   fliflttest.c
 * Author: cheoppy
 *
 * Created on November 26, 2012, 12:36 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fli_common.h"
#include "libfli.h"
#include "fliflttest.h"

int main(int argc, char** argv) {
  int i, ret, position;

  /* scan for devices */
  fli_scan();

  /* open the filter */
  filter_init();

  /* process and execute commands */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--home") == 0) {
      ret = filter_home();
      if (ret) return ret;
    } else if (strcmp(argv[i], "--set-filter") == 0) {
      if (argc < i + 1) {
        fprintf(stderr, "--set-filter needs a position\n");
        return EXIT_FAILURE;
      }
      position = atoi(argv[i + 1]);
      ret = filter_set_filter(position);
      if (ret) return ret;
    } else if (strcmp(argv[i], "--info") == 0) {
      ret = filter_info();
      if (ret) return ret;
    } else if (strcmp(argv[i], "help") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      fprintf(stdout, "fliflttest usage:\n"
              "\t[--help] [--home] [--set-filter [1-5]] [--info]\n");
    }
  }

  /* close the filter handle */
  filter_fini();
  return (EXIT_SUCCESS);
}

int filter_home() {
  long status;
  int err;

  fprintf(stdout, "filter is going home\n");

  /* Home the filter by setting the position to -1 */
  err = FLISetFilterPos(fli->active_filter, -1);
  if (fli_check_error(err, "FLISetFilterPos(-1)")) return -1;

  /* check if it has really arrived at home */
  err = FLIGetDeviceStatus(fli->active_filter, &status);
  if (fli_check_error(err, "FLIGetDeviceStatus")) return -1;

  /* the 0x80 status means that it's at home */
  if (status == 0x80) {
    fprintf(stdout, "filter is at home\n");
    fli->current_filter = 1;
  } else {
    fprintf(stdout, "filter has failed to home\n");
    return -1;
  }
  return (0);
}

int filter_set_filter(int position) {
  int err;
  long status;

  if (position > filter_get_nfilter() || position < 1) {
    fprintf(stderr, "invalid filter position: %d\n", position);
    return -1;
  }

  /* the filter has not been initialized yet, so we start with a homing */
  if (fli->current_filter == -1) {
    filter_home();
  }

  fprintf(stdout, "filter is going to position %d\n", position);

  /* Set the filter */
  err = FLISetFilterPos(fli->active_filter, (long) (position - 1));
  if (fli_check_error(err, "FLISetFilterPos(position-1)")) return -1;

  /* check if it has really arrived at the designated position */
  err = FLIGetDeviceStatus(fli->active_filter, &status);
  if (fli_check_error(err, "FLIGetDeviceStatus")) return -1;

  /* 0 means it's in position, 0x80 means it's at home */
  if (status == 0 || (status == 0x80 && position == 1)) {
    fprintf(stdout, "filter is at position\n");
  } else {
    fprintf(stdout, "filter has failed to go to position\n");
    return -1;
  }

  fli->current_filter = position;

  return (0);
}

int filter_fini() {
  if (fli->active_filter != FLI_INVALID_DEVICE) {
    fli_unlock_and_close_device(&fli->active_filter);
  }
  return (0);
}

int filter_init() {
  if (fli->num_filters == 1 && fli->active_filter == FLI_INVALID_DEVICE) {
    fli_open_and_lock_first_filter(&fli->active_filter);
  } else {
    if (fli->num_filters == 0) {
      fprintf(stderr, "Could not find any filterwheels\n");
    } else {
      fprintf(stderr, "Found more than 1 filterwheels, but fliflttest can only handle 1 filterwheel ATM\n");
    }
    return (EXIT_FAILURE);
  }
  return (0);
}

int filter_get_nfilter() {
  int err;
  long filter_count = 0;

  /* Get the number of filters */
  err = FLIGetFilterCount(fli->active_filter, &filter_count);
  if (fli_check_error(err, "FLIGetFilterCount")) return -1;

  fprintf(stdout, "filter has %ld positions\n", filter_count);

  return filter_count;
}

int filter_info() {
  long fwrev, hwrev;
  char * serial;
  char * model;
  char * lib_version;
  int err;

  lib_version = (char*) malloc((128) * sizeof (char));
  model = (char*) malloc((128) * sizeof (char));
  serial = (char*) malloc((128) * sizeof (char));

  err = FLIGetFWRevision(fli->active_filter, &fwrev);
  if (fli_check_error(err, "FLIGetFWRevision")) return -1;
  err = FLIGetHWRevision(fli->active_filter, &hwrev);
  if (fli_check_error(err, "FLIGetHWRevision")) return -1;
  err = FLIGetSerialString(fli->active_filter, serial, 128);
  if (fli_check_error(err, "FLIGetSerialString")) return -1;
  err = FLIGetModel(fli->active_filter, model, 128);
  if (fli_check_error(err, "FLIGetModel")) return -1;
  err = FLIGetLibVersion(lib_version, 128);
  if (fli_check_error(err, "FLIGetLibVersion")) return -1;

  fprintf(stdout, "filter FW revision: 0x%04lx\n", fwrev);
  fprintf(stdout, "filter HW revision: 0x%04lx\n", hwrev);
  fprintf(stdout, "filter serial number: %s\n", serial);
  fprintf(stdout, "filter model: %s\n", model);
  fprintf(stdout, "filter lib version: %s\n", lib_version);
  return (0);
}