/*
 * File:   fliflttest.h
 * Author: cheoppy
 *
 * Created on November 26, 2012, 12:55 PM
 */

#ifndef FLIFLTTEST_H
#define	FLIFLTTEST_H

#ifdef	__cplusplus
extern "C" {
#endif

  int filter_home();
  int filter_set_filter(int position);
  int filter_fini();
  int filter_init();
  int filter_get_nfilter();
  int filter_info();

#ifdef	__cplusplus
}
#endif

#endif	/* FLIFLTTEST_H */

