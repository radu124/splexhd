// splexgexport.cpp : Defines the entry point for the console application.
//
#include "includes.h"
#include "spx_emfs.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int tilesize=32;
	//if (argc>1) tilesize=_wtoi(argv[1]);
	initEmfs(tilesize);
	return 0;
}

