#include <geospatial.h>
#include <minunit.h>

int tests_run = 0;

static char * test_onland(SHPHandle *hShp, GEOSGeom **polygonList)
{
   mu_assert("error: (x, y) should be 1\n", isOnLand(hShp, polygonList, -96.82341, 58.137457));
   mu_assert("error: (x, y) should be 1\n", isOnLand(hShp, polygonList, -109.26176, 70.585724));
   mu_assert("error: (x, y) should be 1\n", isOnLand(hShp, polygonList, -130.211786, 68.559262));
   mu_assert("error: (x, y) should be 1\n", isOnLand(hShp, polygonList, -47.609331, 65.085327));
   mu_assert("error: (x, y) should be 1\n", isOnLand(hShp, polygonList, -56.19767, 48.487637));
   mu_assert("error: (x, y) should be 1\n", isOnLand(hShp, polygonList, -79.646732, 37.969334));

   return 0;
}

static char * test_not_onland(SHPHandle *hShp, GEOSGeom **polygonList)
{
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -86.305107, 59.391933));
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -59.575107, 59.58493));
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -91.51601, 71.454207));
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -133.492724, 72.998179));
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -50.986768, 57.654966));
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -57.452147, 44.338215));
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -86.787598, 42.890742));
   mu_assert("error: (x, y) should be 0\n", !isOnLand(hShp, polygonList, -71.251389, 36.135868));

   return 0;
}


static char * all_tests(SHPHandle *hShp, GEOSGeom **polygonList)
{
   mu_run_test(test_onland(hShp, polygonList));
   mu_run_test(test_not_onland(hShp, polygonList));
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
   
   GEOSCoordSeq **linearRingCsList;
   GEOSGeom **linearRingList, *polygonList;

   printf("loading...\n");
   if(shpLoad(&hShp, &linearRingCsList, &linearRingList, &polygonList) != 0)
   {
      printf("error: cannot load shapefile at %s\n", argv[1]);
      goto GC_GEOS;
   }

   char *result = all_tests(&hShp, &polygonList);

   if (result != 0)
      printf("%s\n", result);
   else
      printf("Tests pass\n");

   printf("Tests run: %d\n", tests_run);

GC_GEOS:
   printf("unloading...\n");
   shpUnload(&hShp, &linearRingCsList, &linearRingList, &polygonList);

   //printf("GEOS: %s\n", GEOSversion()); 
   finishGEOS();

GC_SHP:
   // destroy shp handle; frees its pointer
   SHPClose(hShp);

EXIT:
   return (result != 0) ? -1 : 0;
}
