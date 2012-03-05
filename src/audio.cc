/*******************************************************************
(C) 2010 by Radu Stefan
radu124@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*******************************************************************/

#include "audio.h"

#include "verbosity.h"
#include "message.h"
#include "audiosfx.h"
#include "audiosong.h"

tPlaying playing;
SDL_AudioSpec desired, obtained;
int callbacklen;

tPlaying::tPlaying()
{
	effect=playingeffects;
	afile=&songfile;
}

int pbpos=0;

int min3(int a, int b, int c)
{
	if (a<b && a<c) return a;
	if (b<c) return b;
	return c;
}

int rangelimit(int a)
{
	if (a<-32760) return -32760;
	if (a>32760) return 32760;
	return a;
}

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
	playing.callback((uint16_t *) stream, len/4);
}

void tPlaying::callback(uint16_t *stream, int len)
{
	int i,q;
	int wrappos;
	int en_play;
	int vol_base, vol_rhyt_le, vol_rhyt_rt, vol_gtar_le, vol_gtar_rt;
	int sfxcount;

	playpos=afile[0].playpos;
	wrappos=playpos%AUDIOFILE_BUFFER_SIZE;

	callbacklen=len;
	int decodebtm=afile[0].decodepos;

	// if enough data was decoded after a stall, unstall
	if (stalled && decodebtm>playpos+AUDIOFILE_STARTAFTER)
	{
		playing=1;
		stalled=0;
		DBG(AUDIO,"unstalled\n");
	}

	// if we reached the end of the song, stop playing
	if (playpos>afile[0].stopsat && songstarted)
	{
		playing=0;
		songended=1;
		DBG(AUDIO,"end of song\n");
	}

	// when we don't have enough music data, we stall
	if (playing && decodebtm<playpos+len)
	{
		stalled=1;
		playing=0;
		DBG(AUDIO,"stalled\n");
	}

	en_play=playing && !pause;

	vol_base=256;
	sfxcount=sfx_playbackready();
	DBG(AUDIO,"playing\n");

	for (i=0; i<len; i++)
	{
		int s=0;

		if (en_play)
		{
			s+=vol_base    * afile[0].buffer[0][wrappos];
		}
		for (q=0; q<sfxcount; q++)
		{
			int addv=effect[q].vol_le * effect[q].ptr[0];
			s+=addv;
		}
		stream[i*2]=rangelimit(s/256);

		s=0;

		if (en_play)
		{
			s+=vol_base    * afile[0].buffer[0][wrappos];
		}
		for (q=0; q<sfxcount; q++)
		{
			int addv=effect[q].vol_rt * effect[q].ptr++[0];
			s+=addv;
		}
		stream[i*2+1]=rangelimit(s/256);

		wrappos=(wrappos+1)%AUDIOFILE_BUFFER_SIZE;
		//effects.data[0][pbpos];
		//pbpos=pbpos+1;
		//if (pbpos>=effects.len[0]) pbpos=0;
	}
	for (q=0; q<sfxcount; q++) effect[q].pos+=len;
	if (en_play)
	{
		afile[0].playpos+=len;
	}
}

void init_audio()
{
	desired.freq=AUDIO_PLAYBACKRATE;
	desired.format=AUDIO_S16SYS;
	desired.samples=AUDIOBUF_SIZE;
	desired.channels=2;
	desired.userdata=(void *) &playing;
	desired.callback=my_audio_callback;
	assume(SDL_OpenAudio(&desired, &obtained)>=0, "Couldn't open audio: %s\n");
	SDL_PauseAudio(0);
}


