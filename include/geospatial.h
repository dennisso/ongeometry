#ifndef _GEOSPATIAL_H_
#define _GEOSPATIAL_H_ 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <shapefil.h>

#ifndef _GEOS_C_H_
#define _GEOS_C_H_
#include <geos_c.h>
#endif

// public strucs

typedef struct {
   int numEntities;
   GEOSCoordSeq **linearRingCsList;
   GEOSGeom **linearRingList, *polygonList;
} GEOSPolygons; 

// public fcn prototypes

extern void notice (const char *, ...);
extern void log_and_exit(const char *, ...);

extern GEOSPolygons * GEOSPolygons_create(int);
extern int GEOSPolygons_destroy(GEOSPolygons **);

extern int shpOpen(SHPHandle *, char *);
extern int shpLoad(SHPHandle *hShp, GEOSPolygons **);
extern int shpUnload(GEOSPolygons **);
extern int isOnPolygons(GEOSPolygons **, double, double);

#endif // _GEOSPATIAL_H_

