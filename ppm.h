#ifndef __ppm_h__
#define __ppm_h__

typedef struct pixel_t {
	int red;
	int green;
	int blue;
} pixel_t;

typedef struct ppm_t {
  int rows;
  int cols;
  pixel_t **data;
} ppm_t;

const ppm_t NULL_PPM = { -1, -1, NULL };

ppm_t makePpm(int nrows, int ncolumns);
void freePpm(ppm_t ppm);
ppm_t loadPpm(char filename[]);
void savePpm(char filename[], ppm_t ppm);

#endif