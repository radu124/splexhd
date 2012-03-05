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

#include "audiosfx.h"
#include "audio.h"
#include "message.h"
#include "verbosity.h"
#include "utils.h"

tEffectPlay playingeffects[EFFECTSPLAY];
tEffectList effectdata;
int sfx_agecounter=0;

/**
 * Effect playback happens in a separate thread than the one requesting
 * the playback, sound effects are enqueued here until the player picks
 * them up
 */
struct tEffectRequestQueue
{
	volatile int head;
	volatile int tail;
	volatile int cancelall;
	tEffectPlay queue[EFFECTRQUEUESIZE];
} effectsQueue;

int16_t interpolate(int16_t *buf, int pos, int chn=1)
{
	return buf[pos/256];
}

int sfx_load(string fn, int eid)
{
	int padding=AUDIOBUF_SIZE<16384?16384:AUDIOBUF_SIZE;
	int16_t buf[4112];
	int bs=0;
	int i,j,v;
	int pos=0,readpos;

	int allocd=220500;
	int16_t *effectbuf=NULL;

	OggVorbis_File vf;
	if(ov_fopen((char *) fn.c_str(), &vf) < 0) return 0;
	int channels = ov_info(&vf,0)->channels;
	int samplerate = ov_info(&vf,0)->rate;

	if (!eid)
	{
		if (effectdata.count>255) return 0;
		eid=++effectdata.count;
	}

	INFO(AUDIO,"OGG load effect %d: chn:%d, rate=%d\n", eid, channels, samplerate);
	buf[0]=buf[1]=0;
	dorealloc(effectbuf,allocd);
	readpos=256;
	while (1)
	{
		int ret=ov_read(&vf,(char *)(buf+2),8192,0,2,1,&bs);
		if (!ret) break;
		int locchn=ov_info(&vf,-1)->channels;
		int locrate=ov_info(&vf,-1)->rate;
		int samples=ret/2/locchn;
		int step=locrate*256/obtained.freq;
		DBG(AUDIO,"Read %d chn:%d sr:%d\n", samples, locchn, locrate);
		for (i=0; i<samples; i++)
		{
			v=0;
			for (j=0; j<locchn; j++) v+=buf[2+i*locchn+j];
			v=v/locchn;
			buf[2+i]=(int16_t) v;
		}
		while (readpos<(samples+1)*256)
		{
			readpos+=step;
			if (pos>=allocd)
			{
				allocd=allocd*2;
				dorealloc(effectbuf,allocd);
			}
			effectbuf[pos++]=interpolate(buf,readpos);
		}
		readpos-=samples*256;
		buf[0]=buf[samples];
		buf[1]=buf[samples+1];
	}
	dorealloc(effectbuf,pos+padding);
	for (i=0; i<padding; i++) effectbuf[i+pos]=0;
	INFO(AUDIO,"Loaded effect, len=%d\n", pos);
	effectdata.len[eid]=pos;
	effectdata.data[eid]=effectbuf;
	ov_clear(&vf);
	return eid;
}

void sfx_start(int effnum, int volume_left, int volume_right)
{
	int vol_le,vol_rt,eipos;
	if (!effnum) return;
	DBG(SFX,"Starting effect %d, volume(%d-%d)\n", effnum, volume_left, volume_right);
	int i=effectsQueue.head;
	if (i>=effectsQueue.tail+EFFECTRQUEUESIZE)
	{
		DBG(SFX,"Queue full, cannot encode effect.\n");
		return;
	}

	eipos=effectsQueue.head%EFFECTRQUEUESIZE;
	effectsQueue.queue[eipos].id=effnum;
	effectsQueue.queue[eipos].pos=0;
	effectsQueue.queue[eipos].vol_le=volume_left;
	effectsQueue.queue[eipos].vol_rt=volume_right;
	effectsQueue.queue[eipos].id=effnum;

	effectsQueue.head++;
}

void sfx_stop_all()
{
	effectsQueue.cancelall=1;
}

int sfx_playbackready()
{
	int i;
	int lasteffect;
	int eqtail;
	DBG(SFX,"SFX list: ");
	for (i=0; i<EFFECTSPLAY; i++)
	{
		int eid=playingeffects[i].id;
		DBG(SFX,"%d(%d/%d V:%d:%d) ", eid, playingeffects[i].pos, effectdata.len[eid],
			playingeffects[i].vol_le, playingeffects[i].vol_rt);
	}
	DBG(SFX,"\n");
	for (i=0; i<EFFECTSPLAY; i++)
	{
		int eid=playingeffects[i].id;
		if (!eid) continue;
		int epos=playingeffects[i].pos;
		if (epos>effectdata.len[eid])
		{
			playingeffects[i].id=0;
			DBG(SFX,"SFX: Effect %d (in pos: %d) ended\n", eid, i);
		}
	}
	lasteffect=EFFECTSPLAY-1;
	for (i=0; i<=lasteffect; i++)
	{
		while (lasteffect>=0 && !playingeffects[lasteffect].id)
		{
			lasteffect--;
		}
		if (lasteffect<=i) break;
		if (!playingeffects[i].id)
		{
			playingeffects[i]=playingeffects[lasteffect];
			DBG(SFX,"Effect moved from pos %d to pos %d", lasteffect, i);
			i--;
		}
	}
	lasteffect++;
	eqtail=effectsQueue.tail;
	while (eqtail<(int) effectsQueue.head)
	{
		int qid=eqtail%EFFECTRQUEUESIZE;
		int eid=effectsQueue.queue[qid].id;
		int seteff=lasteffect;
		eqtail++;
		if (!eid) continue;
		if (!effectdata.data[eid]) continue;
		if (lasteffect>=EFFECTSPLAY)
		{
			seteff=0;
			for (i=1; i<EFFECTSPLAY; i++)
				if (playingeffects[i].age<playingeffects[seteff].age) seteff=i;
		}
		else lasteffect++;
		playingeffects[seteff]=effectsQueue.queue[qid];
		playingeffects[seteff].pos=0;
		playingeffects[seteff].age=sfx_agecounter++;
		DBG(SFX,"Read effect %d from queue (pos %d) into effect %d", eid, qid, lasteffect);
	}
	effectsQueue.tail=eqtail;
	for (i=0; i<lasteffect; i++)
	{
		int eid=playingeffects[i].id;
		int epos=playingeffects[i].pos;
		playingeffects[i].ptr=effectdata.data[eid]+epos;
	}
	return lasteffect;
}



