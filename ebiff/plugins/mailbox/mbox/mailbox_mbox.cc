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


#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "mailbox_mbox.h"

#define THROW(s) {\
		char __tmp[10];\
		snprintf(__tmp,10,": %d:",__LINE__);\
		throw MailboxException(string(__FILE__) + string(__tmp) + s);}

/*******************************************************************************
 * MailboxMbox
 *
 *
 */ 
MailboxMbox::MailboxMbox() throw()
{
last_modify = 0;
last_size = 0;
//end_position = 0;
end_string = "";

path = "";
n_new = 0;
n_old = 0;
mails = vector<MailInfo>(0);

regcomp(&from_ex,"\nFrom: *",REG_ICASE|REG_EXTENDED);
regcomp(&subject_ex,"\nSubject: *",REG_ICASE|REG_EXTENDED);
regcomp(&status_ex,"\nStatus: *",REG_ICASE|REG_EXTENDED);
firsttime = true;

path = "";
newonly = false;
}

/*******************************************************************************
 * ~MailboxMbox
 *
 *
 */ 
MailboxMbox::~MailboxMbox()  throw()
{
regfree(&from_ex);
regfree(&subject_ex);
}

/*******************************************************************************
 * SetString
 *
 *
 */ 
void MailboxMbox::SetString(string name,string value) throw(MailboxException)
{
if ( name == "path" ) 
	path = value;
}

/*******************************************************************************
 * SetBool
 *
 *
 */ 
void MailboxMbox::SetBool(string name,bool value) throw(MailboxException)
{
if ( name == "newonly" ) 
	newonly = value;

}

/*******************************************************************************
 * SetNumber
 *
 *
 */ 
void MailboxMbox::SetNumber(string name,double value) throw(MailboxException)
{
}
	
/*******************************************************************************
 * IsChanged
 *
 *
 */ 
bool MailboxMbox::IsChanged() throw(MailboxException)
{
off_t s_new;
time_t t_new;
bool rc;

t_new = last_modify;
s_new = last_size;

try	{
	UpdateTimestamp();
	}
catch (MailboxException e)
	{
	throw e;
	}

rc = (t_new < last_modify) || (s_new != last_size);

last_modify = t_new;
last_size = s_new;

return rc;
}

/*******************************************************************************
 * CountNew
 *
 *
 */ 
unsigned long int MailboxMbox::CountNew() throw(MailboxException)
{
return n_new;
}

/*******************************************************************************
 * CountOld
 *
 *
 */ 
unsigned long int MailboxMbox::CountOld() throw(MailboxException)
{
return n_old;
}

/*******************************************************************************
 * GetNewFrom
 *
 *
 */ 
string MailboxMbox::GetNewFrom(unsigned long int n) throw(MailboxException)
{
return mails[n].GetFrom();
}

/*******************************************************************************
 * GetNewSubject
 *
 *
 */ 
string MailboxMbox::GetNewSubject(unsigned long int n)throw(MailboxException)
{
return mails[n].GetSubject();
}
/*******************************************************************************
 * GetNewUid
 *
 *
 */ 
string MailboxMbox::GetNewUid(unsigned long int n)throw(MailboxException)
{
return mails[n].GetUid();
}

/*******************************************************************************
 * Sync
 *
 *
 */ 
