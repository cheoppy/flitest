/*
 * File:   argtable_common.h
 * Author: Gergely Csépány (gcsepany@flyseye.net)
 *
 * Created on November 28, 2012, 2:00 PM
 */

#ifndef ARGTABLE_COMMON_H
#define	ARGTABLE_COMMON_H

#include <argtable2.h>

#ifdef	__cplusplus
extern "C" {
#endif

  int argtable_help(const char * progname, int number_of_syntaxes, void *** argtable);

#ifdef	__cplusplus
}
#endif

#endif	/* ARGTABLE_COMMON_H */

