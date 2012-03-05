#ifndef __HDR_audiosfx_h
#define __HDR_audiosfx_h

#include "includes.h"

/**
 * Load special effect
 * @param filename name of ogg file storing that effect name
 * @param eid effect id to be overwritten, 0 for new effect
 * @return effect id on success, 0 otherwise
 *
 * Warning: it is not safe to overwrite an effect while that
 * effect is being played
 */
int sfx_load(string filename, int eid=0);

/**
 * Enqueue sound effect to be played as soon as possible
 * @param effnum effect number
 * @param channel playback channel 0=center 1=left 2=right
 * @param volume effect volume 256=0db change
 */
void sfx_start(int effnum, int volume_left=128, int volume_right=128);

/**
 * Request a stop to all effects on the next sound event
 */
void sfx_stop_all();

/**
 * Prepare effects to be played in the current sound event
 * @return the number of effects that should play
 */
int sfx_playbackready();

struct tEffectList
{
	int count;
	int16_t *data[256];
	int len[256];
	tEffectList():count(0) { int i; for (i=0; i<256; i++) {data[i]=NULL; len[i]=0; } }
};

/**
 *  we can request up to this many effects per playback period
 */
const int EFFECTRQUEUESIZE=4;

/**
 * the engine will play at most this many effects at the same time
 */
const int EFFECTSPLAY=6;

/**
 * Playing effect, contains parameters and current state
 */
struct tEffectPlay
{
	int id;
	int pos;
	int16_t *ptr;
	int vol_le, vol_rt;
	int age;
};

extern tEffectPlay playingeffects[EFFECTSPLAY];
extern tEffectList effectdata;
extern int sfx_agecounter;


#endif
