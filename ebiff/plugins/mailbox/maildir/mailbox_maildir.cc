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
#include <errno.h>
#include <stdio.h>


#include "mailbox_maildir.h"


#define MAX(a,b)	(((a)<(b))?(b):(a)) 
#define MIN(a,b)	(((a)<(b))?(a):(b)) 

#define THROW(s) {throw MailboxException(string(__FILE__) + string(__LINE__) + s)}

/*******************************************************************************
 * MailboxMaildir
 *
 *
 */ 

void MailboxMaildir::Reset() {
last_modify_new = 0;
last_modify_old = 0;
path = "";
n_new = 0;
mails = vector<MailInfo>(0);
}
					
MailboxMaildir::MailboxMaildir() throw()
{
Reset();

regcomp(&from_ex,"^From: *",REG_ICASE|REG_EXTENDED);
regcomp(&subject_ex,"^Subject:",REG_ICASE|REG_EXTENDED);
	
}

/*******************************************************************************
 * ~MailboxMaildir
 *
 *
 */ 
MailboxMaildir::~MailboxMaildir()  throw()
{
regfree(&from_ex);
regfree(&subject_ex);
}

/*******************************************************************************
 * SetString
 *
 *
 */ 
void MailboxMaildir::SetString(string name,string value) throw(MailboxException)
{
if ( name == "path" ) 
	path = value;
	
//cout << "1set " << name << " " << value << endl;
}

/*******************************************************************************
 * SetBool
 *
 *
 */ 
void MailboxMaildir::SetBool(string name,bool value) throw(MailboxException)
{
cout << "2set " << name << " " << value << endl;
}

/*******************************************************************************
 * SetNumber
 *
 *
 */ 
void MailboxMaildir::SetNumber(string name,double value) throw(MailboxException)
{
cout << "3set " << name << " " << value << endl;
}
	
/*******************************************************************************
 * IsChanged
 *
 *
 */ 
bool MailboxMaildir::IsChanged() throw(MailboxException)
{
time_t t_new,t_old;
bool rc;

t_new = last_modify_new;
t_old = last_modify_old;

try	{
	UpdateTimestamps();
	}
catch (MailboxException e)
	{
	throw e;
	}

rc = t_new < last_modify_new || t_old < last_modify_old;

last_modify_new = t_new;
last_modify_old = t_old;

return rc;
}

/*******************************************************************************
 * CountNew
 *
 *
 */ 
unsigned long int MailboxMaildir::CountNew() throw(MailboxException)
{
return n_new;
}

/*******************************************************************************
 * CountOld
 *
 *
 */ 
unsigned long int MailboxMaildir::CountOld() throw(MailboxException)
{
return n_old;
}

/*******************************************************************************
 * GetNewFrom
 *
 *
 */ 
string MailboxMaildir::GetNewFrom(unsigned long int n) throw(MailboxException)
{
return mails[n].GetFrom();
}

/*******************************************************************************
 * GetNewSubject
 *
 *
 */ 
string MailboxMaildir::GetNewSubject(unsigned long int n)throw(MailboxException)
{
return mails[n].GetSubject();
}
/*******************************************************************************
 * GetNewUid
 *
 *
 */ 
string MailboxMaildir::GetNewUid(unsigned long int n)throw(MailboxException)
{
return mails[n].GetFilename();
}

/*******************************************************************************
 * Sync
 *
 *
 */ 
void MailboxMaildir::Sync() throw(MailboxException)
{
unsigned long int ex_n_new;
	
if(!IsChanged())
	return;

ex_n_new = n_new;

try	{
	UpdateTimestamps();
	UpdateCounters();
	}
catch (MailboxException e)
	{
	throw e;
	}

mails.resize(n_new);

if(n_new == 0)
	return;

DIR* d;
struct dirent *e;
unsigned long int i;
long int end;

d = opendir((path + "/new").c_str());

if(d == NULL)
	throw MailboxException(string(strerror(errno)) + " " + path + "/new");

// mark all as deletable
for(unsigned long int i=0;i<mails.size();i++)
	mails[i].Mark();

i = 0;
end = mails.size()-1;
while( (e = readdir(d)) != NULL )
	{
	bool found = false;
	if( strncmp(e->d_name,".",1) == 0 )
		continue;

	// our n_new may be changed, so we have to "enlarge" it if needed.
	if(i == mails.size())
		{
		//FIX MAY BE SLOW (no POSIX implementation for seekdir)
		mails.resize(i+1);
		end = mails.size()-1;
		}
	
	// check if the file is already preset
	for(unsigned long int j=0; j < MIN( ex_n_new, mails.size() ) ; j++)
		if(mails[j].IsMarked() && 
		   mails[j].GetFilename() == string(e->d_name))
			{
			mails[j].Unmark();
			found = true;
			i++;
			break;
			}
	
	if(found)
		continue;
	
	// if not present
	for(int j=end;j>=0;j--)
		// find the first empty block from the end
		if(mails[j].IsMarked())
			{
			string f,s;
			ParseMessage(path + string("/new/") + 
				string(e->d_name),s,f);
			mails[j] = MailInfo(s,f,string(e->d_name));
			end=j-1;
			i++;
			break;
			}
	}
closedir(d);

if(i != n_new) {
	Reset();
	Sync(); //FIXME may diverge!
	//THROW(MailboxException("MailboxMaildir::Sync internal error"));
}


}


