/*
 * File:   flicamtest.c
 * Author: Gergely Csépány (gcsepany@flyseye.net)
 *
 * Created on November 26, 2012, 1:13 PM
 */

/* needed for the proper use of usleep */
#define _BSD_SOURCE

/* the gain is not queryable from the camera, so it's set here */
#define ML16803_CAMERA_GAIN 1.0

/* this is to be counted from 1 to 7, for human legibility */
#define NUMBER_OF_DIFFERENT_SYNTAXES 7

#include "flicamtest.h"
#include <regex.h>
#include <fitsio.h>
#include <fitsio2.h>
#include <sys/time.h>

extern fli_status * fli;

/* global variables for argtable */
struct arg_lit *info1, *gettemp2, *verbose6, *help7;
struct arg_str *fan4, *acquire6, *shutter5, *shutter6, *bin6, *size6, *offset6, *mode6;
struct arg_int *settemp3;
struct arg_file *output6;
struct arg_end *end1, *end2, *end3, *end4, *end5, *end6, *end7;
void **argtable[NUMBER_OF_DIFFERENT_SYNTAXES + 1];
const char* progname = "flicamtest";

int main(int argc, char** argv) {
  int ret, i;
  int nerrors[NUMBER_OF_DIFFERENT_SYNTAXES + 1];

  /* argtable setup */
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

  /* SYNTAX 5: [--shutter]  */
  shutter5 = arg_str1(NULL, "shutter", "(open|close)", "Open or close the shutter");
  end5 = arg_end(20);
  void * argtable5[] = {shutter5, end5};
  argtable[5] = argtable5;

  /* SYNTAX 6: --acquire <time/sec> --shutter {open|close} --output xyz.fits
             [--bin <bx>,<by>] [--offset <x0>,<y0>] [--size <sx>,<sy>]
             [--mode <mode>...] [--verbose]
   */
  acquire6 = arg_str1(NULL, "acquire", "<time>", "Exposure time in sec");
  shutter6 = arg_str1(NULL, "shutter", "(open|close)", "Whether to open or close the shutter");
  output6 = arg_file1(NULL, "output", "xyz.fits", "Output filename for the FITS file");
  bin6 = arg_str0(NULL, "bin", "<bx,by>", "Binning options in X,Y format");
  offset6 = arg_str0(NULL, "offset", "<x0,y0>", "Offset options in X,Y format");
  size6 = arg_str0(NULL, "size", "<sx,sy>", "Sizes in X,Y format");
  mode6 = arg_str0(NULL, "mode", "(1mhz|8mhz)", "Download speed");
  verbose6 = arg_lit0(NULL, "verbose", "Show verbose output");
  end6 = arg_end(20);
  void * argtable6[] = {acquire6, shutter6, output6, bin6, offset6, size6, mode6, verbose6, end6};
  argtable[6] = argtable6;

  /* SYNTAX 7: [--help]*/
  help7 = arg_lit1(NULL, "help", "Print this help");
  end7 = arg_end(20);
  void * argtable7[] = {help7, end7};
  argtable[7] = argtable7;

  /* verify all argtable[] entries were allocated successfully */
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    if (arg_nullcheck(argtable[i]) != 0) {
      printf("%s: insufficient memory\n", progname);
      return EXIT_FAILURE;
    }
  }

  /* set defaults for the optional arguments */
  bin6->sval[0] = "1,1";
  offset6->sval[0] = "0,0";
  size6->sval[0] = "4096,4096";
  mode6->sval[0] = "8mhz";

  /* parse all argument possibilities */
  for (i = 1; i <= NUMBER_OF_DIFFERENT_SYNTAXES; i++) {
    nerrors[i] = arg_parse(argc, argv, argtable[i]);
  }

  /* select the right command */
  /* --info */
  if (nerrors[1] == 0) {
    if (info1->count > 0) {
      ret = camera_info();
      if (ret) return ret;
      return 0;
    }
    /* --get-temperature */
  } else if (nerrors[2] == 0) {
    if (gettemp2->count > 0) {
      ret = camera_get_temp();
      if (ret) return ret;
      return 0;
    }
    /* --set-temperature */
  } else if (nerrors[3] == 0) {
    if (settemp3->count > 0) {
      ret = camera_set_temp(settemp3->ival[0]);
      if (ret) return ret;
      return 0;
    }
    /* --fan */
  } else if (nerrors[4] == 0) {
    int fan;
    if (strcmp("on", fan4->sval[0]) == 0) {
      fan = 1;
    } else if (strcmp("off", fan4->sval[0]) == 0) {
      fan = 0;
    } else {
      fprintf(stderr, "Cannot parse the option for --fan, see --help.\n");
      return -1;
    }
    ret = camera_set_fan(fan);
    if (ret) return ret;
    return 0;
    /* --shutter */
  } else if (nerrors[5] == 0) {
    int open_shutter;
    if (strcmp("open", shutter5->sval[0]) == 0) {
      open_shutter = 1;
    } else if (strcmp("close", shutter5->sval[0]) == 0) {
      open_shutter = 0;
    } else {
      fprintf(stderr, "Cannot parse the option for --shutter, see --help.\n");
      return -1;
    }
    ret = camera_control_shutter(open_shutter);
    if (ret) return ret;
    return 0;
    /* --acquire */
  } else if (nerrors[6] == 0) {
    if (acquire6->count > 0) {

      /* local variables to store the arguments */
      float exposure_time;
      int is_dark;
      char * output_filename;
      char * bin_options;
      char * offset_options;
      char * size_options;
      int bx, by;
      int x0, y0;
      int sx, sy;
      int one_mhz_speed;
      int is_verbose = 0;

      /* copy const char arrays to char arrays to suppress warnings */
      output_filename = (char*) malloc((strlen(output6->filename[0])) * sizeof (char));
      bin_options = (char*) malloc((strlen(bin6->sval[0])) * sizeof (char));
      offset_options = (char*) malloc((strlen(offset6->sval[0])) * sizeof (char));
      size_options = (char*) malloc((strlen(size6->sval[0])) * sizeof (char));
      strcpy(output_filename, output6->filename[0]);
      strcpy(bin_options, bin6->sval[0]);
      strcpy(offset_options, offset6->sval[0]);
      strcpy(size_options, size6->sval[0]);

      /* process arguments */
      /* exposure time */
      exposure_time = atof(acquire6->sval[0]);
      if (exposure_time < 0) {
        fprintf(stderr, "Exposure time cannot be below zero (given %f).\n", exposure_time);
        return -1;
      }

      /* shutter */
      if (strcmp("open", shutter6->sval[0]) == 0) {
        is_dark = 0;
      } else if (strcmp("close", shutter6->sval[0]) == 0) {
        is_dark = 1;
      } else {
        fprintf(stderr, "Cannot parse the option for --shutter, see --help.\n");
        return -1;
      }

      /* bin, size and offset */
      ret = parse_comma_separated_values(bin_options, &bx, &by, "bin");
      if (ret) return ret;
      ret = parse_comma_separated_values(offset_options, &x0, &y0, "offset");
      if (ret) return ret;
      ret = parse_comma_separated_values(size_options, &sx, &sy, "size");
      if (ret) return ret;

      /* mode */
      if (strcmp("1mhz", mode6->sval[0]) == 0) {
        one_mhz_speed = 1;
      } else if (strcmp("8mhz", mode6->sval[0]) == 0) {
        one_mhz_speed = 0;
      } else {
        fprintf(stderr, "Cannot parse the option for --mode, see --help.\n");
        return -1;
      }

      /* verbose */
      if (verbose6->count > 0) is_verbose = 1;

      ret = camera_acquire(exposure_time, is_dark, output_filename,
              bx, by, x0, y0, sx, sy, one_mhz_speed, is_verbose);
      if (ret) return ret;
      return 0;
    }
    /* --help */
  } else if (nerrors[7] == 0) {
    if (help7) {
      ret = camera_help();
      if (ret) return ret;
      return 0;
    }
    /* incorrect or partially incorrect argument syntaxes */
  } else {
    if (settemp3->count > 0) {
      arg_print_errors(stdout, end3, progname);
      printf("usage: %s ", progname);
      arg_print_syntax(stdout, argtable3, "\n");
    } else if (fan4->count > 0) {
      arg_print_errors(stdout, end4, progname);
      printf("usage: %s ", progname);
      arg_print_syntax(stdout, argtable4, "\n");
    } else if (acquire6->count > 0) {
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

/**
 * Parses strings like "X,Y" using regex and strtok. Stores X in *x and Y in *y.
 * Both X and Y are meant to be integers.
 *
 * @param value_string String having a format of "X,Y"
 * @param x integer pointer where to store X
 * @param y integer pointer where to store Y
 * @param option_name Name of the parsed option, used for error messages
 * @return Zero on success, non-zero on failure.
 */
int parse_comma_separated_values(char * value_string, int * x, int * y, char * option_name) {
  regex_t regex;
  int ret, i;
  char msgbuf[100];
  char * next_word;

  /* compile regex expression */
  ret = regcomp(&regex, "^[[:digit:]]*,[[:digit:]]*$", 0);
  if (ret) {
    fprintf(stderr, "Could not compile regex\n");
    return -1;
  }

  /* execute regex */
  ret = regexec(&regex, value_string, 0, NULL, 0);
  /* if the string was matched, tokenize it and convert to integer */
  if (!ret) {
    next_word = strtok(value_string, ",");
    i = 0;
    while (next_word != NULL) {
      i++;
      if (i == 1) *x = atoi(next_word);
      if (i == 2) *y = atoi(next_word);
      next_word = strtok(NULL, ",");
    }
  } else if (ret == REG_NOMATCH) {
    fprintf(stderr, "The --%s option has an invalid value: \"%s\"\n", option_name, value_string);
    return -1;
  } else {
    regerror(ret, &regex, msgbuf, sizeof (msgbuf));
    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    return -1;
  }
  return 0;
}

/**
 * Unlocks and closes the camera handle; and frees all the argtable arrays.
 *
 * @return Zero on success, non-zero on failure.
 */
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

/**
 * Scans for FLI cameras, and if exactly one is found, opens and lock it.
 *
 * @return Zero on success, non-zero on failure.
 */
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

/**
 * Prints the current temperature of the camera CCD, base and the cooling power.
 *
 * @return Zero on success, non-zero on failure.
 */
int camera_get_temp() {
  int err;
  double ccd_temp, base_temp, cooler_power;

  err = camera_init();
  if (err) return err;

  CALLFLIAPI(FLIReadTemperature(fli->active_camera, FLI_TEMPERATURE_CCD, &ccd_temp), "FLIReadTemperature");
  CALLFLIAPI(FLIReadTemperature(fli->active_camera, FLI_TEMPERATURE_BASE, &base_temp), "FLIReadTemperature");
  CALLFLIAPI(FLIGetCoolerPower(fli->active_camera, &cooler_power), "FLIGetCoolerPower");

  fprintf(stdout, "camera CCD  temperature: %f°C\n", ccd_temp);
  fprintf(stdout, "camera base temperature: %f°C\n", base_temp);
  fprintf(stdout, "camera cooler power: %f%%\n", cooler_power);

  err = camera_fini();
  if (err) return err;

  return 0;
}

/**
 * Sets the temperature of the camera CCD to set_temp.
 *
 * @param set_temp integer, the temperature to set the CCD to
 * @return Zero on success, non-zero on failure.
 */
int camera_set_temp(int set_temp) {
  int err;

  err = camera_init();
  if (err) return err;

  CALLFLIAPI(FLISetTemperature(fli->active_camera, set_temp), "FLISetTemperature");

  err = camera_fini();
  if (err) return err;

  return 0;
}

/**
 * Opens (1) or closes (0) the shutter of the camera.
 *
 * @param status integer, 0 or 1.
 * @return Zero on success, non-zero on failure.
 */
int camera_control_shutter(int status) {
  int err;

  err = camera_init();
  if (err) return err;

  if (status == 1 || status == 0) {
    if (status == 1) {
      /* Open the shutter */
      CALLFLIAPI(FLIControlShutter(fli->active_camera, FLI_SHUTTER_OPEN), "FLIControlShutter");
    } else if (status == 0) {
      /* Close the shutter */
      CALLFLIAPI(FLIControlShutter(fli->active_camera, FLI_SHUTTER_CLOSE), "FLIControlShutter");
    }
  } else {
    return -1;
  }

  err = camera_fini();
  if (err) return err;

  return 0;
}

/**
 * Prints information about the camera.
 *
 * @return Zero on success, non-zero on failure.
 */
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

  err = camera_init();
  if (err) return err;

  CALLFLIAPI(FLIGetFWRevision(fli->active_camera, &fwrev), "FLIGetFWRevision");
  CALLFLIAPI(FLIGetHWRevision(fli->active_camera, &hwrev), "FLIGetHWRevision");
  CALLFLIAPI(FLIGetSerialString(fli->active_camera, serial, 128), "FLIGetSerialString");
  CALLFLIAPI(FLIGetModel(fli->active_camera, model, 128), "FLIGetModel");
  CALLFLIAPI(FLIGetLibVersion(lib_version, 128), "FLIGetLibVersion");
  CALLFLIAPI(FLIGetArrayArea(fli->active_camera, &total_ul_x, &total_ul_y, &total_lr_x, &total_lr_y), "FLIGetLibVersion");
  CALLFLIAPI(FLIGetVisibleArea(fli->active_camera, &visible_ul_x, &visible_ul_y, &visible_lr_x, &visible_lr_y), "FLIGetVisibleArea");
  CALLFLIAPI(FLIGetPixelSize(fli->active_camera, &pixel_size_x, &pixel_size_y), "FLIGetPixelSize");

  fprintf(stdout, "camera FW revision: 0x%04x\n", (int) fwrev);
  fprintf(stdout, "camera HW revision: 0x%04x\n", (int) hwrev);
  fprintf(stdout, "camera serial number: %s\n", serial);
  fprintf(stdout, "camera model: %s\n", model);
  fprintf(stdout, "camera lib version: %s\n", lib_version);
  fprintf(stdout, "camera total area: %ld, %ld, %ld, %ld\n", total_ul_x, total_ul_y, total_lr_x, total_lr_y);
  fprintf(stdout, "camera visible area: %ld, %ld, %ld, %ld\n", visible_ul_x, visible_ul_y, visible_lr_x, visible_lr_y);
  fprintf(stdout, "camera pixel sizes: %fm, %fm\n", pixel_size_x, pixel_size_y);

  err = camera_fini();
  if (err) return err;

  return (0);
}

/**
 * Turns on (1) or off (0) the fan on the given camera
 *
 * @param status
 * @return Zero on success, non-zero on failure.
 */
int camera_set_fan(int status) {
  int err;

  err = camera_init();
  if (err) return err;

  if (status == 1 || status == 0) {
    if (status == 1) {
      /* Turn on the fan */
      CALLFLIAPI(FLISetFanSpeed(fli->active_camera, FLI_FAN_SPEED_ON), "FLISetFanSpeed");
    } else if (status == 0) {
      /* Turn off the fan */
      CALLFLIAPI(FLISetFanSpeed(fli->active_camera, FLI_FAN_SPEED_OFF), "FLISetFanSpeed");
    }
  } else {
    return -1;
  }

  err = camera_fini();
  if (err) return err;

  return 0;
}

/**
 * Prints the help based on argtable.
 *
 * @return  Zero on success, non-zero on failure.
 */
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

/**
 * Exposures, downloads and writes the image to a FITS file.
 *
 * @param exposure_time float, exposure time in seconds
 * @param is_dark integer, 0 or 1, indicates if the frame is dark (1) or normal (0)
 * @param output_filename string, where to save the FITS file
 * @param bx, integer, horizontal binning factor
 * @param by, integer, vertical binning factor
 * @param x0, integer, horizontal offset
 * @param y0, integer, vertical offset
 * @param sx, integer, horizontal image size
 * @param sy, integer, vertical image size
 * @param one_mhz_speed, integer, 0 or 1, indicates if the download speed is 1 MHz (1) or 8 MHz (0)
 * @param is_verbose, integer, 0 or 1, if 1, produces verbose logging to stdout
 * @return  Zero on success, non-zero on failure.
 */
int camera_acquire(float exposure_time, int is_dark, char * output_filename,
        long bx, long by, int x0, int y0, int sx, int sy, int one_mhz_speed,
        int is_verbose) {
  int err;
  int i;

  err = camera_init();
  if (err) return err;

  /* set download speed */
  if (one_mhz_speed) {
    if (is_verbose) fprintf(stdout, "setting the camera mode to 1, indicating 1 MHz speed\n");
    CALLFLIAPI(FLISetCameraMode(fli->active_camera, 1), "FLISetCameraMode");
  } else {
    if (is_verbose) fprintf(stdout, "setting the camera mode to 0, indicating 8 MHz speed\n");
    CALLFLIAPI(FLISetCameraMode(fli->active_camera, 0), "FLISetCameraMode");
  }

  /* flush the ccd 5 times before exposing */
  if (is_verbose) fprintf(stdout, "flushing the ccd 5 times before exposing \n");
  CALLFLIAPI(FLISetNFlushes(fli->active_camera, 5), "FLISetNFlushes");

  /* set exposure time */
  if (is_verbose) fprintf(stdout, "set exp time to %f seconds\n", exposure_time * 1000);
  CALLFLIAPI(FLISetExposureTime(fli->active_camera, exposure_time * 1000), "FLISetExposureTime");

  /* set frame type (dark, normal) */
  if (is_dark) {
    if (is_verbose) fprintf(stdout, "setting frame mode to dark (is_dark: %d)\n", is_dark);
    CALLFLIAPI(FLISetFrameType(fli->active_camera, FLI_FRAME_TYPE_DARK), "FLISetFrameType");
  } else {
    if (is_verbose) fprintf(stdout, "setting frame mode to normal (is_dark: %d)\n", is_dark);
    CALLFLIAPI(FLISetFrameType(fli->active_camera, FLI_FRAME_TYPE_NORMAL), "FLISetFrameType");
  }

  /* get visible pixel coordinates */
  long visible_ul_x, visible_ul_y, visible_lr_x, visible_lr_y;
  if (is_verbose) fprintf(stdout, "getting the visible sizes\n");
  CALLFLIAPI(FLIGetVisibleArea(fli->active_camera, &visible_ul_x, &visible_ul_y, &visible_lr_x, &visible_lr_y), "FLIGetVisibleArea");

  /* check the sizes */
  if (visible_ul_y + sy > visible_lr_y || visible_ul_x + sx > visible_lr_x) {
    fprintf(stderr, "The set size is beyond the limits of the camera");
    return -1;
  }
  /* set image size based on mode and visible size*/
  if (is_verbose) fprintf(stdout, "setting the image area to: %ld, %ld, %ld, %ld \n", visible_ul_x, visible_ul_y, visible_ul_x + sx, visible_ul_y + sy);
  CALLFLIAPI(FLISetImageArea(fli->active_camera, visible_ul_x, visible_ul_y, visible_ul_x + sx, visible_ul_y + sy), "FLISetImageArea");

  /* setting the horizontal binning */
  if (is_verbose) fprintf(stdout, "setting the horizontal binning to: %ld \n", bx);
  CALLFLIAPI(FLISetHBin(fli->active_camera, bx), "FLISetHBin");

  /* setting the vertical binning */
  if (is_verbose) fprintf(stdout, "setting the vertical binning: %ld \n", by);
  CALLFLIAPI(FLISetVBin(fli->active_camera, by), "FLISetVBin");

  /* expose the frame */
  /* this function return immediately */
  if (is_verbose) fprintf(stdout, "starting the exposure...\n");
  CALLFLIAPI(FLIExposeFrame(fli->active_camera), "FLIExposeFrame");

  /* wait until expose is ready */
  long timeleft = 1;
  while (timeleft) {
    CALLFLIAPI(FLIGetExposureStatus(fli->active_camera, &timeleft), "FLIGetExposureStatus");
    if (is_verbose) fprintf(stdout, "checking the exposure status (%ld ms to go) ...\n", timeleft);
    usleep(1000 * 100);
  }

  unsigned short *img;
  img = (unsigned short*) malloc(sy * sx * sizeof (unsigned short));

  /* download the image */
  struct timeval t0, t1;
  clock_t cpu_clock;
  long number_of_pixels = sx*sy;

  if (is_verbose) fprintf(stdout, "downloading the image (canonical speed: %s)...\n", one_mhz_speed ? "1 MHz" : "8 MHz");
  /* start the time measurement */
  cpu_clock = clock();
  gettimeofday(&t0, 0);
  for (i = 0; i < sy; i++) {
    CALLFLIAPI(FLIGrabRow(fli->active_camera, &img[i * sx], sx), "FLIGrabRow");
  }
  /* finish the time measurement */
  cpu_clock = clock() - cpu_clock;
  gettimeofday(&t1, 0);

  long long elapsed = (t1.tv_sec - t0.tv_sec)*1000000LL + t1.tv_usec - t0.tv_usec;
  float elapsed_sec = elapsed / (1000.0 * 1000.0);
  float cpu_usage = ((float) cpu_clock) / CLOCKS_PER_SEC / elapsed_sec * 100.0;
  float real_download_speed = (((float) number_of_pixels) / (1000.0 * 1000.0)) / elapsed_sec;

  if (is_verbose) fprintf(stdout, "download time: %10.6f s, real speed: %6.4f MHz\n", elapsed_sec, real_download_speed);
  if (is_verbose) fprintf(stdout, "average CPU usage during download: %6.2f%%\n", cpu_usage);

  /* save the image */
  if (is_verbose) fprintf(stdout, "saving the image (%s)...\n", output_filename);
  err = camera_save_fits_image(output_filename, sx, sy, img);
  if (err) return err;

  err = camera_fini();
  if (err) return err;
  return 0;
}

/**
 * Saves the image to a FITS file using the cfitsio library.
 *
 * @param output_filename string, where to save the FITS file
 * @param sx, integer, horizontal image size
 * @param sy, integer, vertical image size
 * @param data, rows of a two dimensional integer array, image data
 * @return  Zero on success, non-zero on failure.
 */
int camera_save_fits_image(char * output_filename, long sx, long sy, unsigned short * data) {
  fitsfile *fptr;
  int status = 0;
  long fpixel, nelements;
  float gain;
  int bitpix = USHORT_IMG;
  long naxis = 2;
  long naxes[2] = {sx, sy};

  /* Delete old file if it already exists */
  remove(output_filename);

  /* create new FITS file */
  if (fits_create_file(&fptr, output_filename, &status)) {
    if (status) fits_report_error(stderr, status);
    return status;
  }

  /* the setting of bitpix = USHORT_IMG implies that BITPIX = 16,
   BSCALE = 1.0 and BZERO = 32768, and the latter two keywords are
   added automatically. */
  if (fits_create_img(fptr, bitpix, naxis, naxes, &status)) {
    if (status) fits_report_error(stderr, status);
    return status;
  }

  /* first pixel to write */
  fpixel = 1;
  /* number of pixels to write */
  nelements = naxes[0] * naxes[1];

  /* write the array of unsigned integers to the FITS file */
  if (fits_write_img(fptr, TUSHORT, fpixel, nelements, data, &status)) {
    if (status) fits_report_error(stderr, status);
    return status;
  }

  /* write the gain parameter of the camera to the header */
  gain = ML16803_CAMERA_GAIN;
  if (fits_update_key(fptr, TFLOAT, "GAIN", &gain, "Camera gain of ML16803", &status)) {
    if (status) fits_report_error(stderr, status);
    return status;
  }

  /* close the file */
  if (fits_close_file(fptr, &status)) {
    if (status) fits_report_error(stderr, status);
    return status;
  }

  return 0;
}
