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
 * GEOSPolygons_create: initializes and allocates GEOM polygons
 * return: a malloced polygon if successful otherwise NULL
 */
GEOSPolygons * GEOSPolygons_create(int nEntities)
{
   GEOSPolygons *polygons;

   if ((polygons = malloc(sizeof(GEOSPolygons))) &&
      (polygons->linearRingCsList = malloc(sizeof(GEOSCoordSeq *) * nEntities)) &&
      (polygons->linearRingList = malloc(sizeof(GEOSGeom *) * nEntities)) &&
      (polygons->polygonList = malloc(sizeof(GEOSGeom) * nEntities))
      )
   {
      polygons->numEntities = nEntities;
   }
   return polygons;
}

/*
 * GEOSPolygons_destroy: frees GEOMS polygons and its pointer
 * return: 0 if successful otherwise -1
 */
int GEOSPolygons_destroy(GEOSPolygons **polygons)
{
   if (*polygons == NULL)
   {
      for (int i = 0; i < (*polygons)->numEntities; i++)
      {
         if ((*polygons)->polygonList[i] != NULL)
         {
            // destroying the parent GEOSGeom also destroys the GEOSGeoms it owns
            GEOSGeom_destroy((*polygons)->polygonList[i]);
            
            free((*polygons)->polygonList[i]);
            (*polygons)->polygonList[i] = NULL;
            free((*polygons)->linearRingList[i]);
            (*polygons)->linearRingList[i] = NULL;
            free((*polygons)->linearRingCsList[i]);
            (*polygons)->linearRingCsList[i] = NULL;
         }
      }

      free((*polygons)->linearRingList);
      (*polygons)->linearRingList = NULL;
      free((*polygons)->linearRingCsList);
      (*polygons)->linearRingCsList = NULL;
      free((*polygons)->polygonList);
      (*polygons)->polygonList = NULL;
      
      free(*polygons);
      *polygons = NULL;  
   }

   return 0;
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
int shpLoad(SHPHandle *hShp, GEOSPolygons **polygons)
{
   int isError = 0;
   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];
   SHPObject *psShape;
   
   SHPGetInfo(*hShp, &nEntities, &nShapeType, adfMinBound, adfMaxBound );
   
   // allocate memory for polygons
   *polygons = GEOSPolygons_create(nEntities);
  
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
      (*polygons)->linearRingCsList[i] = malloc(sizeof(GEOSCoordSeq) * psShape->nParts);
      (*polygons)->linearRingList[i] = malloc(sizeof(GEOSGeom) * psShape->nParts);

      for (int j = 0; j < psShape->nParts; j++)
      {
         unsigned int startIdx = psShape->panPartStart[j];
         unsigned int endIdx = (j != psShape->nParts - 1) ? psShape->panPartStart[j + 1] : psShape->nVertices; 
         (*polygons)->linearRingCsList[i][j] = GEOSCoordSeq_create(endIdx - startIdx, 2);
         
         for (unsigned int k = startIdx; k < endIdx; k++)
         {
            GEOSCoordSeq_setX((*polygons)->linearRingCsList[i][j], k - startIdx, psShape->padfX[k]);
            GEOSCoordSeq_setY((*polygons)->linearRingCsList[i][j], k - startIdx, psShape->padfY[k]);
         }
         
         (*polygons)->linearRingList[i][j] = GEOSGeom_createLinearRing((*polygons)->linearRingCsList[i][j]);
         
         //char *wkt_c = GEOSGeomToWKT((*polygons)->linearRingList[i][j]);
         //printf("[%d][%d]: %s\n", i, j, wkt_c);
      }

      (*polygons)->polygonList[i]  = GEOSGeom_createPolygon(
         (*polygons)->linearRingList[i][0], &((*polygons)->linearRingList[i][1]), psShape->nParts - 1);

      int bufferCount = 0;

      while (!GEOSisValid((*polygons)->polygonList[i]))
      {
         if (bufferCount > 10)
         {
            fprintf(stderr, "ERROR: polygon cannot be be saved by buffering; shapefile is not valid...\n");
            fprintf(stderr, "%s\n", GEOSisValidReason((*polygons)->polygonList[i]));
            isError = 1;
            goto EXIT;
         }
         
         printf("NOTICE: buffer invalid polygon using a width=0.0 and quad_seg=8; iteration: %d\n", bufferCount + 1);
         (*polygons)->polygonList[i] = GEOSBuffer((*polygons)->polygonList[i], 0.0, 8);
      }

      //char *wkt_c = GEOSGeomToWKT((*polygons)->polygonList[i]);
      //printf("%s\n", wkt_c);

      // destroy shapefile obj; frees its pointer
      SHPDestroyObject(psShape);
   }

EXIT:
   return (isError != 0) ? -1 : 0;
}

/*
 * shpUnload: Unload shapefile to memory as polygons
 * return: 0 if successful otherwise -1
 */
int shpUnload(GEOSPolygons **polygons)
{
   int isError = 0;
  
   GEOSPolygons_destroy(polygons);
   
   return (isError != 0) ? -1 : 0;
}

/*
 * isOnPolygons: determines whether coordinate (x, y) is on land
 * returns: 1 if the coordinate is on land otherwise 0
 */
int isOnPolygons(GEOSPolygons **polygons, double x, double y)
{
   int bIsOnLand = 0;
   
   GEOSCoordSeq pointCs;
   GEOSGeom point;

   pointCs = GEOSCoordSeq_create(1, 2);
   GEOSCoordSeq_setX(pointCs, 0, x);
   GEOSCoordSeq_setY(pointCs, 0, y); 
   point = GEOSGeom_createPoint(pointCs);
   
   for (int i = 0; i < (*polygons)->numEntities; i++)
   {
      if (bIsOnLand != 0)
         break;
      
      bIsOnLand = !GEOSContains((*polygons)->polygonList[i], point);
   }  
   
   // destroying parent geom also destroys its child geoms & coord seq
   // that's how libgeos works
   GEOSGeom_destroy(point);

   return bIsOnLand;
}

