#ifndef __HDR_audio_h
#define __HDR_audio_h

#include "includes.h"

const int AUDIOBUF_SIZE=1024;
const int AUDIO_PLAYBACKRATE=44100;

void init_audio();

extern int callbacklen;
extern SDL_AudioSpec desired, obtained;

struct tEffectPlay;
struct tAudioFile;

struct tPlaying
{
	tPlaying();
	tEffectPlay *effect; // initialized to point to playingeffects
	tAudioFile  *afile;  // initialized to point to songfiles

	/**
	 * song pause flag
	 */
	volatile int pause;
	/**
	 * Flag that indicates a song buffer underrun
	 */
	volatile int stalled;
	/**
	 * Flag that indicates whether a song has ended
	 * set to 0 by decoder thread in a critical section
	 * set to 1 by audio thread when song finishes
	 */
	volatile int songended;
	/**
	 * Flag that indicates whether a song is ready to start playing
	 * (meaning that all components of the song have been initialized)
	 * set by decoder thread - set to 0 in the critical section where
	 * the song playpos and decodepos are also reset
	 */
	volatile int songstarted;

	int playpos;
	int playing;

	void callback(uint16_t *stream, int len);
};

extern tPlaying playing;

#endif
