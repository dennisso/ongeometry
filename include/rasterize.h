#ifndef _RASTERIZE_H_
#define _RASTERIZE_H_ 1

#include <geospatial.h>
#include <string.h>
#include <math.h>

#ifndef _GEOS_C_H_
#define _GEOS_C_H_
#include <geos_c.h>
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct geosPolygonRaster {
   int **raster, width, height;
} GEOSPolygonRaster;

typedef struct geosConversionTable {
   double min, max;
   int division;
} GEOSConversionTable;

GEOSConversionTable * GEOSConversionTable_create(double, double, int);
int GEOSConversionTable_destroy(GEOSConversionTable *);
int GEOSConvertTo (GEOSConversionTable *, double);
double GEOSConvertFrom (GEOSConversionTable *, int);

GEOSPolygonRaster * GEOSPolygonRaster_create(int, int);
int GEOSPolygonRaster_destroy(GEOSPolygonRaster *);
int GEOSPolygon_rasterize(GEOSPolygonRaster *, GEOSGeom *,
   GEOSConversionTable *, GEOSConversionTable *);

#endif //_RASTERIZE_H_

