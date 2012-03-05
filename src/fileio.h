#ifndef __HDR_fileio_h
#define __HDR_fileio_h


#include "includes.h"

/**
 * List directory contents
 * @param dir the name of the directory
 * @param flags - a combination of the following
 *   1 - also list directories
 *   2 - list only files/directories
 *   4 - exclude files starting with . (dot)
 */
vector<string> listDirContents(string dir, int flags=0);

/**
 * determine if a file exists
 * @return 1 if file exists, 0 otherwise
 */
int fileExists(std::string name);

/**
 * load file in a memory region and return a pointer to that region
 * @param name the file name
 * @param data pointer to the region (written)
 * @param len  length of the data (written)
 * @return 0 on failure, a positive file identifier otherwise
 */
int mapFile(string name, const char *&data, int &len);

/**
 * free memory in use by a file
 * @param i a file identifier
 */
void unmapFile(int i);


#endif
