/*
 * File:   flicamtest.c
 * Author: cheoppy
 *
 * Created on November 26, 2012, 1:13 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "flicamtest.h"
#include <argtable2.h>

extern fli_status * fli;


int main(int argc, char** argv) {
  int i, ret;
  int set_temp;
  double exposure_time, gain = 1.0;
  int is_dark = 0, is_verbose = 0;
  char * output_filename;
  char * binning_options;
  char *offset_options;
  char *size_options;
  char *download_speed;

  output_filename = "";
  binning_options = "";
  offset_options = "";
  size_options = "";
  download_speed = "";

  /* scan for devices */
  fli_scan();

  /* open the filter */
  camera_init();

  /* process and execute commands */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--get-temperature") == 0) {
      ret = camera_get_temp();
      if (ret) return ret;
    } else if (strcmp(argv[i], "--set-temperature") == 0) {
      if (argc < i + 1) {
        fprintf(stderr, "The set temperature must be provided.\n");
        return EXIT_FAILURE;
      }

      set_temp = atoi(argv[i + 1]);
      i++;

      ret = camera_set_temp(set_temp);
      if (ret) return ret;
    } else if (strcmp(argv[i], "--shutter") == 0) {
      if (i + 1 < argc && strcmp(argv[i + 1], "open") == 0) {
        ret = camera_control_shutter(1);
        if (ret) return ret;
        i++;
      } else if (i + 1 < argc && strcmp(argv[i + 1], "close") == 0) {
        ret = camera_control_shutter(0);
        if (ret) return ret;
        i++;
      }
    } else if (strcmp(argv[i], "--fan") == 0) {
      if (i + 1 < argc && strcmp(argv[i + 1], "on") == 0) {
        ret = camera_set_fan(1);
        if (ret) return ret;
        i++;
      } else if (i + 1 < argc && strcmp(argv[i + 1], "off") == 0) {
        ret = camera_set_fan(0);
        if (ret) return ret;
        i++;
      }
    } else if (strcmp(argv[i], "--acquire") == 0) {
      if (argc < i + 6) {
        fprintf(stderr, "acquire requires '--acquire <time in sec> --shutter (open|close) --output <fits filename>'\n");
        return EXIT_FAILURE;
      }
      exposure_time = atof(argv[i + 1]);
      i++;
      i++;
      if (i < argc && strcmp(argv[i], "--shutter") == 0) {
        if (i + 1 < argc && strcmp(argv[i + 1], "open") == 0) {
          is_dark = 0;
          i++;
        } else if (i + 1 < argc && strcmp(argv[i + 1], "close") == 0) {
          is_dark = 1;
          i++;
        }
        i++;
      }
      if (i + 1 < argc && strcmp(argv[i], "--output") == 0) {
        output_filename = argv[i + 1];
        i++;
        i++;
      }
      if (i + 1 < argc && strcmp(argv[i], "--bin") == 0) {
        binning_options = argv[i + 1];
        i++;
        i++;
      }
      if (i + 1 < argc && strcmp(argv[i], "--offset") == 0) {
        offset_options = argv[i + 1];
        i++;
        i++;
      }
      if (i + 1 < argc && strcmp(argv[i], "--size") == 0) {
        size_options = argv[i + 1];
        i++;
        i++;
      }
      if (i + 1 < argc && strcmp(argv[i], "--gain") == 0) {
        gain = atof(argv[i + 1]);
        i++;
        i++;
      }
      if (i + 1 < argc && strcmp(argv[i], "--mode") == 0) {
        download_speed = argv[i + 1];
        i++;
        i++;
      }
      if (i < argc && strcmp(argv[i], "--verbose") == 0) {
        is_verbose = 1;
        i++;
      }
      fprintf(stdout, "acquire options:\n\texposure_time: %f\n\tis_dark: %d"
              "\n\toutput: %s\n\tbinning_options: %s"
              "\n\toffset_options: %s\n\tsize_options: %s\n\tgain: %f"
              "\n\tdownload_speed: %s\n\tis_verbose: %d\n", exposure_time,
              is_dark, output_filename, binning_options, offset_options, size_options, gain,
              download_speed, is_verbose);
    } else if (strcmp(argv[i], "--info") == 0) {
      ret = camera_info();
      if (ret) return ret;
    }
  }

  /* close the filter handle */
  camera_fini();
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
  double ccd_temp, base_temp, cooler_power;
  err = FLIReadTemperature(fli->active_camera, FLI_TEMPERATURE_CCD, &ccd_temp);
  if (fli_check_error(err, "FLIReadTemperature")) return -1;
  err = FLIReadTemperature(fli->active_camera, FLI_TEMPERATURE_BASE, &base_temp);
  if (fli_check_error(err, "FLIReadTemperature")) return -1;
  err = FLIGetCoolerPower(fli->active_camera, &cooler_power);
  if (fli_check_error(err, "FLIGetCoolerPower")) return -1;

  fprintf(stdout, "camera CCD  temperature: %f°C\n", ccd_temp);
  fprintf(stdout, "camera base temperature: %f°C\n", base_temp);
  fprintf(stdout, "camera cooler power: %f%%\n", cooler_power);
  return 0;
}

