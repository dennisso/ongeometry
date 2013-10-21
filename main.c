#include <geospatial.h>

int main(int argc, char *argv[])
{
   int isError = 0;
   double x, y;
   SHPHandle hShp;

   if (argc < 4)
   {
      printf("Usage: onland <shapefile path> <x|long> <y|lat>\n");
      isError = 1;
      goto EXIT;
   }

   // if it does not return 0, it's an error
   if (shpOpen(&hShp, argv[1]) != 0)
      goto GC;
      
   initGEOS(notice, log_and_exit);   

   x = atof(argv[2]);
   y = atof(argv[3]); 
   
   printf("%d\n", isOnLand(&hShp, x, y));
     
   //printf("GEOS: %s\n", GEOSversion()); 
   finishGEOS();

GC:     
   // destroy shp handle; frees its pointer
   SHPClose(hShp);
EXIT:   
   return (isError != 1) ? -1 : 0;
}
