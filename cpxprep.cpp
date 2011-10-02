#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <queue>

using namespace std;

enum ISTATUS{ ISTAT_NEW=0, ISTAT_ADDED=1, ISTAT_BEGAN=2, ISTAT_AFTERIMPLEM=3, ISTAT_FINISHED=4 };

struct inclitem
{
	inclitem(string s):name(s),fname(s+".cpx"),status(ISTAT_ADDED),fin(NULL),usedby(0),counter(0),id(0){;}
	string name;
	string fname;
	queue<int> uqueue;
	int id;
	int status; // 0, 1 began parsing, 2 after implem, 3 finished
	FILE *fin;	
	int counter;
	int usedby;
	void process();
	void adduse(string un);
	void openfile();
	int unused;	
};

vector<inclitem*> ilist;
vector<int> istack;
vector<int> mstack;
int crtfile;
int verbose=0;
int showlines=1;
const char *exename="cpxprep";

char *namespacename(char *filename);

void assume(int b,const char *c,const char *extra="")
{
    if (b) return;
    fprintf(stderr,"Error: %s.\nErrno:%x\n%s\n",c,errno,extra);
    exit(1);
}

int strxcmp(const char *a, string txt, int pos)
{
	char c;
	int r;
	int len=strlen(a);
	assume(pos<=txt.size(),"strxcmp looking beyond end-of-string");
	r=strncmp(a,txt.c_str()+pos,len);
	if (r) return r;
	//    printf("b[len]-%c-%i-",b[len],b[len]);
	if (len+pos==txt.size()) return 0;
	c=txt[len+pos];
	if (c!=' ' && c!='\t' && c!=0 && c!=0 && c!=10 && c!=13 && c!=9 && c!=8) return -1;
	return 0;
}

char *namespacename(const char *filename)
{

	char result[1024],c;
	int i,j;
	strcpy(result,"__CPXNS_");
	j=strlen(result);
	for (i=0; filename[i]!=0 ; i++)
	{
		if (j>100) break;
		c=filename[i];
		if (isalnum(c)) result[j++]=toupper(c);
		else if (c=='/') result[j++]='_';
		else 
		{
			sprintf(result+j,"__%02X_",c);
			j+=4;
		}
	}
	result[j]=0;
	//printf("---%s\n",result);
	return strdup(result);

}

int isfilenamechar(int v)
{
	return isalnum(v)
		|| v=='.' || v=='_' || v=='-'
			;
}

string readLine(FILE *f)
{
	string res="";
	int l,eoln=0;
	char txt[4100];
	char *unused;
	while (1)
	{
		txt[0]=0;
		unused=fgets(txt,4095,f);
		txt[4096]=0;
		l=strlen(txt);
		if (l==0) return res;
		if (txt[l-1]==10 || txt[l-1]==13)
		{
			eoln=1;
			if (l>1 && txt[l-2]==13) txt[l-2]=0;
			txt[l-1]=0;
		}
		res+=txt;
		return res;
	}
}

void solvestacks()
{
	while (1)
	{
		int crt;
		if (istack.size()>0)
		{
			crt=istack.back();
			inclitem *it=ilist[crt];
			if (it->status==ISTAT_FINISHED) 
			{
				istack.pop_back();
				continue;
			}
			if (it->status==ISTAT_AFTERIMPLEM)
			{
				istack.pop_back();
				mstack.push_back(crt);
				continue;
			}
			it->process();
			continue;
		}
		if (mstack.size()==0) break;
		crt=mstack.back();
		inclitem *it=ilist[crt];
		if (it->status==ISTAT_FINISHED)
		{
			mstack.pop_back();
			continue;
		}
		it->process();
	}
}

void inclitem::openfile()
{
	if (name=="") fin=fdopen(1,"r");
	else fin=fopen(fname.c_str(),"r");
	
	inclitem *it=ilist[usedby];
	if (fin==NULL)
	{
		if (id==0)
		{
			fprintf(stderr,"cpxprep: error: input file: %s, could not be opened\n",
				fname.c_str());
		}
		else
			fprintf(stderr,"%s:%d: error: %s could not be opened\n",
				it->fname.c_str(),
				it->counter,
      				fname.c_str());
		exit(1);
	}
	counter=0;
	status=ISTAT_BEGAN;
}

