/******************************************************************************
 * $Id$
 * This file is part of ebiff (http://ebiff.sf.net)                           *
 * This file is distributed under the terms of GNU GPL license.               *
 ******************************************************************************/

/******************************************************************************
 * File description:
 *	bla bla bla
 * Notes:
 *	blablabla
 * Authors:
 * 	Enrico Tassi <gareuselesinge@users.sourceforge.net>
 ******************************************************************************/


#include <list>
#include <map>
#include <algorithm>

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>

#include "parser.h"
#include "relation.h"
#include "scheduler.h"
#include "config.h"

class Action
	{
private:
	map<Mailbox*,bool> already_sync;
public:
	Action();
	void operator() (Relation*);
	int result();
	};

Action::Action() {}
void Action::operator() (Relation*r) {

if(already_sync[r->box] || r->box->IsChanged())
	{
	// we have to sync and notify
	if(!already_sync[r->box])
		{
		//printf("SYNC\n");
		r->box->Sync();
		already_sync[r->box] = true;
		}

	//printf("Notify\n");
	r->nty->Notify(r->box);
	}
else
	{
	//printf("Unchanged\n");
	}
	
}
int Action::result() {return 0;}

static struct option opts[] = { 
	{ "version" , no_argument , NULL , 'V'},
	{ NULL, 0, NULL, 0 } };

static void usage(const char *progname) {
	fprintf(stderr, "Usage:  %s\t[-V|--version] \n", progname);
}

int main(int argc, char *argv[])
{
int res;
while ((res=getopt_long(argc,argv,"V",opts,NULL))!= -1) 
	{
	if (res == 'V') 
		{
		usage(argv[0]);
		exit(0);
		}
	else 
		{
		usage(argv[0]);
		exit(1);
		}
	}
	
// check for the config file
const char* home = getenv("HOME");
char path[255];
snprintf(path,255,"%s/%s",home,CFG_STD);
FILE* f = fopen(path,"r");

if(f == NULL)
	{
	fprintf(stderr,"Failed %s, trying the DEVEL path %s\n",path,CFG_DEVEL);
	snprintf(path,255,"%s",CFG_DEVEL);
	}
else
	fclose(f);

//fprintf(stderr,"Using %s\n",path);

try
	{
	Scheduler s(parse_config(path));
	while(1)
		{
		time_t x;
		list<Relation*> ready;
	
		x=s.get_next_burst();
		//printf("sleeping for %lu\n",x);
		sleep(x);
	
		ready = s.get_ready(x);
		//printf("Ready= %d\n",ready.size());
		assert(ready.size() > 0);
	
		for_each(ready.begin(),ready.end(),Action());
		}
	}
catch(Failure f)
	{
	fprintf(stderr,"Nothing ot do!\n");
	exit(1);
	}
catch(MailboxException f)
	{
	fprintf(stderr,"EXCEPTION: %s\n",f.msg.c_str());
	exit(1);
	}
catch(NotifierException f)
	{
	fprintf(stderr,"EXCEPTION: %s\n",f.msg.c_str());
	exit(1);
	}
catch(...)
	{
	fprintf(stderr,"EXCEPTION: ?\n");
	exit(1);
	}

	
return 0;
}
