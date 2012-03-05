#ifndef __HDR_audiofile_h
#define __HDR_audiofile_h

#include "includes.h"

/**
 * number of samples in the audio file wrap-around buffer
 * a few seconds' worth of data is recommended
 */
const int AUDIOFILE_BUFFER_SIZE=262144;

/// the amount of data to keep in the buffers for whammy
const int AUDIOFILE_MARGIN=16384;

/// the minimum amount that should be found in the buffer to (re)start playing
const int AUDIOFILE_STARTAFTER=44100; 

/**
 * Music file, it is read dynamically from a file and decoded.
 * the decoded data is placed in a circular buffer implemented as a
 * thread-safe consumer-producer fifo
 */
struct tAudioFile
{
	volatile int playpos;
	volatile int decodepos;
	volatile int isopen;
	volatile int stopsat;
	int reducedstereo;
	int ogg_bs;
	OggVorbis_File oggfile;
	int16_t buffer[2][AUDIOFILE_BUFFER_SIZE];
	/**
	 * open filename and stream it into the buffer
	 * @param filename name of ogg file
	 */
	int open(const char *filename);
	/**
	 * decode samples from file to the buffer
	 * @param target number of samples
	 * 
	 * Warning: the function will decode fewer samples if
	 * there is not enough space in the buffer
	 */
	void decode(int target);
	/**
	 * reset buffer pointers to point at beginning of song
	 */
	void reset();
	/**
	 * close associated file and produce only zeros in the buffers
	 */
	void close();
};


#endif
