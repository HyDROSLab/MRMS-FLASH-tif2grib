#!/bin/csh

g++ -O3 -I/usr/include/libgeotiff -lz -ltiff -lgeotiff -lpng -I../g2clib-1.4.0 Main.cpp TifGrid.cpp ../g2clib-1.4.0/libgrib2c.a -o tif2grib
g++ -O3 -I/usr/include/libgeotiff -lz -ltiff -lgeotiff -lpng -I../g2clib-1.4.0 MainMask.cpp TifGrid.cpp ../g2clib-1.4.0/libgrib2c.a -o tif2gribmask
