#include "dbg.h"
#include "ppm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Allocates space for an empty ppm of the given size.
 * <b>NOTE: Remember to call freePpm when you are done.</b>
 * @param nrows the number of rows
 * @param ncolumns the number of columns (this is multiplied by 3 inside the function)
 * @return the ppm or NULL if error.
 */
ppm_t makePpm(int nrows, int ncols) {
  // code adapted from http://c-faq.com/aryptr/dynmuldimary.html
  //                   last access 10/23/2013
  int i;
  ppm_t ppm = NULL_PPM;
  pixel_t **array2 = malloc(nrows * sizeof(pixel_t *));

  check_mem(array2);
  array2[0] = malloc(nrows * ncols * sizeof(pixel_t));

  check_mem(array2[0]);

  for(i = 1; i < nrows; i++) {
    array2[i] = array2[0] + i * ncols;
  }
  ppm.rows = nrows;
  ppm.cols = ncols;
  ppm.data = array2;
  return ppm;
 error:
  if (array2) {
    if (array2[0]) {
      free(array2[0]);
    }
    free(array2);
  }
  return NULL_PPM;
} // end makePpm

/**
 * Frees the memory allocated by makePpm and loadPpm.
 * @param ppm the ppm (created/allocated by makePpm or loadPpm).
 */
void freePpm(ppm_t ppm) {
  check(ppm.data, "Attempt to free NULL ppm!");
  free(ppm.data[0]);	
  free(ppm.data);
 error: // do nothing -- required by check
  return;
} // end freePpm

/**
 * Allocates necessary space for and 
 * loads a ppm from the given file.
 * <b>NOTE: Remember to call freePpm when you are done.</b>
 * @param filename the name of the ppm file.
 * @return the data in a 2D array of ints, 
 *          or NULL if error
 */
ppm_t loadPpm(char filename[]) {
  char buffer[BUFSIZ];
  int magicNum = 0;
  int ret = 0;
  int rows = 0;
  int cols = 0;
  int maxVal = 0;
  int red = 0;
  int green = 0;
  int blue = 0;
  int numsRead = 0;
  int r;
  int c;
  ppm_t result = NULL_PPM;

  FILE* fp = NULL;
  check(filename && strlen(filename) > 0, "NULL/empty filename provided to loadPpm");

  fp = fopen(filename, "r");
  check(fp, "Could not open file: %s.", filename);

  // NOTE: the space at end of format string
  //       used to remove any additional whitespace	
  ret = fscanf(fp, "P%u ", &magicNum);
  check(ret > 0, "Error reading magic number in: %s.", filename);
  check(magicNum == 3, "Invalid ppm format, magicNum %d in %s.", magicNum, filename);

  // some ppm files have a # comment line
  // after the magic number
  do {
    fgets(buffer, BUFSIZ, fp);
  } while(strncmp("#", buffer, 1) == 0);

  // read width and height
  ret = sscanf(buffer, "%u %u ", &cols, &rows);
  check(ret == 2 && cols > 0 && rows > 0, "Error reading width and height from %s.", filename);

  ret = fscanf(fp, "%u ", &maxVal);
  check(ret > 0, "Error reading max value from %s.", filename);
  check(maxVal > 0 && maxVal <= 255, "Invalid maxVal: %u in %s.", maxVal, filename);

  // create a (2D) array to store the data
  result = makePpm(rows, cols);
  check(result.data, "Failed to create ppm");

  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++) {
      ret = fscanf(fp, "%d %d %d ", &red, &green, &blue);
      check(ret == 3, "Error reading ppm data at r: %d, c: %d from %s.", r, c, filename);
      result.data[r][c].red = red;
      result.data[r][c].green = green;
      result.data[r][c].blue = blue;

      //log_info("Read [%d, %d] = %d", r, c, num);
      numsRead++;
    }
  }
  check(numsRead == rows * cols, "Error in numsRead: %d should be %d", numsRead, rows * cols);

  fclose(fp); // close the file

  return result;

  // "called" by check macros when an error occurs
 error: 
  // close file
  if (fp != NULL) {
    fclose(fp);
  }
  // free any allocated memory
  if (result.data) {
    freePpm(result);
  }
  return NULL_PPM;
}

/**
 * Saves the ppm to the specified file.
 * @param nrows the number of rows
 * @param ncolumns the number of columns (this is multiplied by 3 inside the function)
 */
void savePpm(char filename[], ppm_t ppm) {
  int r;
  int c;
  FILE *fp;

  check(filename && strlen(filename) > 0, "Attempt to save to NULL/empty filename!");
  fp = fopen(filename, "w");
  check(fp, "Unable to open %s for writing!", filename);

  check(ppm.data, "Attempt to save NULL/empty ppm!");

  // write the magic number
  fputs("P3\n", fp);

  // write the size
  fprintf(fp, "%u %u\n", ppm.cols, ppm.rows);

  // write the max val
  fputs("15\n", fp);

  for (r = 0; r < ppm.rows; r++) {
    for (c = 0; c < ppm.cols; c++) {
      if (c == 0) {
	fprintf(fp, "%2u%3u%3u", ppm.data[r][c].red, ppm.data[r][c].green, ppm.data[r][c].blue);
      }
      else {
	fprintf(fp, "%3u%3u%3u", ppm.data[r][c].red, ppm.data[r][c].green, ppm.data[r][c].blue);
      }
    }
    fputs("\n", fp);
  }

  fclose(fp);
  return;

 error:
  if (fp) {
    fclose(fp);
  }
} // end savePpm

int main() {
  ppm_t ppm = NULL_PPM;
  int r;
  int c;

  ppm = loadPpm("nodata.ppm");
  printf("ppm loaded, rows: %d, cols: %d\n", ppm.rows, ppm.cols);

  //for (r = 0; r < ppm.rows; r++) {
  //	for (c = 0; c < ppm.cols; c++) {
  //		printf("%3d%3d%3d", ppm.data[r][c].red, ppm.data[r][c].green, ppm.data[r][c].blue);
  //	}
  //	puts("");
  //}
  savePpm("copy.ppm", ppm);

  if (ppm.data) {
    freePpm(ppm);
  } 
}
