OnLand
======

Objective
---------
Check if a coordinate is on land by reading a shapefile which contain the
landmass as polygons and checks if it is on land or on water

Prerequisites
-------------
- [libgeos (libgeos_c)](http://geos.osgeo.org)
- [shapelib (libshp)](http://shapelib.maptools.org)
- shapefile which contains polygons that represent the landmass

Installation
------------
make

Use
---
./onland <shapefile path> <x||longitude> <y||latitude>

Example
-------
Input
> ./onland data/coastline_and_islands_na_gcs_wgs_1984.shp -81 41.6
Output
> 0

Input
> ./onland data/coastline_and_islands_na_gcs_wgs_1984.shp -80 43
Output
> 1

