/*
 *
 * Tif2Grib Program
 * By Zac Flamig
 * June 2014
 *
 */

#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <math.h>
#include <time.h>
#include <zlib.h>
#include "Grid.h"
#include "TifGrid.h"
#include "grib2.h"

int main(int argc, char *argv[]) {

	if (argc < 6) {
		printf("Usage ./tif2gribmask InputTifName TimeString OutputGribName ParameterID MaskFile\nTimeString as YYYYMMDD.HHMM\n");
		return 1;
	}

	FloatGrid *data = ReadFloatTifGrid(argv[1]);

	if (!data) {
                printf("Unable to open input tif file %s!\n", argv[1]);
                return 1;
        }

	data->extent.top -= 0.005;
        data->extent.bottom += 0.005;
        data->extent.left += 0.005;
        data->extent.right -= 0.005;

	FloatGrid *mask = ReadFloatTifGrid(argv[5]);

        if (!mask) {
                printf("Unable to open mask tif file %s!\n", argv[5]);
                return 1;
        }

	// Read in the time string
	struct tm timeDate;
	strptime(argv[2], "%Y%m%d.%H%M00", &timeDate);

	g2int productID = atoi(argv[4]);
	
	unsigned char *buffer = new unsigned char[1024*1024*50];
	g2int sec0[2], sec1[13];
	
	sec0[0] = 209; // Discipline 0 = meteorology, 1 = hydrology
	sec0[1] = 2; // GRIB Edition Number
	
	sec1[0] = 161; // Center
	sec1[1] = 0; // Sub-Center
	sec1[2] = 255;
        sec1[3] = 1;
        sec1[4] = 3;
        sec1[5] = timeDate.tm_year + 1900; // Year
        sec1[6] = timeDate.tm_mon + 1; // Month
        sec1[7] = timeDate.tm_mday; // Day
        sec1[8] = timeDate.tm_hour; // Hour
        sec1[9] = timeDate.tm_min; // Minute
        sec1[10] = 0;
        sec1[11] = 2;
        sec1[12] = 0;

	long length = g2_create(buffer, sec0, sec1);

	g2int griddef0[5];
	griddef0[0] = 0; // source of definition
	griddef0[1] = data->numCols * data->numRows; // num grid points nx * ny
	griddef0[2] = 0; // num octets
	griddef0[3] = 0; // interpretation
	griddef0[4] = 0; // grid template

	float scaleFactorRes = 1000000;

	g2int griddef[19];
	griddef[0] = 2;
	griddef[1] = 1;
	griddef[2] = 6367470;
	griddef[3] = 1;
	griddef[4] = 6378160;
	griddef[5] = 1;
	griddef[6] = 6356775;
	griddef[7] = data->numCols; // nx
	griddef[8] = data->numRows; // ny
	griddef[9] = 1;
	griddef[10] = scaleFactorRes; // base scale factor
	griddef[11] = data->extent.top * griddef[10]; //ul lat
	griddef[12] = (data->extent.left + 360.0) * griddef[10]; //ul lon
	griddef[13] = 48;
	griddef[14] = data->extent.bottom * griddef[10]; // lr lat
	griddef[15] = (data->extent.right + 360.0) * griddef[10]; // lr lon
	griddef[16] = data->cellSize * scaleFactorRes; // x res
	griddef[17] = data->cellSize * scaleFactorRes; // y res
	griddef[18] = 0;

	g2int proddef[15];
	proddef[0] = 12; //16; // Category; 16 = Future radar imagery, 0 = hydrology basic products 
	proddef[1] = productID; // Product id
	proddef[2] = 8; // observation
	proddef[3] = 0; // bleh
	proddef[4] = 97; // bleh
	proddef[5] = 0; // hours 
	proddef[6] = 0; // minutes
	proddef[7] = 0; // units 
	proddef[8] = 0; // forecast time
	proddef[9] = 1; // first surface, // 102
	proddef[10] = 1;
	proddef[11] = 0;
	proddef[12] = 255; // second surface
	proddef[13] = 1;
	proddef[14] = 0; 

  g2int datadef[16];
  datadef[0] = 0; // Ref Value
  datadef[1] = 0; // Binary scale factor
  datadef[2] = 1; // decimal scale value
  datadef[3] = 7; // number of bits
  datadef[4] = 0; // Type of original field values (0 = float)
  datadef[5] = 1; // Group Splitting method
  datadef[6] = 1; // Missing value management
  datadef[7] = 3296313344; // -999.0 Primary missing value http://www.h-schmidt.net/FloatConverter/IEEE754.html
  datadef[8] = -1; // Secondary missing value
  datadef[9] = 25786; // number of groups
  datadef[10] = 0; // reference for group widths
  datadef[11] = 4; // number of bits for group widths
  datadef[12] = 1; // reference for group lengths
  datadef[13] = 1; // length increment for group lengths
  datadef[14] = 6135; // true length of last group
  datadef[15] = 13; // number of bits for scaled group lengths	
	
	length = g2_addgrid(buffer, griddef0, griddef, NULL, 0);

	g2float *gridData = new g2float[data->numCols * data->numRows];
	for (int i = 0; i < data->numRows; i++) {
		for (int j = 0; j < data->numCols; j++) {
			if (mask->data[i][j] > 0 && data->data[i][j] != data->noData && std::isfinite(data->data[i][j])) {
				float val = floorf(data->data[i][j] * 10.0 + 0.5f) / 10.0f;
				gridData[i * data->numCols + j] = val;
			} else {
				gridData[i * data->numCols + j] = -999.0;
			}
		}
	}

	length = g2_addfield(buffer, 0, proddef, NULL, 0, 2, datadef, gridData, data->numCols * data->numRows, 255, NULL);
	length = g2_gribend(buffer);
	
	printf("Length is now2 %i\n", length);
	
	/*gzFile fp = gzopen(argv[3],"w9");
	if (!fp) {
                printf("Unable to open output grib file %s!\n", argv[3]);
                return 1;
        }
	gzwrite(fp, buffer, length);
	gzclose(fp);*/

	FILE *fp = fopen(argv[3], "w");
        if (!fp) {
                printf("Unable to open output grib file %s!\n", argv[3]);
                return 1;
        }
        fwrite(buffer, 1, length, fp);
        fclose(fp);

	return 0;
}
