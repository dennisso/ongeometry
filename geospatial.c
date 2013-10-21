#include <geospatial.h>

void notice(const char *fmt, ...)
{
   va_list ap;

   fprintf(stdout, "NOTICE: ");
   va_start(ap, fmt);
   vfprintf(stdout,fmt, ap);
   va_end(ap);
   fprintf(stdout, "\n");
}

void log_and_exit(const char *fmt, ...)
{
   va_list ap;

   fprintf(stdout, "ERROR: ");
   vfprintf(stdout, fmt, ap);
   va_end(ap);
   fprintf(stdout, "\n");
   exit(1);
}

/*
 * shpOpen: read shapefile using shapelib
 * returns: 0 if it succeeds
 */
int shpOpen(SHPHandle *hShp, char *path)
{
   int isError = 0;
   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];
   
   *hShp = SHPOpen(path, "rb");

   if (*hShp == NULL)
   {
      printf("Unable to open: %s\n", path);
      isError = 1;
      goto EXIT; 
   }

   SHPGetInfo(*hShp, &nEntities, &nShapeType, adfMinBound, adfMaxBound );
   
   /*
   printf("Shapefile Type: %s   # of Shapes: %d\n\n",
      SHPTypeName(nShapeType), nEntities);
    
   printf("File Bounds: (%.15g,%.15g,%.15g,%.15g)\n"
      "         to  (%.15g,%.15g,%.15g,%.15g)\n",
      adfMinBound[0], 
      adfMinBound[1], 
      adfMinBound[2], 
      adfMinBound[3], 
      adfMaxBound[0], 
      adfMaxBound[1], 
      adfMaxBound[2], 
      adfMaxBound[3]);
   */
EXIT:
   return (isError != 0) ? -1 : 0;
}

/*
 * isOnLand: determines whether coordinate (x, y) is on land
 * returns: 1 if the coordinate is on land otherwise 0
 */
int isOnLand(SHPHandle *hShp, double x, double y)
{
   int bIsOnLand = 0;
   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];

   SHPGetInfo(*hShp, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

   for (int i = 1; i < nEntities; i++)
   {
      if (bIsOnLand != 0)
         break;

      SHPObject *psShape;
      psShape = SHPReadObject(*hShp, i);

      if (psShape == NULL)
      {
         fprintf(stderr, "Unable to read shape %d, stop object reading...\n", i);
         break;
      }

      //printf("Shape:%d (%s) nVertices=%d nParts=%d\n",
      //   i, SHPTypeName(psShape->nSHPType),
      //   psShape->nVertices, psShape->nParts);

      GEOSCoordSeq **polygonCsList, pointCs;
      polygonCsList = (GEOSCoordSeq **) malloc(sizeof(GEOSCoordSeq) * psShape->nParts);
      
      for (int j = 0; j < psShape->nParts; j++)
      {
         unsigned int startIdx = psShape->panPartStart[j];
         unsigned int endIdx = (j != psShape->nParts - 1) ? psShape->panPartStart[j + 1] : psShape->nVertices; 
         polygonCsList[j] = (GEOSCoordSeq *) GEOSCoordSeq_create(endIdx - startIdx, 2);
         
         for (unsigned int k = startIdx; k < endIdx; k++)
         {
            GEOSCoordSeq_setX((GEOSCoordSeq) polygonCsList[j], k - startIdx, psShape->padfX[k]);
            GEOSCoordSeq_setY((GEOSCoordSeq) polygonCsList[j], k - startIdx, psShape->padfY[k]);
         }      
      }
      
      GEOSGeom **linearRingList, polygon, point;
      linearRingList = (GEOSGeom **) malloc(sizeof(GEOSGeom) * psShape->nParts);
      
      for (int j = 0; j < psShape->nParts; j++)
      {
         linearRingList[j] = (GEOSGeom *) GEOSGeom_createLinearRing((GEOSCoordSeq) polygonCsList[j]);
      }

      polygon = GEOSGeom_createPolygon(
         (GEOSGeom) linearRingList[0], (GEOSGeom *) &linearRingList[1], psShape->nParts - 1);
      
      //char *wkt_c = GEOSGeomToWKT(polygon);
      //printf("%s\n", wkt_c);
     
      pointCs = GEOSCoordSeq_create(1, 2);
      GEOSCoordSeq_setX(pointCs, 0, x);
      GEOSCoordSeq_setY(pointCs, 0, y); 
      point = GEOSGeom_createPoint(pointCs);
     
      bIsOnLand = GEOSContains(polygon, point);
      
      // destroying parent geom also destroys its child geoms & coord seq
      // that's how libgeos works
      GEOSGeom_destroy(polygon);
      GEOSGeom_destroy(point);
       
      // free the libgeos pointers
      free(polygonCsList);
      free(linearRingList);

      // destroy shapefile obj; frees its pointer
      SHPDestroyObject(psShape);
   }  
   
   return bIsOnLand;
}
