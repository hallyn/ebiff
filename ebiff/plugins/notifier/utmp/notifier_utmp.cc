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

#include <stdio.h>
#include <utmp.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#include "notifier_utmp.h"

/*******************************************************************************
 *
 *
 */
void NotifierUtmp::SetString(string name, string value) throw(NotifierException)
{
}
/*******************************************************************************
 *
 *
 */
void NotifierUtmp::SetBool(string name, bool value) throw(NotifierException)
{
if( name == "printall")
	printall = value;

}
/*******************************************************************************
 *
 *
 */
void NotifierUtmp::SetNumber(string name, double value) throw(NotifierException)
{

}
		
/*******************************************************************************
 *
 *
 */
void NotifierUtmp::Notify(ReducedMailbox* m)
{
long unsigned int n = m->CountNew();

struct utmp *entry=NULL;
struct passwd *pass=NULL;

pass = getpwuid(getuid());

if ( pass == NULL)
	{
	throw NotifierException("getpwuid failed");
	}

setutent();

while( (entry = getutent()) != NULL)
 	{
	if(entry->ut_type == USER_PROCESS &&
	   !strcmp(entry->ut_user,pass->pw_name))
		{
		FILE* f;
		int len = UT_LINESIZE + strlen("/dev/");
		char file[len];
		snprintf(file,len,"/dev/%s",entry->ut_line);
		f = fopen(file,"w");
		if( f != NULL)
			{
			
			if( n > 0)
				fprintf(f,"\n\nNew mail in mailbox %s",
					m->GetName().c_str());
			else
				fprintf(f,"\n\nNo new mail in mailbox %s",
					m->GetName().c_str());
	
			if(printall != 0 && n > 0)
				fprintf(f,":\n");
			else
				fprintf(f,"\n\n");

			if(printall != 0 && n > 0)
				{
				for(long unsigned int i=0 ; i < n ; i++)
					fprintf(f,"  %d From: %s Subject: %s\n",
						i,m->GetNewFrom(i).c_str(),
						m->GetNewSubject(i).c_str());
				fprintf(f,"\n");
				}

			fclose(f);
			}
		}
	}
endutent();
}
	
/*******************************************************************************
 *
 *
 */
NotifierUtmp::NotifierUtmp()
{
printall = 0;
}

/*******************************************************************************
 *
 *
 */
NotifierUtmp::~NotifierUtmp()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierUtmp();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



