#include "player.h"

#include "levelset.h"
#include "configuration.h"
#include "message.h"
#include "verbosity.h"

struct tlevstats
{
	int state;
	int time;
};

map<string,tlevstats> playerstats;

int levelstate(int level)
{
	string lhash=getlevelhash(level);
	tlevstats a=playerstats[lhash];
	return a.state;
}

void updatestats(string lhash, int victory, int time)
{
	if (victory) playerstats[lhash].state=2;
	if (playerstats[lhash].state==0) playerstats[lhash].state=1;
	playerstats[lhash].time+=time;
}

void finishlevel(int level, int victory, int time)
{
	string lhash=getlevelhash(level);
	updatestats(lhash,victory,time);
	FILE *scf=fopen(scoresFile.c_str(),"a");
	if (!scf) return;
	fprintf(scf,"%s %d %d USER\n",lhash.c_str(),victory,time);
	fclose(scf);
}

void loadplayerstate()
{
	int time;
	int victory;
	char hash[128];
	char line[1024];
	FILE *scf=fopen(scoresFile.c_str(),"r");
	if (!scf) return;
	while (!feof(scf))
	{
		if (!fgets(line,999,scf)) WARN(PLAYERSTATE,"Error reported while reading player state from file");
		sscanf(line,"%40s %d %d\n",hash,&victory,&time);
		updatestats(hash,victory,time);
		DBG(PLAYERSTATE,"updated :%s: %d %d\n", hash, victory, time);
	}
	fclose(scf);
}



