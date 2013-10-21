#ifndef _GEOSPATIAL_H_
#define _GEOSPATIAL_H_ 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <shapefil.h>
#include <geos_c.h>

// public fcn prototypes

extern void notice (const char *, ...);
extern void log_and_exit(const char *, ...);

extern int shpOpen(SHPHandle *, char *);
extern int isOnLand(SHPHandle *, double, double);

#endif // _GEOSPATIAL_H_
