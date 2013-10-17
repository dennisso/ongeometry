#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <shapefil.h>
#include <geos_c.h>

int main(int argc, char *argv[])
{
   SHPHandle hShp;
   int nShapeType, nEntities, i, iPart, bValidate = 0, nInvalidCount = 0;
   int bHeaderOnly = 0;
   double adfMinBound[4], adfMaxBound[4];
   
   hShp = SHPOpen(argv[1], "rb");

   if (hShp == NULL)
   {
      printf("Unable to open: %s\n", argv[1]);
      goto EXIT; 
   }

   SHPGetInfo(hShp, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

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

   #define BUFSIZE 8192

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

   initGEOS(notice, log_and_exit);
   finishGEOS();

EXIT:
   return 0;
}
