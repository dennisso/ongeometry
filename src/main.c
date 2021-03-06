#include <geospatial.h>

int main(int argc, char *argv[])
{
   int isError = 0, result = 0;
   double x, y;
   SHPHandle hShp;
  
   if (argc < 4)
   {
      info_printf("Usage: onland <shapefile path> <x|long> <y|lat>\n")
      isError = 1;
      goto EXIT;
   }

   // if it does not return 0, it's an error
   if (shpOpen(&hShp, argv[1]) != 0)
      goto GC_SHP;
      
   initGEOS(notice, log_and_exit);   

   GEOSPolygons *polygons;

   debug_printf("loading shapefile to memory...\n")
   result = shpLoad(&hShp, &polygons); 
   if (result != 0)
   {  
      error_printf("loading failed...\n")
      goto GC_GEOS;
   }
   
   x = atof(argv[2]);
   y = atof(argv[3]); 
   
   info_printf("%d\n", isOnPolygons(&polygons, x, y))

GC_GEOS:
   debug_printf("unloading shapefile to memory...\n")
   shpUnload(&polygons);

	debug_printf("GEOS: %s\n", GEOSversion()) 
   
	finishGEOS();

GC_SHP:
   // destroy shp handle; frees its pointer
   SHPClose(hShp);

EXIT:   
   return (isError != 1) ? -1 : 0;
}
