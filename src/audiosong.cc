/*******************************************************************
(C) 2011 by Radu Stefan
radu124@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*******************************************************************/

#include "audiosong.h"
#include "audio.h"
#include "message.h"
#include "verbosity.h"

tSongCtrl songctrl;
tAudioFile songfile;

int song_start(string path, int paused)
{
	int len=path.length();
	if (len>2047) len=2047;
	songctrl.refreshsong=0;
	songctrl.startpaused=paused;
	INFO(AUDIO,"Song start: >%s<\n", path);
	strncpy(songctrl.songname,path.c_str(),len);
	songctrl.songname[len]=0;
	songctrl.songname[2048]=0;
	songctrl.refreshsong=1;
	// TODO: need some thread protection here, in case we read this string at the same time
	// not a problem until we move audio decode to another thread
}

void song_stop()
{
	songctrl.songname[0]=0;
	songctrl.refreshsong=1;
}

void tSongCtrl::close_song()
{
	// critical section
	// this is needed because we are resetting the audiofile buffer pointers
	int i;
	songfile.close();
	SDL_LockAudio();
	songfile.reset();
	playing.songstarted=0;
	playing.songended=0;
	playing.stalled=1;
	SDL_UnlockAudio();
}

void tSongCtrl::open_song()
{
	// use char arrays here for thread safety reasons
	DBG(AUDIO,"opening song: >%s<\n", songname);
	char namelocal[2052];
	int i;
	strncpy(namelocal,songname,2048);
	namelocal[2048]=0;
	playing.pause=startpaused;
	int l=strlen(namelocal);
	if (!strlen(namelocal)) return;
	songfile.open(namelocal);
	playing.songstarted=1;
}

void tSongCtrl::some_predecode()
{
	int thistime=SDL_GetTicks();
	decode_ticks=thistime-decode_lasttime;
	if (decode_ticks>500) decode_ticks=500;
	decode_lasttime=thistime;

	int target=4096;
	int target2=decode_ticks*obtained.freq/250;
	if (target2>target) target=target2;
	// decode at least 4 times as much as it took to render one frame
	DBG(AUDIO,"Filling buffer, targets are: %d %d\n", target, target2);

	songfile.decode(target);
}

void audio_yield()
{
	if (songctrl.refreshsong)
	{
		songctrl.close_song();
		if (songctrl.songname[0]) songctrl.open_song();
		songctrl.refreshsong=0;
	}
	songctrl.some_predecode();
}