void MailboxMbox::Sync() throw(MailboxException)
{
if(!IsChanged())
	return;

/* FIX LOCKIT */
while( link(path.c_str(),(path+string(".lock")).c_str()) != 0)
	{
	//fprintf(stderr,"F\n");
	usleep(250000);
	}

UpdateTimestamp();

FILE* f = fopen(path.c_str(),"r");

if(f == NULL)
	{
	unlink((path+string(".lock")).c_str());
	THROW(string("Unable to open ") + path.c_str());
	}

#if 0
n_old = n_new;
bool check_resumed = !firsttime && newonly;
if( !firsttime && newonly && fsetpos(f,&end_position) != 0)
	{
	check_resumed = false;
	rewind(f);
	fgetpos(f,&end_position);
	if( fsetpos(f,&end_position) != 0)
		{
		unlink((path+string(".lock")).c_str());
		THROW("Unable to setpos()");
		}
	}
firsttime = false;

//fprintf(stderr,"Positioned at (%lu)\n",ftell(f));

char l[100];
regmatch_t p[1];
p[0].rm_so = p[0].rm_eo = -1;
bool in_header = false;
unsigned long int cur = 0;
string sb,fr,last;
fpos_t pos;
bool skip_next=false;
while(!feof(f))
	{
	fgetpos(f,&pos);
	fgets(l,100,f);

	if(check_resumed)
		{
		//fprintf(stderr,"checking %s with %s\n",end_string.c_str(),l);
		if ( end_string != "" &&
			end_string != l )
			{
			//we have to rewind the file and start again
			//fprintf(stderr,"OUCH! %s != %s\n",l,
			//	end_string.c_str());
			rewind(f);
			cur = 0;
			in_header = false;
			last = "";
			end_string = "";
			}
		else if ( end_string != "")
			skip_next = true;
		// test only one time
		check_resumed = false;
		}
	
	if(in_header)
		{
		regexec(&subject_ex,l,1,p,0);
		if (p[0].rm_so != -1)
			{
			unsigned int len = strlen(l);
			if (l[len-1] == '\n')
				l[len-- -1] = '\0';
			if (l[len-1] == '\r')
				l[len-- -1] = '\0';
			
			sb = &l[p[0].rm_eo];
			p[0].rm_so = p[0].rm_eo = -1;
			}
		regexec(&from_ex,l,1,p,0);
		if (p[0].rm_so != -1)
			{
			unsigned int len = strlen(l);
			if (l[len-1] == '\n')
				l[len-- -1] = '\0';
			if (l[len-1] == '\r')
				l[len-- -1] = '\0';
			fr = &l[p[0].rm_eo];
			
			p[0].rm_so = p[0].rm_eo = -1;
			}
		}
	if(!strncmp("From ",l,5))
		{
		in_header = true;
		
		last = l;
		//fprintf(stderr,"(%lu)\n",ftell(f));
		
		// assign pos to end_postion in a 
		fpos_t tmp;
		fgetpos(f,&tmp);
		fsetpos(f,&pos);
		fgetpos(f,&end_position);
		fsetpos(f,&tmp);
		}
	if(!strncmp("\n",l,1) && in_header )
		{
		if( !skip_next)
			{
			if(mails.size() <= cur)
				mails.resize(cur + 10);
			mails[cur] = MailInfo(sb,fr,last);
			//fprintf(stderr,"Added %d message\n",cur);
			cur++;
			}
		else
			{
			//fprintf(stderr,"skipping\n");	
			skip_next = false;
			}
		in_header = false;
		}
	}

n_new = cur;
end_string = last;
//fprintf(stderr,"Saving end string to %s\n",end_string.c_str());

mails.resize(mails.size());
#else
regmatch_t p[1];
string sb,fr,last;
int newm;
unsigned long pos;
char l[1024];
char *c;
size_t nr;
unsigned long int cur = 0;
n_new = n_old = 0;

while(!feof(f))
{
	pos = ftell(f);
	nr=fread(l,1,1023,f);

	if (nr == 0)
	{
		if (feof(f))
			break;
		if (ferror(f))
		{
			unlink((path+string(".lock")).c_str());
			THROW("Error reading file");
		}
	}

	if ( (c = strstr(l,"\n\n")) != NULL )
	{
		*c = '\0';
	}
	else if ( (c = strstr(l,"\r\n\r\n")) != NULL )
	{
		*c = '\0';
	}
	else
	{
		l[nr] = '\0';
	}

	p[0].rm_so = p[0].rm_eo = -1;
	// Parse read header
	regexec(&status_ex,l,1,p,0);
	if (p[0].rm_so != -1)
	{
		char bak;
		c = &l[p[0].rm_eo];
		while ((*c != '\n') && (*c != '\r'))
			c++;
		bak = *c;
		*c = '\0';
		if (strchr(&l[p[0].rm_eo],'R') == NULL)
		{
			newm = 1;
		}
		else
		{
			newm = 0;
		}
		*c = bak;
		p[0].rm_so = p[0].rm_eo = -1;
	}
	else
	{
		newm = 1;
	}

	if (newm)
	{
		n_new++;
		regexec(&subject_ex,l,1,p,0);
		if (p[0].rm_so != -1)
		{
			char bak;
			c = &l[p[0].rm_eo];
			while ((*c != '\n') && (*c != '\r'))
				c++;
			bak = *c;
			*c = '\0';
			while (l[p[0].rm_eo] == ' ')
				p[0].rm_eo++;
			sb = &l[p[0].rm_eo];
			*c = bak;
			p[0].rm_so = p[0].rm_eo = -1;
		}
		regexec(&from_ex,l+1,1,p,0);
		if (p[0].rm_so != -1)
		{
			char bak;
			c = &l[p[0].rm_eo];
			while ((*c != '\n') && (*c != '\r'))
				c++;
			bak = *c;
			*c = '\0';
			while (l[p[0].rm_eo] == ' ')
				p[0].rm_eo++;
			fr = &l[p[0].rm_eo];
			*c = bak;
			p[0].rm_so = p[0].rm_eo = -1;
		}

		if(mails.size() <= cur)
			mails.resize(cur + 10);
		mails[cur] = MailInfo(sb,fr,"");
		cur++;
	}
	else
	{
		n_old++;
	}

	// Seek for next message
	int lastempty=0;
	fseek(f,pos,SEEK_SET);
	do
	{
		pos = ftell(f);
		fgets(l,100,f);

		if ( (lastempty) && (strncmp(l,"From",4)==0) )
		{
			fseek(f,pos,SEEK_SET);
			break;
		}

		lastempty = ((strcmp(l,"\n") == 0) || (strcmp(l,"\r\n")==0));
	}
	while(!feof(f));
}
//printf("NEW: %d OLD: %d\n",n_new,n_old);
#endif

fclose(f);
unlink((path+string(".lock")).c_str());
}


/*******************************************************************************
 * UpdateTimestamps
 *
 *
 */ 
void MailboxMbox::UpdateTimestamp() throw(MailboxException)
{
struct stat s;
int rc;
string file;

// check for new mail
file = path;

rc = stat( file.c_str() , &s);

if( rc == -1 )
	THROW(string(strerror(errno)) + " " + file);

last_modify = s.st_mtime;
last_size = s.st_size;
}

/*******************************************************************************
 * 
 *
 *
 */
MailboxMbox::MailInfo::MailInfo(string sub, string fr,string _id) throw()
{
subject = sub;
from = fr;
uid = _id;
}

/*******************************************************************************
 * 
 *
 *
 */
MailboxMbox::MailInfo::MailInfo() throw()
{
subject = "--no subject--";
from = "--no from--";
}

/*******************************************************************************
 * 
 *
 *
 */
string MailboxMbox::MailInfo::GetSubject() throw()
{
return subject;
}
/*******************************************************************************
 * 
 *
 *
 */
string MailboxMbox::MailInfo::GetUid() throw()
{
return uid;
}

/*******************************************************************************
 * 
 *
 *
 */
string MailboxMbox::MailInfo::GetFrom() throw()
{
return from;
}


extern "C" Mailbox* new_mailbox(){
return new MailboxMbox();
}

extern "C" void delete_mailbox(Mailbox*p){
delete p;
}



