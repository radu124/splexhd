#ifndef __HDR_audiosong_h
#define __HDR_audiosong_h


#include "audiofile.h"

int song_start(string path, int paused=0);
void song_stop();

struct tSongCtrl
{
	volatile int refreshsong;
	volatile int loopsong;    // TODO
	volatile int startpaused;

	/** last time when buffers were filled */
	int decode_lasttime;

	int decode_ticks;

	char songname[2048];
	/**
	 * Close song files and disable playback
	 */
	void close_song();
	/**
	 * open song with the given songname
	 * assumes the previous song has already been stopped
	 *
	 */
	void open_song();
	/**
	 * estimate the amount of decoding necessary and ask songs
	 * to fill their buffers
	 */
	void some_predecode();
};

extern tSongCtrl songctrl;
extern tAudioFile songfile;

void audio_yield();


#endif
