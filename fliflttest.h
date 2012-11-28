/*
 * File:   fliflttest.h
 * Author: Gergely Csépány (gcsepany@flyseye.net)
 *
 * Created on November 26, 2012, 12:55 PM
 */

#ifndef FLIFLTTEST_H
#define	FLIFLTTEST_H

#include "fli_common.h"
#include "argtable_common.h"

#ifdef	__cplusplus
extern "C" {
#endif

  int filter_home(int standalone);
  int filter_set_filter(int position);
  int filter_fini();
  int filter_init();
  int filter_info();

#ifdef	__cplusplus
}
#endif

#endif	/* FLIFLTTEST_H */