/*******************************************************************************
 * UpdateTimestamps
 *
 *
 */ 
void MailboxMaildir::UpdateTimestamps() throw(MailboxException)
{
struct stat s;
int rc;
string file;

// check for new mail
file = path + "/new";

rc = stat( file.c_str() , &s);

if( rc == -1 )
	throw MailboxException(string(strerror(errno)) + " " + file);

last_modify_new = s.st_mtime;

// check for other changes
file = path + "/cur";

rc = stat( file.c_str() , &s);

if( rc == -1 )
	throw MailboxException(string(strerror(errno)) + " " + file);

last_modify_old = s.st_mtime;
}
/*******************************************************************************
 * UpdateCounters
 *
 *
 */ 
void MailboxMaildir::UpdateCounters() throw(MailboxException)
{
try	{
	n_new = CountFiles(path + "/new");
	n_old = CountFiles(path + "/cur");
	}
catch (MailboxException e)
	{
	throw e;
	}
}

/*******************************************************************************
 * CountFiles
 *
 *
 */ 
unsigned long int MailboxMaildir::CountFiles(string file)throw(MailboxException)
{
DIR* d;
struct dirent *e;
int i = 0;

d = opendir(file.c_str());

if(d == NULL)
	throw MailboxException(string(strerror(errno)) + " " + file);

while( (e = readdir(d)) != NULL )
	{
	if( strncmp(e->d_name,".",1) != 0 )
		i++;
	}

closedir(d);

return i;
}
/*******************************************************************************
 * 
 *
 *
 */
#define MAIL_LINE_LEN 100
void MailboxMaildir::ParseMessage(string file,string &subject,string &from) throw(MailboxException)
{
char line[MAIL_LINE_LEN] = "";
FILE* f;
bool found_s = false,found_f = false;

f = fopen(file.c_str(),"r");
if(f == NULL)
	throw MailboxException(string(strerror(errno)) + " " + file);

subject = "--no subjects--";
from = "--no from--";

do
	{
	char* s = fgets(line,MAIL_LINE_LEN,f);
	if(s == NULL)
		break;		

	regmatch_t p[1];
	
	p[0].rm_so = p[0].rm_eo = -1;
	if(!found_s)
		{
		regexec(&subject_ex,line,1,p,0);
		if (p[0].rm_so != -1)
			{
			unsigned int len = strlen(line);
			if (line[len-1] == '\n')
				line[len-- -1] = '\0';
			if (line[len-1] == '\r')
				line[len-- -1] = '\0';
		
			subject = string(&line[p[0].rm_eo]);
			found_s = true;
			continue;
			}
		}
	
	p[0].rm_so = p[0].rm_eo = -1;
	if(!found_f)
		{
		regexec(&from_ex,line,1,p,0);
		if (p[0].rm_so != -1)
			{
			unsigned int len = strlen(line);
			if (line[len-1] == '\n')
				line[len-- -1] = '\0';
			if (line[len-1] == '\r')
				line[len-- -1] = '\0';
	
			from = string(&line[p[0].rm_eo]);
			found_f = true;
			}
		}
	
	}
while(strcmp("\n",line) != 0 || strcmp("\r\n",line) != 0 || feof(f) || 
	(found_s && found_f));
	
fclose(f);
}

/*******************************************************************************
 * 
 *
 *
 */
MailboxMaildir::MailInfo::MailInfo(string sub, string fr, string file) throw()
{
subject = sub;
from = fr;
filename = file;
}

/*******************************************************************************
 * 
 *
 *
 */
MailboxMaildir::MailInfo::MailInfo() throw()
{
subject = "--no subject--";
from = "--no from--";
filename = "--no file--";
marked = false;
}

/*******************************************************************************
 * 
 *
 *
 */
string MailboxMaildir::MailInfo::GetSubject() throw()
{
return subject;
}

/*******************************************************************************
 * 
 *
 *
 */
string MailboxMaildir::MailInfo::GetFrom() throw()
{
return from;
}

/*******************************************************************************
 * 
 *
 *
 */
string MailboxMaildir::MailInfo::GetFilename() throw()
{
return filename;
}
/*******************************************************************************
 * 
 *
 *
 */
void MailboxMaildir::MailInfo::Mark() throw()
{
marked = true;
}
/*******************************************************************************
 * 
 *
 *
 */
void MailboxMaildir::MailInfo::Unmark() throw()
{
marked = false;
}
/*******************************************************************************
 * 
 *
 *
 */
bool MailboxMaildir::MailInfo::IsMarked() throw()
{
return marked;
}

extern "C" Mailbox* new_mailbox(){
return new MailboxMaildir();
}

extern "C" void delete_mailbox(Mailbox*p){
delete p;
}



