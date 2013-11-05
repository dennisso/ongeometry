#include <geospatial.h>

// function declarations
int GEOSPolygonRaster_set(GEOSPolygonRaster **, int, int);

/*	
 *	GEOSConversionTable_create: allocates and creates a conversation table
 * returns: GEOSConversionTable if successful otherwise NULL
 */
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

/*
 * GEOSConversionTable_destroy: free the conversion table
 * returns: 0
 */
int GEOSConversionTable_destroy(GEOSConversionTable *convTable)
{
   free(convTable);
   convTable = NULL;

   return 0;
}

/*
 * GEOSConvertTo: Converts the given
 * returns: result of the conversion
 */
int GEOSConvertTo (GEOSConversionTable *convTable, double value)
{
   return (int) floor((double) (value - convTable->min) / (convTable->max - convTable->min)
      * (double) convTable->division + 0.5);
}

/*
 * GEOSConvertFrom: Reverses the conversion
 * returns: results of the reverse conversion 
 */
double GEOSConvertFrom (GEOSConversionTable *convTable, int value)
{
   return (double) (value * (convTable->max - convTable->min)
      / (double) convTable->division) + (double) convTable->min;
}

/*
 * GEOSPolygonRaster_create: allocs and creates a rectangular raster
 * returns: pointer to raster if sucessful otherwise NULL
 */
GEOSPolygonRaster * GEOSPolygonRaster_create(int width, int height)
{
   GEOSPolygonRaster *polyRaster;

   if ((polyRaster = malloc(sizeof(GEOSPolygonRaster))) &&
      (polyRaster->raster = malloc(sizeof(int *) * height)))
   {
      polyRaster->width = width;
      polyRaster->height = height;

      for (int row = 0; row < height; row++)
      {
         polyRaster->raster[row] = calloc(1, sizeof(int) * width);
      }
   }

   return polyRaster;
}

/*
 * GEOSPolygonRaster_destroy: destroy raster and free the pointer
 * returns: 0
 */
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

/*
 * GEOSPolygon_rasterize: rasterized a GEOSGeom
 * returns: 0
 */
int GEOSPolygon_rasterize(GEOSPolygonRaster **polyRaster, GEOSGeom *geom,
   GEOSConversionTable **xConvTable, GEOSConversionTable **yConvTable)
{
   GEOSGeom *lineStrings, multiline, intersected;
   GEOSCoordSeq *lineStringCss;

   if ((lineStrings = calloc(1, sizeof(GEOSGeom) * (*polyRaster)->height)) &&
      (lineStringCss = calloc(1, sizeof(GEOSCoordSeq) * (*polyRaster)->height)))
   {
      for (int row = 0; row < (*polyRaster)->height; row++)
      {
         lineStringCss[row] = GEOSCoordSeq_create(2, 2);
         GEOSCoordSeq_setX(lineStringCss[row], 0, GEOSConvertFrom((*xConvTable), 0)); //X is column (LONG)
         GEOSCoordSeq_setX(lineStringCss[row], 1, GEOSConvertFrom((*xConvTable), (*polyRaster)->width));
         GEOSCoordSeq_setY(lineStringCss[row], 0, GEOSConvertFrom((*yConvTable), row)); //Y is row (LAT)
         GEOSCoordSeq_setY(lineStringCss[row], 1, GEOSConvertFrom((*yConvTable), row));
         lineStrings[row] = GEOSGeom_createLineString(lineStringCss[row]);
      }

      multiline = GEOSGeom_createCollection(GEOS_MULTILINESTRING,
         lineStrings, (unsigned int) (*polyRaster)->height);

      intersected = GEOSIntersection(multiline, *geom);
      
      debug_run(char *wkt_c = GEOSGeomToWKT(multiline);)
		debug_printf("%s\n", wkt_c)
      debug_run(wkt_c = GEOSGeomToWKT(intersected);)
      debug_printf("%s\n", wkt_c)

      debug_printf("Parsing %d lines\n", GEOSGetNumGeometries(intersected))
      
		for(int lineNum = 0; lineNum < GEOSGetNumGeometries(intersected); lineNum++)
      {
         double vertex[2][2];
         int index[2][2];
         GEOSGeom line;
         GEOSCoordSeq lineCs;
         line = GEOSGetGeometryN(intersected, lineNum);
         lineCs = GEOSGeom_getCoordSeq(line);

         if (GEOSGetNumCoordinates(line) == 2)
         {
            GEOSCoordSeq_getX(lineCs, 0, &vertex[0][1]); //X is column
            GEOSCoordSeq_getY(lineCs, 0, &vertex[0][0]); //Y is row
            GEOSCoordSeq_getX(lineCs, 1, &vertex[1][1]);
            GEOSCoordSeq_getY(lineCs, 1, &vertex[1][0]); 
            
            index[0][1] = GEOSConvertTo((*xConvTable), vertex[0][1]); //column
            index[0][0] = GEOSConvertTo((*yConvTable), vertex[0][0]); //row
            index[1][1] = GEOSConvertTo((*xConvTable), vertex[1][1]);
            index[1][0] = GEOSConvertTo((*yConvTable), vertex[1][0]);
          
            //printf("P1 (%f, %f); P2 (%f, %f)\n",
            //	vertex[0][0], vertex[0][1], vertex[1][0], vertex[1][1]);

            //printf("Line %d: P1 (%d, %d); P2 (%d, %d)\n",
            //	lineNum, index[0][0], index[0][1], index[1][0], index[1][1]);
                      
            for (int col = index[0][1]; col < index[1][1]; col++)
            {
               GEOSPolygonRaster_set(polyRaster, col, index[0][0]);
            }
         }
         else
         {
            warn_printf("%d line has more than 2 coordinates\n", lineNum)
            GEOSCoordSeq_getX(lineCs, 0, &vertex[0][1]); //X is column
            GEOSCoordSeq_getY(lineCs, 0, &vertex[0][0]); //Y is row
            index[0][1] = GEOSConvertTo((*xConvTable), vertex[0][1]);
            index[0][0] = GEOSConvertTo((*yConvTable), vertex[0][0]);
         
            GEOSPolygonRaster_set(polyRaster, index[0][1], index[0][0]);
         }
      }

      GEOSGeom_destroy(multiline);
      GEOSGeom_destroy(intersected);

      free(lineStrings);
      lineStrings = NULL;
      free(lineStringCss);
      lineStringCss = NULL;
   }

   return 0;
}

int GEOSPolygonRaster_set(GEOSPolygonRaster **polyRaster, int col, int row)
{
   //if (col < 0 || col > (*polyRaster)->width)
   //	warn_printf("raster[%d][%d] - col index out of range\n", row, col)
   // 
   //if (row < 0 || row > (*polyRaster)->height)
   //	warn_printf("raster[%d][%d] - row index out of range\n", row, col)
   
   col = (col < 0) ? 0 : col;
   col = (col > (*polyRaster)->width) ? (*polyRaster)->width : col;
   row = (row < 0) ? 0 : row;
   row = (row > (*polyRaster)->height) ? (*polyRaster)->height : row;  

   (*polyRaster)->raster[row][col] = 1;
   
   return 0;
}
