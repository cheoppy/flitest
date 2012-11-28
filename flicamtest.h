/*
 * File:   flicamtest.h
 * Author: Gergely Csépány (gcsepany@flyseye.net)
 *
 * Created on November 26, 2012, 1:34 PM
 */

#ifndef FLICAMTEST_H
#define	FLICAMTEST_H

#include "fli_common.h"
#include "argtable_common.h"

#ifdef	__cplusplus
extern "C" {
#endif

  int camera_fini();
  int camera_init();
  int camera_get_temp();
  int camera_info();
  int camera_set_fan(int status);
  int camera_set_temp(int set_temp);
  int camera_control_shutter(int status);
  int camera_help();
  int camera_acquire(float exposure_time, int is_dark, char * output_filename,
          long bx, long by, int x0, int y0, int sx, int sy, int one_mhz_speed,
          int is_verbose);
  int parse_comma_separated_values(char * value_string, int * x, int * y, char * option_name);
  int camera_save_fits_image(char * output_filename, long sx, long sy, unsigned short * data);

#ifdef	__cplusplus
}
#endif

#endif	/* FLICAMTEST_H */

