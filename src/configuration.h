/**
 * @file configuration.h global variables for program settings that
 * will be loaded and written into the user configuration file
 */
#ifndef __HDR_configuration_h
#define __HDR_configuration_h


#include "includes.h"
#include "confrw.h"

/** path to the configuration file */
extern string configFile;
/** path to the file storing the player progress and statistics */
extern string scoresFile;
/** directory where the configuration will be saved,
 * under Linux, it defaults to $HOME/.config/splexhd */
extern string homeconfdir;
/** directory where the game data is stored, the game will look for
 * the default.ttf font in the following directories and will assign the
 * data dir to the first path found
 *   <homeconfdir>/data
 *   ./data
 *   /usr/share/games/splexhd/data
 *   /usr/local/share/games/splexhd/data
 */
extern string datadir;

/**
 * This is a list of configuration variables, their types and default values
 * this list is used 4 times
 *   1) declaring the variable extern
 *   2) declaring the variable
 *   3) reading the variable from the configuration file
 *   4) writing the variable to the configuration file
 * each time, CONFIGITEM is substituted with a different macro
 */
#define CONFIGLIST \
CONFIGITEM(int,video_glflush,0) \
CONFIGITEM(int,video_glfinish,0) \
CONFIGITEM(int,video_fpslimit,0) \
CONFIGITEM(int,video_dofullscreen,0) \
CONFIGITEM(int,video_overrideres,0) \
CONFIGITEM(int,scr_width,800) \
CONFIGITEM(int,scr_height,600) \


/**
 * Substitution of CONFIGITEM that will declare the variable b of type a
 * with default value c
 */
#define CI_DECLARE(a,b,c) a b=c;

/**
 * Substitution of CONFIGITEM that will declare the variable b extern
 */
#define CI_DECLARE_EXTERN(a,b,c) extern a b;

/**
 * Substitution of CONFIGITEM that will produce configuration file output
 * for the variable b
 */
#define CI_PRINT(a,b,c)         \
{                               \
	fprintf(fou,""#b" = ");     \
	CONFWRITE_##a(b);           \
	fprintf(fou,"\n");          \
}

/**
 * Substitution of CONFIGARRAY that will produce configuration file output
 * for the array b with elements of type a with field name d in the config
 * file and field name c in the source code
 * Note: arrays need to be declared manually
 */
#define CIA_PRINT(a,b,c,d)                      \
{                                               \
	int it;                                     \
	for (it=0; it<b.size(); it++)               \
	{                                           \
		fprintf(fou,""#b"[%d]"#d" = ",it);      \
		CONFWRITE_##a(b[it]c);                  \
		fprintf(fou,"\n");                      \
	}                                           \
}

/**
 * Substitution of CONFIGITEM that will produce matching and reading
 * from the configuration file
 *
 * Check if the (char *) string p (in current scope) matches the name of b
 * and if it does read variable b of type a from p
 */
#define CI_READ(a,b,c)                          \
{                                               \
	if (tsimatch(p,""#b))                       \
	{                                           \
		CONFREAD_##a(p,b);                      \
	}                                           \
}

/**
 * Substitution of CONFIGARRAY that will produce matching and reading
 * of a configuration array item from the configuration file
 *
 * Check in the (char *) string p (in current scope) whether the name of
 * b and suffix d matches and read the variable if it does
 * resize the array if necessary
 */
#define CIA_READ(a,b,c,d)                        \
{                                                \
	int idx;                                     \
	if (tsamatch(p,""#b,""#d,idx))               \
	{                                            \
		assume(idx<1000,"Bad array index>1000"); \
		if (idx>=b.size()) b.resize(idx+1);      \
		CONFREAD_##a(p,b[idx]c);                 \
	}                                            \
}

/*************************************************************
 * Automatic generation of config variable extern declarations
 */
#define CONFIGITEM CI_DECLARE_EXTERN
#define CONFIGARRAY(a,b,c,d)
CONFIGLIST
#undef CONFIGARRAY
#undef CONFIGITEM


/**
 * vector that will store the unknown configuration variables
 * so that they are not lost when rewriting the configuration file
 */
extern vector<string> config_unknown;

/**
 * initialize paths (in particular homeconfdir, datadir, configFile
 * and scoresFile) and read configuration file
 *
 */
void init_config();

/**
 * Read the configuration variables from the configuration file
 * indicated by configFile
 */
int config_read();

/**
 * write the configuration variables to the configuration file
 */
void config_write();

/**
 * Match a string read from the configuration file against the name of
 * a configuration variable, a successful match is considered if the string
 * is of the form:
 * variablename<space?>=
 * @param s (char *) input string, passed as reference, on a successful
 *   match, s is increased by the length of the variable name,
 *   additional space and 1 for the equal sign
 * @param v the variable
 * @return 1 on a match 0 otherwise
 */
int tsimatch(char *&s, const char *v);

/**
 * Match a string read from the configuration file against the name of
 * a configuration item in an array
 * a successful match is considered if the string is of the form:
 * prefix[idx]suffix<space?>=
 * @param s (char *) input string, passed as reference, on a successful
 *   match, s is increased by the length of the variable name,
 *   additional space and 1 for the equal sign
 * @param prefix the name of the array
 * @param suffix an optional member field, useful for example when the
 *   array elements are structs
 * @param idx output variable that will store the array index on a
 *   successful match
 * @return 1 on a match 0 otherwise
 */
int tsamatch(char *&s, const char *prefix, const char *suffix, int &idx);


#endif
