/**
 * @file message.h
 * print messages formatted like printf() but using c++ features
 * (and macros)
 */
#ifndef __HDR_message_h
#define __HDR_message_h


#include "includes.h"

/**
 * Class for producing formatted output messages in a style similar to
 * printf(), but without a variable number of parameters on the stack
 * this makes printing in this way safer in that mismatches between
 * the string format and actual parameters can be caught
 *
 * The class is not meant to be used directly, but rather using macros
 * like MESSAGE, __FILE_OUT, __COND_DBG_OUT, these hide the fact that
 * the comma is not a separator but an operator and appear to the user
 * as a normal printf().
 *
 * Note that the order of "parameter" evaluation will be different
 * from what you expect in a classic printf() (important if parameter
 * expressions have side-effects)
 *
 * while the end result is rather nice, the implementation details are
 * not: the class implements the comma operator, which some sources do
 * nor recommend, and variadic macros are used (which is not in the
 * c++ standard but is de facto supported)
 *
 * currently only s,c,x,d,f formats are supported
 * the class accepts c++ strings as well as C (char*) strings
 */
class MESSAGEWRITER
{
public:
	/**
	 * class constructor, stores the given FILE *, initializes contents
	 * to an empty string and sets the position indicator to the first
	 * character in contents
	 */
	MESSAGEWRITER(FILE *f):fou(f),contents(""),pos(0){;}
	/**
	 * overloaded comma operator acting similar to << in ostream
	 * when the contents string does not contain a format specifier
	 * (for example an empty contents string) append this to the format
	 * string
	 *
	 * @param b string to be printed (with %s) or format string
	 * @return a reference to the same class to be used by the next
	 * comma operator
	 */
	MESSAGEWRITER & operator ,(const string b);
	/**
	 * overloaded comma operator acting similar to << in ostream
	 * when the contents string does not contain a format specifier
	 * (for example an empty contents string) append this to the format
	 * string
	 *
	 * @param b string to be printed (with %s) or format string
	 * @return a reference to the same class to be used by the next
	 * comma operator
	 */
	MESSAGEWRITER & operator ,(const char * b);
	/**
	 * overloaded comma operator acting similar to << in ostream
	 * print an integer using the %c/x/d/u formats, produce a warning
	 * if the wrong specifier is encountered (note %d actually prints
	 * as signed)
	 *
	 * @param b unsigned integer to be printed
	 * @return a reference to the same class to be used by the next
	 * comma operator
	 */
	MESSAGEWRITER & operator ,(unsigned int b);
	/**
	 * overloaded comma operator acting similar to << in ostream
	 * print an integer using the %c/x/d/u formats, produce a warning
	 * if the wrong specifier is encountered (note %u, %x will actually
	 * print unsigned)
	 *
	 * @param b integer to be printed
	 * @return a reference to the same class to be used by the next
	 * comma operator
	 */
	MESSAGEWRITER & operator ,(int b);
	/**
	 * overloaded comma operator acting similar to << in ostream
	 * print a floating point value using the %f/g/G/e/E formats
	 * produce a warning if the wrong specifier is encountered
	 *
	 * @param b unsigned integer to be printed
	 * @return a reference to the same class to be used by the next
	 * comma operator
	 */
	MESSAGEWRITER & operator ,(float b);
	/**
	 * overloaded comma operator acting similar to << in ostream
	 * print a floating point value using the %f/g/G/e/E formats
	 * produce a warning if the wrong specifier is encountered
	 *
	 * @param b unsigned integer to be printed
	 * @return a reference to the same class to be used by the next
	 * comma operator
	 */
	MESSAGEWRITER & operator ,(double b);

	/**
	 * write the remainder of the contents string
	 * any remaining % specifiers will be printed as is
	 */
	 ~MESSAGEWRITER();
	/**
	 * advance character-by-character through contents looking for
	 * a %(char) specifier, skip %% and for %(char) check if the found
	 * character is in the list of accepted characters
	 *
	 * if it is not, print a warning message and set modifier to
	 * %(first character in the list of allowed characters)
	 * otherwise set modifier to the found %(char) string
	 * digits and . may appear in the specifier like this %[0-9.]*[char]
	 *
	 * print the part of the contents string from pos to the specifier
	 * and advance pos until after the specifier
	 *
	 * if the end of the contents string is reached, print the remainder
	 * of the contents string and set contents to the empty string and
	 * pos to 0
	 *
	 * @param c list of accepted characters
	 * @return 0 on error, 1 on success
	 */
	int skiptochar(string);

	/**
	 * file to write to
	 */
	FILE *fou;
	/**
	 * stored format string
	 */
	string contents;
	/**
	 * index inside the format string indicating characters that have
	 * already been processed
	 */
	int pos;
	/**
	 * modifier found by skiptochar()
	 */
	string modifier;
};

/**
 * Write formatted output to stderr if a condition is met
 * the variable list of arguments hides the comma operator of MESSAGEWRITER
 * and makes it appear as a separator
 */
#define __COND_DBG_OUT(cond,...) do { if (!(cond)) break; { MESSAGEWRITER __MW(stderr); __MW , __VA_ARGS__; }} while(0)

/**
 * Write formatted output to the given file
 * the variable list of arguments hides the comma operator of MESSAGEWRITER
 * and makes it appear as a separator
 */
#define __FILE_OUT(fou,...) do { {MESSAGEWRITER __MW(fou); __MW , __VA_ARGS__;} } while(0)

/**
 * Similar to assert, on a failed condition print a formatted error message and
 * then raise an assert
 * see MESSAGEWRITER class for message format details
 */
#define assume(cond,...) \
	do { if (cond) break; \
	{ MESSAGEWRITER __MW(stderr); __MW , "\n!!! ERROR !!! in " ,__func__ ,"() at " , __FILE__ ,":",__LINE__ , "\n" , __VA_ARGS__ , "\n\n"; \
	} assert(0); } while(0)

/**
 * Produce a warning message if a condition is met
 * see MESSAGEWRITER class for message format details
 */
#define warnif(cond,...) \
	do { if (!cond) break; \
	{ MESSAGEWRITER __MW(stderr); __MW , "\n!!! WARNING !!! in " ,__func__ ,"() at " , __FILE__ ,":",__LINE__ , "\n" , __VA_ARGS__ , "\n\n"; \
	} } while(0)

/**
 * paranoid version of assume, check for conditions that in theory
 * could never ever happen, outside debug this could be defined to
 * have no effect
 */
#define assume_paranoid(cond,...) assume(cond,__VA_ARGS__)

/**
 * produce a formatted message to stdout
 * see MESSAGEWRITER class for message format details
 */
#define MESSAGE(...) __FILE_OUT(stdout,__VA_ARGS__)



#endif
