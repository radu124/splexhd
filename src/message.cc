/*******************************************************************
(C) 2010 by Radu Stefan
radu124@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*******************************************************************/

#include "message.h"

int MESSAGEWRITER::skiptochar(string c)
{
	// store the initial value of pos
	int ipos=pos;
	int lpos;
	// set default value of modifier to %(first accepted char)
	modifier="%";
	modifier+=c[0];
	// scan the format string for format specifiers
	while (pos<contents.length())
	{
		// skip everything up to the first % sign
		if (contents[pos++]!='%') continue;
		// first char position of the format specifier
		lpos=pos-1;
		// skip %%
		if (contents[pos]=='%') { pos++; continue; }
		while (isdigit(contents[pos]) || contents[pos]=='.') pos++;
		// format character not one of the expected ones
		if (c.find(contents[pos])==string::npos)
		{
			fprintf(fou,"!!! bad output qualifier %c, expecting \"%s\"!!!",contents[pos],c.c_str());
			return 0;
		}
		modifier=contents.substr(lpos,pos-lpos+1);
		// print format string contents up to the specifier
		fprintf(fou,"%s",contents.substr(ipos,lpos-ipos).c_str());
		pos++;
		return 1;
	}
	// format specifier not found, print the remainder of the
	// format string
	fprintf(fou,"%s",contents.substr(ipos).c_str());
	contents="";
	pos=0;
	return 0;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(const string b)
{
	// if there are no format specifiers in the format string
	// append to format string
	if (skiptochar("s")) fprintf(fou,modifier.c_str(),b.c_str());
	else contents+=b;
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(const char * b)
{
	// same as the string version of this function
	if (skiptochar("s")) fprintf(fou,modifier.c_str(),b);
	else contents+=b;
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(unsigned int b)
{
	// find next format specifier, print an error
	// if the type is different from %c %x %d or %u
	skiptochar("ucxd");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(int b)
{
	skiptochar("dcxu");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(float b)
{
	skiptochar("feEgG");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(double b)
{
	skiptochar("feEgG");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER::~MESSAGEWRITER()
{
	fprintf(fou,"%s",contents.c_str()+pos);
}



