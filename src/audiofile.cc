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

#include "audiofile.h"
#include "message.h"
#include "verbosity.h"

void tAudioFile::decode(int target)
{
	int i;
	int maxtarget=AUDIOFILE_BUFFER_SIZE-AUDIOFILE_MARGIN-(decodepos-playpos);
	int16_t tempbuf[4112];
	if (target==0 || target>maxtarget) target=maxtarget;
	int pos=decodepos;
	while (target>0)
	{
		if (!isopen)
		{
			// when song is not open, just fill with zeros
			// this may be wasteful, but it's not going to be
			// any slower than actually decoding the song
			for (i=0; i<target; i++)
			{
				buffer[0][pos%AUDIOFILE_BUFFER_SIZE]=0;
				buffer[1][pos%AUDIOFILE_BUFFER_SIZE]=0;
				pos++;
			}
			decodepos=pos;
			target=0;
			break;
		}
		int ret=ov_read(&oggfile,(char *)tempbuf,8192,0,2,1,&ogg_bs);
		if (ret<=0) 
		{
			stopsat=pos;
			close();
			continue;
		}
		int locchn=ov_info(&oggfile,-1)->channels;
		int locrate=ov_info(&oggfile,-1)->rate;
		int samples=ret/2/locchn;
		int chn2=locchn>1?1:0;
		
		DBG(AUDIO,"Read %d chn:%d sr:%d\n", samples, locchn, locrate);
		
		// TODO: for now we assume the ogg rate is the same with our playback rate
		for (i=0; i<samples; i++)
		{
			int qa=tempbuf[i*locchn];
			int qb=tempbuf[i*locchn+chn2];
			if (reducedstereo)
			{
				int t=(qa*5+qb*3)/8;
				qb=(qa*3+qb*5)/8;
				qa=t;
				// reduce the stereo effect since
				// each player has his own track to play
			}
			buffer[0][pos%AUDIOFILE_BUFFER_SIZE]=qa;
			buffer[1][pos%AUDIOFILE_BUFFER_SIZE]=qb;				
			pos++;
		}
		decodepos=pos;
		target-=samples;
	}
}

void tAudioFile::close()
{
	if (!isopen) return;
	isopen=0;
	ov_clear(&oggfile);
}

void tAudioFile::reset()
{
	playpos=0;
	decodepos=0;
	ogg_bs=0;
}

int tAudioFile::open(const char *filename)
{
	if (isopen) close();
	reset();
	// zero length by default
	stopsat=0;
	if (ov_fopen((char *) filename, &oggfile) >=0) 
	{ 
		isopen=1; 
		stopsat=1000000000; 
	}
}