int camera_set_temp(int set_temp) {
  int err;
  err = FLISetTemperature(fli->active_camera, set_temp);
  if (fli_check_error(err, "FLISetTemperature")) return -1;

  return 0;
}

int camera_control_shutter(int status) {
  int err;
  if (status == 1 || status == 0) {
    if (status == 1) {
      /* Open the shutter */
      err = FLIControlShutter(fli->active_camera, FLI_SHUTTER_OPEN);
      if (fli_check_error(err, "FLIControlShutter")) return -1;
    } else if (status == 0) {
      /* Close the shutter */
      err = FLIControlShutter(fli->active_camera, FLI_SHUTTER_CLOSE);
      if (fli_check_error(err, "FLIControlShutter")) return -1;
    }

    return 0;
  } else {
    return -1;
  }

  return 0;
}

int camera_info() {
  long fwrev, hwrev;
  char * serial;
  char * model;
  char * lib_version;
  int err;
  long total_ul_x, total_ul_y, total_lr_x, total_lr_y;
  long visible_ul_x, visible_ul_y, visible_lr_x, visible_lr_y;
  double pixel_size_x, pixel_size_y;

  lib_version = (char*) malloc((128) * sizeof (char));
  model = (char*) malloc((128) * sizeof (char));
  serial = (char*) malloc((128) * sizeof (char));

  err = FLIGetFWRevision(fli->active_camera, &fwrev);
  if (fli_check_error(err, "FLIGetFWRevision")) return -1;
  err = FLIGetHWRevision(fli->active_camera, &hwrev);
  if (fli_check_error(err, "FLIGetHWRevision")) return -1;
  err = FLIGetSerialString(fli->active_camera, serial, 128);
  if (fli_check_error(err, "FLIGetSerialString")) return -1;
  err = FLIGetModel(fli->active_camera, model, 128);
  if (fli_check_error(err, "FLIGetModel")) return -1;
  err = FLIGetLibVersion(lib_version, 128);
  if (fli_check_error(err, "FLIGetLibVersion")) return -1;
  err = FLIGetArrayArea(fli->active_camera, &total_ul_x, &total_ul_y, &total_lr_x, &total_lr_y);
  if (fli_check_error(err, "FLIGetLibVersion")) return -1;
  err = FLIGetVisibleArea(fli->active_camera, &visible_ul_x, &visible_ul_y, &visible_lr_x, &visible_lr_y);
  if (fli_check_error(err, "FLIGetVisibleArea")) return -1;
  err = FLIGetPixelSize(fli->active_camera, &pixel_size_x, &pixel_size_y);
  if (fli_check_error(err, "FLIGetPixelSize")) return -1;

  fprintf(stdout, "camera FW revision: 0x%04x\n", (int) fwrev);
  fprintf(stdout, "camera HW revision: 0x%04x\n", (int) hwrev);
  fprintf(stdout, "camera serial number: %s\n", serial);
  fprintf(stdout, "camera model: %s\n", model);
  fprintf(stdout, "camera lib version: %s\n", lib_version);
  fprintf(stdout, "camera total area: %ld, %ld, %ld, %ld\n", total_ul_x, total_ul_y, total_lr_x, total_lr_y);
  fprintf(stdout, "camera visilbe area: %ld, %ld, %ld, %ld\n", visible_ul_x, visible_ul_y, visible_lr_x, visible_lr_y);
  fprintf(stdout, "camera pixel sized: %f, %f\n", pixel_size_x, pixel_size_y);
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