/*
 * File:   fli_common.h
 * Author: cheoppy
 *
 * Created on November 26, 2012, 12:37 PM
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libfli.h"


#ifndef FLI_COMMON_H
#define	FLI_COMMON_H

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

  /* domains for the FLIList command  */
  static flidomain_t FLI_USB_CAMERA_DOMAIN = FLIDOMAIN_USB | FLIDEVICE_CAMERA;
  static flidomain_t FLI_USB_FILTER_DOMAIN = FLIDOMAIN_USB | FLIDEVICE_FILTERWHEEL;

  int fli_check_error(int error, char * source);
  int fli_open_and_lock_first_camera(flidev_t * fli_dev);
  int fli_open_and_lock_first_filter(flidev_t * fli_dev);
  int fli_open_and_lock_camera(int i, flidev_t * fli_dev);
  int fli_open_and_lock_filter(int i, flidev_t * fli_dev);
  int fli_open_and_lock_device(int i, flidev_t *fli_dev, char ** device_list, flidomain_t domain);
  int fli_unlock_and_close_device(flidev_t * fli_dev);
  int fli_scan();
  int fli_scan_domain(char * device_type, flidomain_t domain, char *** device_list, int *device_count);

  static fli_status fli_m = {
    .num_filters = 0,
    .num_cameras = 0,
    .cameras = NULL,
    .filters = NULL,
    .current_filter = -1,
    .active_camera = FLI_INVALID_DEVICE,
    .active_filter = FLI_INVALID_DEVICE,
    .set_1mhz_speed = 0
  };
  static fli_status * fli = &fli_m;

#ifdef	__cplusplus
}
#endif

#endif	/* FLI_COMMON_H */

