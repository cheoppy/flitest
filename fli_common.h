/*
 * File:   fli_common.h
 * Author: Gergely Csépány (gcsepany@flyseye.net)
 *
 * Created on November 26, 2012, 12:37 PM
 */
#ifndef FLI_COMMON_H
#define	FLI_COMMON_H

#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libfli.h"

#include <unistd.h>

#define CALLFLIAPI(F, G) { if (fli_check_error((F), G)) return -1; }

#ifdef	__cplusplus
extern "C" {
#endif

  typedef struct {
    int num_cameras;
    int num_filters;
    char ** filters;
    char ** cameras;
    flidev_t active_camera;
    flidev_t active_filter;
    int current_filter;
    int set_1mhz_speed;
  } fli_status;

  int fli_check_error(int error, char * source);
  int fli_open_and_lock_first_camera(flidev_t * fli_dev);
  int fli_open_and_lock_first_filter(flidev_t * fli_dev);
  int fli_open_and_lock_camera(int i, flidev_t * fli_dev);
  int fli_open_and_lock_filter(int i, flidev_t * fli_dev);
  int fli_open_and_lock_device(int i, flidev_t *fli_dev, char ** device_list, flidomain_t domain);
  int fli_unlock_and_close_device(flidev_t * fli_dev);
  int fli_scan();
  int fli_scan_domain(char * device_type, flidomain_t domain, char *** device_list, int *device_count);

#ifdef	__cplusplus
}
#endif

#endif	/* FLI_COMMON_H */

