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

#define NUMBER_OF_DIFFERENT_SYNTAXES 6

extern fli_status * fli;

struct arg_lit *info1, *gettemp2, *verbose5, *help6;
struct arg_str *fan4, *acquire5, *shutter5, *bin5, *size5, *offset5, *gain5, *mode5;
struct arg_int *settemp3;
struct arg_file *output5;
struct arg_end *end1, *end2, *end3, *end4, *end5, *end6;

void **argtable[NUMBER_OF_DIFFERENT_SYNTAXES];

const char* progname = "flicamtest";

int main(int argc, char** argv) {
  int ret, i;
  int nerrors[NUMBER_OF_DIFFERENT_SYNTAXES];

  /* SYNTAX 1: [--info] */
  info1 = arg_lit1(NULL, "info", "Show information about the FLI camera");
  end1 = arg_end(20);
  void * argtable1[] = {info1, end1};
  argtable[1] = argtable1;

  /* SYNTAX 2: [--get-temperature]  */
  gettemp2 = arg_lit1(NULL, "get-temperature", "Print the temperature of the CCD, base and cooling power");
  end2 = arg_end(20);
  void * argtable2[] = {gettemp2, end2};
  argtable[2] = argtable2;

  /* SYNTAX 3: [--set-temperature]  */
  settemp3 = arg_int1(NULL, "set-temperature", "N", "Set the temperature of the CCD (in Celsius)");
  end3 = arg_end(20);
  void * argtable3[] = {settemp3, end3};
  argtable[3] = argtable3;

  /* SYNTAX 4: [--fan]  */
  fan4 = arg_str1(NULL, "fan", "(on|off)", "Turn on or off the fan");
  end4 = arg_end(20);
  void * argtable4[] = {fan4, end4};
  argtable[4] = argtable4;

  /* SYNTAX 5: --acquire <time/sec> --shutter {open|close} --output xyz.fits
             [--bin <bx>,<by>] [--offset <x0>,<y0>] [--size <sx>,<sy>]
             [--gain <gain> --mode <mode>...] [--verbose]
   */
  acquire5 = arg_str1(NULL, "acquire", "<time>", "Exposure time in sec");
  shutter5 = arg_str1(NULL, "shutter", "(open|close)", "Whether to open or close the shutter");
  output5 = arg_file1(NULL, "output", "xyz.fits", "Output filename for the FITS file");
  bin5 = arg_str0(NULL, "bin", "<bx,by>", "Binning options in X,Y format");
  offset5 = arg_str0(NULL, "offset", "<x0,y0>", "Offset options in X,Y format");
  size5 = arg_str0(NULL, "size", "<sx,sy>", "Sizes in X,Y format");
  gain5 = arg_str0(NULL, "gain", "<gain>", "Gain factor");
  mode5 = arg_str0(NULL, "mode", "(1mhz|8mhz)", "Download speed");
  verbose5 = arg_lit0(NULL, "verbose", "Show verbose output");
  end5 = arg_end(20);
  void * argtable5[] = {acquire5, shutter5, output5, bin5, offset5, size5, gain5, mode5, verbose5, end5};
  argtable[5] = argtable5;

  /* SYNTAX 6: [--help]*/
  help6 = arg_lit1(NULL, "help", "Print this help");
  end6 = arg_end(20);
  void * argtable6[] = {help6, end6};
  argtable[6] = argtable6;

  /* verify all argtable[] entries were allocated sucessfully */
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
      ret = camera_init();
      if (ret) return ret;
      ret = camera_info();
      if (ret) return ret;
      ret = camera_fini();
      if (ret) return ret;
      return 0;
    }
    /* --get-temperature */
  } else if (nerrors[2] == 0) {
    if (gettemp2->count > 0) {
      ret = camera_init();
      if (ret) return ret;
      ret = camera_get_temp();
      if (ret) return ret;
      ret = camera_fini();
      if (ret) return ret;
      return 0;
    }
    /* --set-temperature */
  } else if (nerrors[3] == 0) {
    if (settemp3->count > 0) {
      ret = camera_init();
      if (ret) return ret;
      ret = camera_set_temp(settemp3->ival[0]);
      if (ret) return ret;
      ret = camera_fini();
      if (ret) return ret;
      return 0;
    }
    /* --fan */
  } else if (nerrors[4] == 0) {
    if (strcmp("on", fan4->sval[0]) == 0) {
      ret = camera_init();
      if (ret) return ret;
      ret = camera_set_fan(1);
    } else if (strcmp("off", fan4->sval[0]) == 0) {
      ret = camera_init();
      if (ret) return ret;
      ret = camera_set_fan(0);
    } else {
      fprintf(stderr, "Cannot parse the option for --fan.\n");
      ret = -1;
    }
    if (ret) return ret;
    ret = camera_fini();
    if (ret) return ret;
    return 0;
    /* --acquire */
  } else if (nerrors[5] == 0) {
    if (acquire5->count > 0) {
      //camera_acquire();
    }
    /* --help */
  } else if (nerrors[6] == 0) {
    if (help6) {
      ret = camera_help();
      if (ret) return ret;
      return 0;
    }
    /* incorrent or partially incorrect argument syntaxes */
  } else {
    if (settemp3->count > 0) {
      arg_print_errors(stdout, end3, progname);
      printf("usage: %s ", progname);
      arg_print_syntax(stdout, argtable3, "\n");
    } else if (fan4->count > 0) {
      arg_print_errors(stdout, end4, progname);
      printf("usage: %s ", progname);
      arg_print_syntax(stdout, argtable4, "\n");
    } else if (acquire5->count > 0) {
      arg_print_errors(stdout, end5, progname);
      printf("usage: %s ", progname);
      arg_print_syntax(stdout, argtable5, "\n");
    } else {
      printf("%s: unable to parse arguments, see syntax below:\n", progname);
      ret = 0;
      ret = camera_help();
      return ret;
    }
    return EXIT_FAILURE;
  }

  /* no command line options at all */
  printf("Try '%s --help' for more information.\n", progname);
  return (EXIT_SUCCESS);
}

int camera_fini() {
  int i;
  if (fli->active_camera != FLI_INVALID_DEVICE) {
    return fli_unlock_and_close_device(&fli->active_camera);
  }
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    arg_freetable(argtable[i], sizeof (argtable[i]) / sizeof (argtable[i][0]));
  }
  return (0);
}

int camera_init() {
  int ret;
  ret = fli_scan();
  if (ret) return ret;
  if (fli->num_cameras == 1 && fli->active_camera == FLI_INVALID_DEVICE) {
    ret = fli_open_and_lock_first_camera(&fli->active_camera);
    if (ret) return ret;
  } else {
    if (fli->num_cameras == 0) {
      fprintf(stderr, "Could not find any cameras\n");
    } else {
      fprintf(stderr, "Found more than 1 cameras, but flicamtest can only handle 1 camera ATM\n");
    }
    return -1;
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

int camera_help() {
  int i;
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    if (i == 1) {
      printf("Usage: %s", progname);
    } else {
      printf("       %s", progname);
    }
    arg_print_syntax(stdout, argtable[i], "\n");
  }
  printf("Explanation of the options:\n");
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    arg_print_glossary_gnu(stdout, argtable[i]);
  }
  return 0;
}