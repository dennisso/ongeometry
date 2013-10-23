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
 * returns: 0 if it succeeds otherwise -1
 */
int shpOpen(SHPHandle *hShp, char *path)
{
   int isError = 0;
   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];
   
   *hShp = SHPOpen(path, "rb");

   if (*hShp == NULL)
   {
      fprintf(stderr, "Unable to open: %s\n", path);
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
 * shpLoad: load shapefile to memory as polygons
 * return: 0 if successful otherwise -1
 */
int shpLoad(SHPHandle *hShp, GEOSCoordSeq ***linearRingCsList,
   GEOSGeom ***linearRingList, GEOSGeom **polygonList)
{
   int isError = 0;
   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];
   SHPObject *psShape;
   
   SHPGetInfo(*hShp, &nEntities, &nShapeType, adfMinBound, adfMaxBound );
   
   // allocate memory for point indices for 2D jagged array
   *linearRingCsList = malloc(sizeof(GEOSCoordSeq *) * nEntities);
   *linearRingList = malloc(sizeof(GEOSGeom *) * nEntities);
   *polygonList = malloc(sizeof(GEOSGeom) * nEntities);
   
   for (int i = 0; i < nEntities; i++)
   {
      psShape = SHPReadObject(*hShp, i);

      if (psShape == NULL)
      {
         fprintf(stderr, "Unable to read shape %d, stop object reading...\n", i);
         isError = 1;
         goto EXIT;
      }
      
      //printf("Shape:%d (%s) nVertices=%d nParts=%d\n",
      //   i, SHPTypeName(psShape->nSHPType),
      //   psShape->nVertices, psShape->nParts);
      
      // allocate 
      (*linearRingCsList)[i] = malloc(sizeof(GEOSCoordSeq) * psShape->nParts);
      (*linearRingList)[i] = malloc(sizeof(GEOSGeom) * psShape->nParts);

      for (int j = 0; j < psShape->nParts; j++)
      {
         unsigned int startIdx = psShape->panPartStart[j];
         unsigned int endIdx = (j != psShape->nParts - 1) ? psShape->panPartStart[j + 1] : psShape->nVertices; 
         (*linearRingCsList)[i][j] = GEOSCoordSeq_create(endIdx - startIdx, 2);
         
         for (unsigned int k = startIdx; k < endIdx; k++)
         {
            GEOSCoordSeq_setX((*linearRingCsList)[i][j], k - startIdx, psShape->padfX[k]);
            GEOSCoordSeq_setY((*linearRingCsList)[i][j], k - startIdx, psShape->padfY[k]);
         }
         
         (*linearRingList)[i][j] = GEOSGeom_createLinearRing((*linearRingCsList)[i][j]);
      }

      (*polygonList)[i]  = GEOSGeom_createPolygon(
         (*linearRingList)[i][0], &(*linearRingList)[i][1], psShape->nParts - 1);

      if (!GEOSisValid((*polygonList)[i]))
      {
         fprintf(stderr, "Invalid polygon. Shapefile is not valid...\n");
         isError = 1;
         break; 
      }

      //char *wkt_c = GEOSGeomToWKT(polygonList[i]);
      //printf("%s\n", wkt_c);

      // destroy shapefile obj; frees its pointer
      SHPDestroyObject(psShape);
   }

GC:
   //if (psShape)
   //   SHPDestroyObject(psShape);
   
EXIT:
   return (isError != 0) ? -1 : 0;
}

/*
 * shpUnload: Unload shapefile to memory as polygons
 * return: 0 if successful otherwise -1
 */
int shpUnload(SHPHandle *hShp, GEOSCoordSeq ***linearRingCsList,
   GEOSGeom ***linearRingList, GEOSGeom **polygonList)
{
   int isError = 0;
   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];

   SHPGetInfo(*hShp, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

   for (int i = 0; i < nEntities; i++)
   {
      // destroying parent geom also destroys its child geoms & coord seq
      // that's how libgeos works
      if ((*polygonList)[i])
      {
         //printf("freeing polygon[%d]...\n", i);
         GEOSGeom_destroy((*polygonList)[i]);
      }

      free((*linearRingCsList)[i]);
      (*linearRingCsList)[i] = NULL; 
      free((*linearRingList)[i]);
      (*linearRingList)[i] = NULL;
   }

   // free the libgeos pointers
   free(*polygonList);
   *polygonList  = NULL;
   free(*linearRingCsList);
   *linearRingCsList = NULL;
   free(*linearRingList);
   *linearRingList = NULL;

EXIT:
   return (isError != 0) ? -1 : 0;
}

/*
 * isOnLand: determines whether coordinate (x, y) is on land
 * returns: 1 if the coordinate is on land otherwise 0
 */
int isOnLand(SHPHandle *hShp, GEOSGeom **polygonList, double x, double y)
{
   int bIsOnLand = 0;
   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];

   SHPGetInfo(*hShp, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

   GEOSCoordSeq pointCs;
   GEOSGeom point;

   pointCs = GEOSCoordSeq_create(1, 2);
   GEOSCoordSeq_setX(pointCs, 0, x);
   GEOSCoordSeq_setY(pointCs, 0, y); 
   point = GEOSGeom_createPoint(pointCs);

   for (int i = 0; i < nEntities; i++)
   {
      if (bIsOnLand != 0)
         break;
          
      bIsOnLand = GEOSContains((*polygonList)[i], point);
   }  
   
   // destroying parent geom also destroys its child geoms & coord seq
   // that's how libgeos works
   GEOSGeom_destroy(point);

   return bIsOnLand;
}

