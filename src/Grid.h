#ifndef GRID_H
#define GRID_H

#include <cstdio>
#include "BoundingBox.h"

struct GridLoc {
	long x;
	long y;
};

struct RefLoc {
	float x;
	float y;
};

class Grid {

	public:
		long numCols;
		long numRows;
		BoundingBox extent; 
		float cellSize;

		bool IsSpatialMatch(const Grid *testGrid) {
			return ((numCols == testGrid->numCols) && (numRows == testGrid->numRows)
				&& (extent.left == testGrid->extent.left) && (extent.bottom == testGrid->extent.bottom)
				&& (cellSize == testGrid->cellSize));
		}

		bool GetGridLoc(float lon, float lat, GridLoc *pt) {
			float xDiff = lon - extent.left;
			float yDiff = extent.top - lat;
			float xLoc = xDiff/cellSize;
			float yLoc = yDiff/cellSize; 
			pt->x = (long)xLoc;
			pt->y = (long)yLoc;

			if (pt->x < 0) {
				pt->x = 0;
			} else if (pt->x >= numCols) {
				pt->x = numCols - 1;
			}

			if (pt->y < 0) {
				pt->y = 0;
			} else if (pt->y >= numRows) {
				pt->y = numRows;
			}

			if (extent.left > lon || extent.right < lon || extent.bottom > lat || extent.top < lat) {
                                return false; // This point isn't in the grid!
                        } else {
				return true;
			}	
		}

		bool GetRefLoc(long x, long y, RefLoc *pt) {
			pt->x = (float)x * cellSize + extent.left;
			pt->y = extent.top - (float)y * cellSize;
			return true; 
		}

};

class FloatGrid : public Grid {

	public:
		FloatGrid() {
			data = NULL;
		}
		~FloatGrid() {
			if (data) {
				for (long i = 0; i < numRows; i++) {
					delete [] data[i];
				}
				delete [] data;
			}
		}
		float noData;
		float **data;

};

class LongGrid : public Grid {

        public:
		LongGrid() {
                        data = NULL;
                }
                ~LongGrid() {
                        if (data) {
                                for (long i = 0; i < numRows; i++) {
                                        delete [] data[i];
                                }
				delete [] data;
                        }
                }
		long noData;
                long **data;

};

#endif
