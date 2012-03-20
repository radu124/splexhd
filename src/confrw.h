/**
 * @file confrw.h helper functions and macros for reading and writing
 * the configuration file
 */
#ifndef __HDR_confrw_h
#define __HDR_confrw_h


#include "includes.h"
#include "globals.h"

/**
 * Declare a function that reads an enum-type variable
 * The function will have two parameters:
 *      - a string containing the value
 *      - a variable of the given type, passed by reference
 *      that will store the read value
 *
 * @param a the enumeration type
 * @param b a string containing the valid enumeration items,
 * separated by commas
 */
#define MULTIRDR(a,b)\
void CONFREAD_##a(char *&s, a &v) \
{ \
	 v=(a)multiplechoice(s,b); \
	 DBG(CONFIG," value " #a " %d (from:%s)\n" ,(int)v ,s); \
}

/**
 * @defgroup CONFWRITE
 * Declare a set of macros with uniform naming for writing different
 * data types to the configuration file
 * @param a - value to be written
 * note the usage of file "fou" from the current scope
 * @{
 */
#define CONFWRITE_GLfloat(a) fprintf(fou,"%f",a)
#define CONFWRITE_int(a) fprintf(fou,"%d",a)
#define CONFWRITE_string(a) fprintf(fou,"%s",a.c_str())

/** @} */

/**
 * @defgroup CONFREAD
 * Declare a set of macros with uniform naming for reading different
 * data types from the configuration files
 * @{
 */

/**
 * Read a value of type GLfloat from a (char *) string
 * @param s the input string
 * @param v variable that will store the value
 */
void CONFREAD_GLfloat(char *&s, GLfloat &v);

/**
 * Read a value of type int from a (char *) string
 * @param s the input string
 * @param v variable that will store the value
 */
void CONFREAD_int(char *&s, int &v);

/**
 * Read a value of type (STL) string from a (char *) string
 * @param s the input string
 * @param v variable that will store the value
 */
void CONFREAD_string(char *&s, string &v);

/**
 * Read a value of type mcolor from a (char *) string
 * the color is stored in the #RRGGBB format
 * @param s the input string
 * @param v variable that will store the value
 */
void CONFREAD_mcolor(char *&s, mcolor &v);

/** @} */

/**
 * Match a string to a set of possible values
 * @param s the string to look for
 * @param choices the possible values, separated by comma
 * @return the index of the matched value, if the value was not
 * found among the list of options 0 is returned
 */
int multiplechoice(const char *s, const char *choices);


#endif
