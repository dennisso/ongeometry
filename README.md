OnGeometry
=========

Functionalities
---------------
- rasterizes geometries (vectors)
- check if a point is on the geometry

Prerequisites
-------------
- [libgeos (libgeos_c)](http://geos.osgeo.org)
- [shapelib (libshp)](http://shapelib.maptools.org)
- shapefile which contains polygons that represent the landmass

Compliation
------------
make

or

make CFLAGS+=-DDEBUG to debug

Use
---
./bin/OnGeometry <shapefile path> <x||longitude> <y||latitude>


