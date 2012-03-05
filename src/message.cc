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
	ipos=pos;
	modifier="%";
	modifier+=c[0];
	while (pos<contents.length())
	{
		if (contents[pos++]!='%') continue;
		lpos=pos-1;
		if (contents[pos]=='%') { pos++; continue; }
		while (isdigit(contents[pos]) || contents[pos]=='.') pos++;
		if (c.find(contents[pos])==string::npos)
		{
			fprintf(fou,"!!! bad output qualifier %c, expecting \"%s\"!!!",contents[pos],c.c_str());
			return 0;
		}
		modifier=contents.substr(lpos,pos-lpos+1);
		fprintf(fou,"%s",contents.substr(ipos,lpos-ipos).c_str());
		pos++;
		return 1;
	}
	fprintf(fou,"%s",contents.substr(ipos).c_str());
	contents="";
	pos=0;
	return 0;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(const string b)
{
	if (skiptochar("s")) fprintf(fou,modifier.c_str(),b.c_str());
	else contents+=b;
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(const char * b)
{
	if (skiptochar("s")) fprintf(fou,modifier.c_str(),b);
	else contents+=b;
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(unsigned int b)
{
	skiptochar("cxd");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(int b)
{
	skiptochar("cxd");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(float b)
{
	skiptochar("f");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER & MESSAGEWRITER::operator ,(double b)
{
	skiptochar("f");
	fprintf(fou,modifier.c_str(),b);
	return *this;
}

MESSAGEWRITER::~MESSAGEWRITER()
{
	fprintf(fou,"%s",contents.c_str()+pos);
}



