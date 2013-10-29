#include <rasterize.h>

GEOSConversionTable * GEOSConversionTable_create(double min, double max,
   int division)
{
   GEOSConversionTable *convTable;

   if ((convTable = malloc(sizeof(GEOSConversionTable))))       
   {
      convTable->min = min;
      convTable->max = max;
      convTable->division = division;
   }
   
   return convTable;
}

int GEOSConversionTable_destroy(GEOSConversionTable *convTable)
{
   free(convTable);
   convTable = NULL;

   return 0;
}

int GEOSConvertTo (GEOSConversionTable *convTable, double value)
{
   return (int) floor((double) value / (convTable->max - convTable->min)
      * (double) convTable->division);
}

double GEOSConvertFrom (GEOSConversionTable *convTable, int value)
{
   return (double) (value * (convTable->max - convTable->min)
      / (double) convTable->division);
}

GEOSPolygonRaster * GEOSPolygonRaster_create(int width, int height)
{
   GEOSPolygonRaster *polyRaster;

   if ((polyRaster = malloc(sizeof(GEOSPolygonRaster))) &&
      (polyRaster->raster = malloc(sizeof(int *) * width)))
   {
      polyRaster->width = width;
      polyRaster->height = height;
   }

   return polyRaster;
}
int GEOSPolygonRaster_destroy(GEOSPolygonRaster *polyRaster)
{
   for(int row = 0; row < polyRaster->height; row++)
   {
      free(polyRaster->raster[row]);
      polyRaster->raster[row] = NULL;
   }

   free(polyRaster->raster);
   polyRaster->raster = NULL;

   return 0;
}

int GEOSPolygon_rasterize(GEOSPolygonRaster *polyRaster, GEOSGeom *geom,
   GEOSConversionTable *xConvTable, GEOSConversionTable *yConvTable)
{
   GEOSGeom *lineStrings, multiline, intersected;
   GEOSCoordSeq *lineStringCss;

   if ((lineStrings = malloc(sizeof(GEOSGeom) * polyRaster->height)) &&
      (lineStringCss = malloc(sizeof(GEOSCoordSeq) * polyRaster->height)))
   {
      for (int row = 0; row < polyRaster->height; row++)
      {
         lineStringCss[row] = GEOSCoordSeq_create(2, 2);
         GEOSCoordSeq_setX(lineStringCss[row], 0, GEOSConvertFrom(xConvTable, row));
         GEOSCoordSeq_setX(lineStringCss[row], 1, GEOSConvertFrom(xConvTable, row));
         GEOSCoordSeq_setY(lineStringCss[row], 0, GEOSConvertFrom(yConvTable, 0));
         GEOSCoordSeq_setY(lineStringCss[row], 1, GEOSConvertFrom(yConvTable, polyRaster->width));
         lineStrings[row] = GEOSGeom_createLineString(lineStringCss[row]);
      }

      multiline = GEOSGeom_createCollection(GEOS_MULTILINESTRING,
         lineStrings, (unsigned int) polyRaster->height);

      intersected = GEOSIntersection(multiline, *geom);

      GEOSGeom_destroy(multiline);
      GEOSGeom_destroy(intersected);

      free(lineStrings);
      lineStrings = NULL;
      free(lineStringCss);
      lineStringCss = NULL;
   }

   return 0;
}

