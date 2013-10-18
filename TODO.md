TODO
====

https://svn.osgeo.org/geos/branches/3.4/tests/thread/threadtest.c
http://trac.osgeo.org/geos/browser/trunk/tests/geostest/geostest.c
https://svn.osgeo.org/geos/branches/3.4/tests/unit/capi/GEOSCoordSeqTest.cpp

helpful fcns to use later
-------------------------
extern char GEOS_DLL GEOSContains(const GEOSGeometry* g1, const GEOSGeometry* g2);

case GEOS_POLYGON:
   gtmp = GEOSGetExteriorRing_r(handle, g1);
   cs = GEOSCoordSeq_clone_r(handle, GEOSGeom_getCoordSeq_r(handle, gtmp));
   shell = GEOSGeom_createLinearRing_r(handle, cs);
   ngeoms = GEOSGetNumInteriorRings_r(handle, g1);
   geoms = malloc(ngeoms*sizeof(GEOSGeometry*));
   for (i=0; i<ngeoms; i++)
   {
      gtmp = GEOSGetInteriorRingN_r(handle, g1, i);
      cs = GEOSCoordSeq_clone_r(handle, GEOSGeom_getCoordSeq_r(handle, gtmp));
      geoms[i] = GEOSGeom_createLinearRing_r(handle, cs);
   }
   g2 = GEOSGeom_createPolygon_r(handle, shell, geoms, ngeoms);
   free(geoms);
   return g2;
   break;


cs_ = GEOSCoordSeq_create(5, 3);
unsigned int size;
unsigned int dims;

ensure ( 0 != GEOSCoordSeq_getSize(cs_, &size) );
ensure_equals( size, 5u );

ensure ( 0 != GEOSCoordSeq_getDimensions(cs_, &dims) );
ensure_equals( dims, 3u );

for (unsigned int i=0; i<5; ++i)
{
   double x = i*10;
   double y = i*10+1;
   double z = i*10+2;

   GEOSCoordSeq_setX(cs_, i, x);
   GEOSCoordSeq_setY(cs_, i, y);
   GEOSCoordSeq_setZ(cs_, i, z);

   double xcheck, ycheck, zcheck;
   ensure( 0 != GEOSCoordSeq_getX(cs_, i, &xcheck) );
   ensure( 0 != GEOSCoordSeq_getY(cs_, i, &ycheck) );
   ensure( 0 != GEOSCoordSeq_getZ(cs_, i, &zcheck) );

   ensure_equals( xcheck, x );
   ensure_equals( ycheck, y );
   ensure_equals( zcheck, z );
}
