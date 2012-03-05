#include "effects.h"

#include "configuration.h"
#include "audio.h"
#include "audiosfx.h"

#define NAMEDSFXLOAD(a) sfx_load(datadir+"/sounds/" #a ".ogg",SE_##a);

void init_effects()
{
	NAMEDSFXLOAD(bug)
	NAMEDSFXLOAD(explosion)
	NAMEDSFXLOAD(hit);
	NAMEDSFXLOAD(infotron);
	NAMEDSFXLOAD(pcb);
	NAMEDSFXLOAD(push);
	NAMEDSFXLOAD(victory);
}