void inclitem::adduse(string un)
{
	int atpos;
	int res;
	int i;
	
	if (verbose) { fprintf(stderr,"  ** found use statement for %s\n",un.c_str()); }
	
	for (i=0; i<ilist.size(); i++) if (ilist[i]->name==un) 
	{
		int tgtstat=ilist[i]->status;
		if (tgtstat==ISTAT_BEGAN && status<ISTAT_AFTERIMPLEM)
		{
			fprintf(stderr,"%s:%d: error, circular reference %s, stack follows\n",fname.c_str(),counter,ilist[i]->name.c_str());
			for (i=istack.size()-1; i>=0; i--)
			{
				fprintf(stderr," *** %s:%d\n",ilist[istack[i]]->name.c_str(),ilist[istack[i]]->counter);
			}
			fprintf(stderr," *** %s:%d\n",ilist[mstack.back()]->name.c_str(),ilist[mstack.back()]->counter);
			exit(1);
		}
		uqueue.push(i);
		return;
	}
	atpos=ilist.size();
	
	inclitem *it=new inclitem(un);	
	it->id=atpos;
	it->usedby=id;
	ilist.push_back(it);
	uqueue.push(atpos);
	return;	
}
	
void inclitem::process()
{
	int pos,pos2;
	string txt,un;
	if (status==ISTAT_FINISHED) return;
	
	if (verbose) { fprintf(stderr,"  * cpx processing %s\n",fname.c_str()); }
	
	if (uqueue.size()>0)
	{
		int crt=uqueue.front();
		if (verbose) { fprintf(stderr,"  * items found in queue, using %s\n",ilist[crt]->fname.c_str()); }
		istack.push_back(crt);
		uqueue.pop();
		return;
	}
	
	if (fin==NULL) openfile();
	
	if (id!=crtfile)
	{
		if (showlines) printf("#line %d \"%s\"\n",counter+1,fname.c_str());
		crtfile=id;
	}	
	
	while (!feof(fin))
	{
		txt=readLine(fin);
		pos=0;
		counter++;
		while (pos<txt.length() && (txt[pos]==' ' || txt[pos]=='\t')) pos++;
		if (!strxcmp("implementation",txt,pos))
		{
			if (verbose) { fprintf(stderr,"  * encountered implementation, deferring processing\n"); }	
			status=ISTAT_AFTERIMPLEM;
			printf("\n");
			return;
		}
		if (!strxcmp("interface",txt,pos))
		{			
			printf("\n");
			continue;
		}		
		if (!strxcmp("uses",txt,pos))
		{
			pos+=5;
			while (1)
			{
				// we allow multiple included files in an uses clause				
				while (pos<txt.size() && isblank(txt[pos])) pos++;
				//skip leading white spaces
				if (pos>=txt.size()) 
				{
					fprintf(stderr,"%s:%d cpxprep error\n",fname.c_str(),counter);
					fprintf(stderr,"*** End of line encountered in uses clause, expecting semicolon. ***");
					exit(1);
				}
				if (txt[pos]==';') break;
				if (txt[pos]==',') { pos++; continue; }
				pos2=pos;
				while (pos<txt.size() && isfilenamechar(txt[pos])) pos++;
				un=txt.substr(pos2,pos-pos2);
				adduse(un);
			}
			printf("\n");
			return;
		}
		printf("%s\n",txt.c_str());	
	}
	status=ISTAT_FINISHED;
	fclose(fin);
	//if (innamespace) printf("}//namespace\n");
	printf("\n\n");
}

int main(int argc, char *argv[])
{
	FILE *f;
	int i;
	string inputname;
	assume(argc>=2,"incorrect usage: normal is cpxprep [options] <filename>\nOptions are:\n   -c   read input from console\n   -v   verbose\n --no-line-numbers\n   --   end of options");
	inputname=argv[argc-1];
	for (i=1; i<argc; i++)
	{
		if (!strcmp("-c",argv[i])) inputname="";
		if (!strcmp("-v",argv[i])) verbose=1;
		if (!strcmp("--",argv[i])) break;
		if (!strcmp("--no-line-numbers",argv[i])) showlines=0;
	}
	crtfile=-1;
	inclitem *it=new inclitem(inputname); 
	ilist.push_back(it);
	it->fname=inputname;
	it->id=0;
	if (inputname=="") it->name="console";
	mstack.push_back(0);	
	solvestacks();
	return 0;
}
