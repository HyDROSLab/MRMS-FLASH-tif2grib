#!/bin/csh

g++ -O3 -I/usr/include/libgeotiff -lz -ltiff -lgeotiff -lpng -I../g2clib-1.4.0 Main.cpp TifGrid.cpp ../g2clib-1.4.0/libgrib2c.a -o tif2grib
g++ -O3 -g -I/usr/include/libgeotiff -lz -ltiff -lgeotiff -lpng -I../g2clib-1.4.0 MainMask.cpp TifGrid.cpp ../g2clib-1.4.0/libgrib2c.a -o tif2gribmask
g++ -O3 -I/usr/include/libgeotiff -lz -ltiff -lgeotiff -lpng -I../g2clib-1.4.0 -fopenmp MainMRMS.cpp TifGrid.cpp MRMSGrid.cpp ../g2clib-1.4.0/libgrib2c.a -o mrms2gribc
g++ -O3 -I/usr/include/libgeotiff -lz -ltiff -lgeotiff -lpng -I../g2clib-1.4.0 -fopenmp MainMRMSPNG.cpp TifGrid.cpp MRMSGrid.cpp ../g2clib-1.4.0/libgrib2c.a -o mrms2gribpng
