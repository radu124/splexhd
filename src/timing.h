#ifndef __HDR_timing_h
#define __HDR_timing_h


struct ttiming
{
	int o1,o2,o3,o4,o5,o6,o7,o8;
	int m1,m2,m3,m4,m5,m6;
	int bugact;
	int xpropag;
	int xplode;
	int exitlev;
	int expdanger;
	int reddiskblow;
	int enemymove;
	int enemymoveshort;
	int &enemyrotate;
	int &murphmove;
	int murphvanishing;
	int &zonkmove;
	int &zonkfall;
	int explosion;
	int isoct(int i);
	void init(int scale);
	ttiming()
		:enemyrotate(o8)
		,murphmove(o8)
		,zonkmove(o8)
		,zonkfall(o8) { init(8); }
};

extern ttiming timing;

#endif
