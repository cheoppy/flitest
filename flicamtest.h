/*
 * File:   flicamtest.h
 * Author: cheoppy
 *
 * Created on November 26, 2012, 1:34 PM
 */

#ifndef FLICAMTEST_H
#define	FLICAMTEST_H

#include "fli_common.h"

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

#ifdef	__cplusplus
}
#endif

#endif	/* FLICAMTEST_H */

