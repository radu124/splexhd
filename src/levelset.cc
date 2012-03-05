#include "levelset.h"

#include "verbosity.h"
#include "sha1.h"
#include "fileio.h"
#include "utils.h"
#include "message.h"

uint8_t *lsetdat=NULL;
int lsetallocd;
vector<int> lsetindex;
vector<string> lsetnames;
vector<string> lsethashes;

int getnumlevels()
{
	return lsetindex.size();
}

int load_levelset_idx(string filename)
{
	const char * idxdat;
	int idxsize;
	int idxfile;
	int cnt;
	uint8_t * datdat;
	int datsize;

	idxfile=mapFile(filename,idxdat,idxsize);
	{

	}
	unmapFile(idxfile);
}

int load_levelset_dat(string filename)
{
	const char * dat;
	int datasize;
	int datafile;
	int i;
	datafile=mapFile(filename,dat,datasize);
	if (lsetallocd<datasize)
	{
		dorealloc(lsetdat,datasize);
		lsetallocd=datasize;
	}
	memcpy(lsetdat,dat,datasize);
	lsetindex.resize(0);
	lsethashes.resize(0);
	lsetnames.resize(0);
	for (i=0; i<datasize; i+=1536)
	{
		char a[32];
		lsetindex.push_back(i);
		memcpy(a,lsetdat+i+0x5a6,23);
		a[23]=0;
		lsetnames.push_back(string(a));
		string hash=compute_sha(lsetdat+i,1536);
		lsethashes.push_back(hash);
		DBG(LEVELSET,"Level loaded from DAT: %s - hash %s\n",a,hash);
	}
	unmapFile(datafile);
}

int load_levelset(string filename)
{
	return load_levelset_dat(filename);
	//levfile=mapFile(s,levdat,levdsize);
	//if (!levfile) return 0;
	//unmapFile(levfile);

}

const uint8_t* getleveldata(int levelnum)
{
	if (!lsetdat) return NULL;
	if (levelnum>=lsetindex.size()) return lsetdat;
	return lsetdat+lsetindex[levelnum];
}

string getlevelname(int levelnum)
{
	if (levelnum>=lsetnames.size()) return "";
	return lsetnames[levelnum];
}

string getlevelhash(int levelnum)
{
	if (levelnum>=lsetnames.size()) return "";
	return lsethashes[levelnum];
}



