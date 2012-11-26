/*
 * File:   flicamtest.c
 * Author: cheoppy
 *
 * Created on November 26, 2012, 1:13 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "fli_common.h"
#include "libfli.h"
#include "flicamtest.h"

int main(int argc, char** argv) {
  int i, ret;

  /* scan for devices */
  fli_scan();

  /* open the filter */
  camera_init();

  /* process and execute commands */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--get-temperature") == 0) {
      ret = camera_get_temp();
      if (ret) return ret;
    } else if (strcmp(argv[i], "--fan") == 0) {
      if (i + 1 < argc && strcmp(argv[i + 1], "on") == 0) {
        fprintf(stdout, "turning on the fan for the first camera\n");
        ret = camera_set_fan(1);
        if (ret) return ret;
        i++;
      } else if (i + 1 < argc && strcmp(argv[i + 1], "off") == 0) {
        fprintf(stdout, "turning off the fan for the first camera\n");
        ret = camera_set_fan(0);
        if (ret) return ret;
        i++;
      }
    } else if (strcmp(argv[i], "--info") == 0) {
      ret = camera_info();
      if (ret) return ret;
    }
  }

  /* close the filter handle */
  camera_fini();
  return (EXIT_SUCCESS);
  return (EXIT_SUCCESS);
}

int camera_fini() {
  if (fli->active_camera != FLI_INVALID_DEVICE) {
    fli_unlock_and_close_device(&fli->active_camera);
  }
  return (0);
}

int camera_init() {
  if (fli->num_cameras == 1 && fli->active_camera == FLI_INVALID_DEVICE) {
    fli_open_and_lock_first_camera(&fli->active_camera);
  } else {
    if (fli->num_cameras == 0) {
      fprintf(stderr, "Could not find any cameras\n");
    } else {
      fprintf(stderr, "Found more than 1 cameras, but flicamtest can only handle 1 camera ATM\n");
    }
    return (EXIT_FAILURE);
  }
  return (0);
}

int camera_get_temp() {
  int err;
  double ccd_temp, base_temp;
  err = FLIReadTemperature(fli->active_camera, FLI_TEMPERATURE_CCD, &ccd_temp);
  if (fli_check_error(err, "FLIReadTemperature")) return -1;
  err = FLIReadTemperature(fli->active_camera, FLI_TEMPERATURE_BASE, &base_temp);
  if (fli_check_error(err, "FLIReadTemperature")) return -1;

  fprintf(stdout, "camera CCD  temperature: %f°C\n", ccd_temp);
  fprintf(stdout, "camera base temperature: %f°C\n", base_temp);
  return 0;
}

int camera_info() {
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

  fprintf(stdout, "filter FW revision: 0x%04x\n", (int) fwrev);
  fprintf(stdout, "filter HW revision: 0x%04x\n", (int) hwrev);
  fprintf(stdout, "filter serial number: %s\n", serial);
  fprintf(stdout, "filter model: %s\n", model);
  fprintf(stdout, "filter lib version: %s\n", lib_version);
  return (0);
}

/**
 * Turns on (1) or off (0) the fan on the given camera
 *
 * @param status
 * @return
 */
int camera_set_fan(int status) {
  int err;

  if (status == 1 || status == 0) {
    if (status == 1) {
      /* Turn on the fan */
      err = FLISetFanSpeed(fli->active_camera, FLI_FAN_SPEED_ON);
      if (fli_check_error(err, "FLISetFanSpeed")) return -1;
    } else if (status == 0) {
      /* Turn off the fan */
      err = FLISetFanSpeed(fli->active_camera, FLI_FAN_SPEED_OFF);
      if (fli_check_error(err, "FLISetFanSpeed")) return -1;
    }

    return 0;
  } else {
    return -1;
  }
}