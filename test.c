#include <geospatial.h>
#include <minunit.h>
#include <time.h>

int tests_run = 0;

static char * test_isOnPolygons(GEOSPolygons **polygons)
{
   mu_assert("error: (-96.82341, 58.137457) should be 1\n", isOnPolygons(polygons, -96.82341, 58.137457));
   mu_assert("error: (-109.26176, 70.585724) should be 1\n", isOnPolygons(polygons, -109.26176, 70.585724));
   mu_assert("error: (-130.211786, 68.559262) should be 1\n", isOnPolygons(polygons, -130.211786, 68.559262));
   mu_assert("error: (-47.609331, 65.085327) should be 1\n", isOnPolygons(polygons, -47.609331, 65.085327));
   mu_assert("error: (-56.19767, 48.487637) should be 1\n", isOnPolygons(polygons, -56.19767, 48.487637));
   mu_assert("error: (-79.646732, 37.969334) should be 1\n", isOnPolygons(polygons, -79.646732, 37.969334));

   return 0;
}

static char * test_not_isOnPolygons(GEOSPolygons **polygons)
{
   mu_assert("error: (-86.305107, 59.391933) should be 0\n", !isOnPolygons(polygons, -86.305107, 59.391933));
   mu_assert("error: (-59.575107, 59.58493) should be 0\n", !isOnPolygons(polygons, -59.575107, 59.58493));
   mu_assert("error: (-91.51601, 71.45420) should be 0\n", !isOnPolygons(polygons, -91.51601, 71.454207));
   mu_assert("error: (-133.492724, 72.998179) should be 0\n", !isOnPolygons(polygons, -133.492724, 72.998179));
   mu_assert("error: (-50.986768, 57.654966) should be 0\n", !isOnPolygons(polygons, -50.986768, 57.654966));
   mu_assert("error: (-57.452147, 44.338215) should be 0\n", !isOnPolygons(polygons, -57.452147, 44.338215));
   mu_assert("error: (-86.787598, 42.890742) should be 0\n", !isOnPolygons(polygons, -86.787598, 42.890742));
   mu_assert("error: (-71.251389, 36.135868) should be 0\n", !isOnPolygons(polygons, -71.251389, 36.135868));

   return 0;
}

static char * test_million_onland(GEOSPolygons **polygons)
{
   time_t startTime, endTime;
   double seconds;

   time(&startTime);
   for (int i = 0; i < 1000; i++)
   {
      for (int j = 0; j < 1000; j++)
      {
         isOnPolygons(polygons, i, j);  
      }
   }
   time(&endTime);
   seconds = difftime(endTime, startTime);
   printf("%.f seconds elapsed over 1m calls\n", seconds);
   
   return 0;
}

static char * all_tests(GEOSPolygons **polygons)
{
   mu_run_test(test_isOnPolygons(polygons));
   mu_run_test(test_not_isOnPolygons(polygons));
   mu_run_test(test_million_onland(polygons));
   return 0;
}

int main(int argc, char *argv[])
{
   SHPHandle hShp;
   
   if (argc < 2)
   {
      printf("Usage: test <shapefile path>\n");
      goto EXIT;
   }

   // if it does not return 0, it's an error
   if (shpOpen(&hShp, argv[1]) != 0)
   {
      printf("error: cannot open shapefile at %s\n", argv[1]);
      goto GC_SHP;
   }
      
   initGEOS(notice, log_and_exit);   
   
   GEOSPolygons *polygons;
    
   printf("loading...\n");
   if(shpLoad(&hShp, &polygons) != 0)
   {
      printf("error: cannot load shapefile at %s\n", argv[1]);
      goto GC_GEOS;
   }

   char *result = all_tests(&polygons);

   if (result != 0)
      printf("%s\n", result);
   else
      printf("Tests pass\n");

   printf("Tests run: %d\n", tests_run);

GC_GEOS:
   printf("unloading...\n");
   shpUnload(&polygons);

   //printf("GEOS: %s\n", GEOSversion()); 
   finishGEOS();

GC_SHP:
   // destroy shp handle; frees its pointer
   SHPClose(hShp);

EXIT:
   return (result != 0) ? -1 : 0;
}
