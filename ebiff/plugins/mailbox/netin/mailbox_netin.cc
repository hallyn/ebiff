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

#define TEMP_FAILURE_RETRY(r) (r)

#include <iostream>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "mailbox_netin.h"
#include "mailbox_exception.h"


#define MAX(a,b)	(((a)<(b))?(b):(a)) 
#define MIN(a,b)	(((a)<(b))?(a):(b)) 

#define THROW(s) {\
		char __tmp[10];\
		snprintf(__tmp,10,": %d:",__LINE__);\
		throw MailboxException(string(__FILE__) + string(__tmp) + s);}

#define VERSION 1
		
/*******************************************************************************
 * MailboxNetin
 *
 *
 */ 
MailboxNetin::MailboxNetin() throw()
{

pthread_mutexattr_init(&queue_mutex_att);
pthread_mutex_init(&queue_mutex,&queue_mutex_att);

pthread_attr_init(&manager_att);

manager = pthread_create(&manager,&manager_att,manager_f,
		new pair< pair <NetInfo*,pthread_mutex_t*> *,queue<string>*>(
			new pair<NetInfo*,pthread_mutex_t*>(
				&netinfo,&queue_mutex),
			&incoming));
	
}

MailboxNetin::MailBoxInfo::MailBoxInfo()throw()
{
mails = vector<MailInfo>(0);
n_new = n_old = 0;
name = "--no name--";
command = "--no command--";
}

class Buff
{
public:
int len;
int size;
char buffer[65536];

Buff() {len = 0;size = 65536;};
void clear() {len = 0;};
};

static int readl(char* buf,int len,int s,Buff* b)
{
ssize_t l=0;

while(1)
{
for( int i = 0;/*MAX(0,b->len-1) ;*/ i < l + b->len && i < b->size; i++)
	{
	if(b->buffer[i] == '\r' &&
	   i+1 < l + b->len &&
	   b->buffer[i+1] == '\n')
		{
		strncpy(buf,b->buffer,MIN(i+2,len));
		buf[MIN(i+2,len)] = '\0';
		for(int j = MIN(i+2,b->size),k=0 ; j < b->len + l ; j++,k++)
			b->buffer[k] = b->buffer[j];
		b->len = b->len + l - MIN(i+2,len);
		return i+2;
		}
	
	}
b->len = b->len + l;

if(b->len == b->size)
	{
	strncpy(buf,b->buffer,MIN(len,b->size));
	buf[MIN(b->size,len)] = '\0';
	b->len = 0;	
	return MIN(b->size,len);
	}



l = TEMP_FAILURE_RETRY(recv(s,&(b->buffer[b->len]),b->size - b->len,0));

if(l == 0)
	return 0;

}

}

static int sendl(const char* buf,int len,int s)
{
int rc=0, sent = 0, sent2 = 0;

while(sent < len)
	{
	rc = TEMP_FAILURE_RETRY(send(s,&buf[sent],len-sent,MSG_NOSIGNAL));
	if(rc == -1)
		return -1;
	sent += rc;
	}

while(sent2 < 2)
	{
	rc = TEMP_FAILURE_RETRY(send(s,
		&"\r\n"[sent2],2-sent2,MSG_NOSIGNAL));
	if(rc == -1)
		return -1;
	sent2 += rc;
	}

return sent;
}

void *MailboxNetin::manager_f(void*x)
{
pair< pair < NetInfo* , pthread_mutex_t* >* ,queue < string > * >* w = 
	(pair< pair < NetInfo* , pthread_mutex_t* >* ,queue < string > * >* )x;
	
NetInfo* n = w->first->first;
pthread_mutex_t *q_mutex = w->first->second;
queue<string>* q = w->second;
int s = -1;
int rc;
struct sockaddr_in ad;
socklen_t adl = sizeof(ad);
Buff b;
char line[1000];
string file;

while ( n-> ready < 3) sleep(1);

for(;;){
	sleep(5);
	
	pthread_mutex_lock(q_mutex);
	//q->clear(); 
#warning implement !!!
	pthread_mutex_unlock(q_mutex);

	s = socket(PF_INET, SOCK_STREAM, 0);

	if(s == -1)
		{
		fprintf(stderr,"Unable to create the socket\n");
		exit(1);
		}

	struct addrinfo hint, *addr;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = PF_INET;
	rc = getaddrinfo(n->host.c_str(), NULL, &hint, &addr);

	if(rc != 0)
		{
		fprintf(stderr,"Unable to get addr info\n");
		close(s);
		s = -1;
		continue;
		}

	memset(&ad, 0, sizeof(ad));
	ad.sin_family = AF_INET;
	ad.sin_port = htons(n->port);
	ad.sin_addr = ((struct sockaddr_in *)(addr->ai_addr))
			->sin_addr;
	freeaddrinfo(addr);

	rc = connect(s,(struct sockaddr *)&ad,adl);
	if(rc == -1)
		{
		fprintf(stderr,"Unable to connect "
			"host='%s'='%s' port='%d' : ",
			n->host.c_str(),
			inet_ntoa(ad.sin_addr),
			n->port);
		char buff[100];
		fprintf(stderr,"%s\n",
			strerror_r(errno, buff,100));
		close(s);
		s = -1;
		continue;
		}

	/* handshake */

	rc = readl(line,1000,s,&b);

	if(rc <= 0)
		{
		fprintf(stderr,"Lost connection\n");	
		close(s);
		s = -1;
		b.clear();
		exit(1);
		}

	int version = atoi(line);

	if(version != VERSION)
		{
		fprintf(stderr,"Wrong version %d != %d\n",
			version,VERSION);	
		close(s);
		s = -1;
		b.clear();
		exit(1);
		}
		
	rc = sendl(n->pass.c_str(),n->pass.length(),s);

	while(s != -1) {
		do {
			rc = readl(line,1000,s,&b);

			if(rc <= 0)
				{
				fprintf(stderr,"Lost connection\n");	
				close(s);
				s = -1;
				b.clear();
				break;
				}

			file = file + line;
			
		} while( strcmp(line,"\r\n"));

		pthread_mutex_lock(q_mutex);
		q->push(file);
		pthread_mutex_unlock(q_mutex);
	}
}
}

/*******************************************************************************
 * ~MailboxNetin
 *
 *
 */ 
MailboxNetin::~MailboxNetin()  throw()
{
}

list<string> chunk(string s)
{
list <string> a;

unsigned long int rc=0,rco=0;

do
	{
	rc = s.find("\r\n",rco);

	if(rc != string::npos)
		{
		a.push_back(s.substr(rco,rc-rco));
			
		rco = rc + 2;
		}
	}
while(rc != string::npos);

return a;
}

/*******************************************************************************
 * parse
 *
 *
 */ 
MailboxNetin::MailBoxInfo MailboxNetin::parse(string s)
{
list<string> l = chunk(s);

MailBoxInfo b;

if( l.size() < 4 )
	return b; //ERROR

string name = *l.begin();
l.erase(l.begin());

string command = *l.begin();
l.erase(l.begin());

string nums = *l.begin();
l.erase(l.begin());

unsigned long int n_new=0,n_old=0;
sscanf(nums.c_str(),"%lu %lu",&n_new,&n_old);

b.n_new = n_new;
b.n_old = n_old;
b.name = name;
b.command = command;

b.mails.resize(n_new);

for( unsigned long int i = 0 ; i < n_new ; i++)
	{
	if(l.size() < 4)
		return b; //ERROR
	
	string from = *l.begin();
	l.erase(l.begin());

	string subj = *l.begin();
	l.erase(l.begin());

	string uid = *l.begin();
	l.erase(l.begin());

	b.mails[i].subject = subj;
	b.mails[i].from = from;
	b.mails[i].filename = uid;
	}

return b;
}
/*******************************************************************************
 * SetString
 *
 *
 */ 
void MailboxNetin::SetString(string name,string value) throw(MailboxException)
{
if(name == "host")
	{
	netinfo.host = value;
	netinfo.ready++;
	}
else if(name == "pass")
	{
	netinfo.pass = value;
	netinfo.ready++;
	}

}

/*******************************************************************************
 * SetBool
 *
 *
 */ 
void MailboxNetin::SetBool(string name,bool value) throw(MailboxException)
{
	
}

/*******************************************************************************
 * SetNumber
 *
 *
 */ 
void MailboxNetin::SetNumber(string name,double value) throw(MailboxException)
{
if(name == "port")
	{
	netinfo.port = (unsigned int)value;
	netinfo.ready++;
	}
}
	
/*******************************************************************************
 * IsChanged
 *
 *
 */ 
bool MailboxNetin::IsChanged() throw(MailboxException)
{
string s;

do
	{
	s = "";

	pthread_mutex_lock(&queue_mutex);
	if(!incoming.empty())
		{
		s = incoming.front();
		incoming.pop();
		}
	pthread_mutex_unlock(&queue_mutex);

	if( s != "")
		infos.push(parse(s)); 
	}
while( s != "");

return !infos.empty();
}

/*******************************************************************************
 * CountNew
 *
 *
 */ 
unsigned long int MailboxNetin::CountNew() throw(MailboxException)
{
return current.n_new;
}

/*******************************************************************************
 * CountOld
 *
 *
 */ 
unsigned long int MailboxNetin::CountOld() throw(MailboxException)
{
return current.n_old;
}

/*******************************************************************************
 * GetNewFrom
 *
 *
 */ 
string MailboxNetin::GetNewFrom(unsigned long int n) throw(MailboxException)
{
return current.mails[n].from;
}

/*******************************************************************************
 * GetNewSubject
 *
 *
 */ 
string MailboxNetin::GetNewSubject(unsigned long int n)throw(MailboxException)
{
return current.mails[n].subject;
}
/*******************************************************************************
 * GetNewUid
 *
 *
 */ 
string MailboxNetin::GetNewUid(unsigned long int n)throw(MailboxException)
{
return current.mails[n].filename;
}

/*******************************************************************************
 * Sync
 *
 *
 */ 
void MailboxNetin::Sync() throw(MailboxException)
{

if (!infos.empty())
	{
	current = infos.front();
	infos.pop();
	}

}

/*******************************************************************************
 * 
 *
 *
 */
MailboxNetin::MailInfo::MailInfo(string sub, string fr, string file) throw()
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
MailboxNetin::MailInfo::MailInfo() throw()
{
subject = "--no subject--";
from = "--no from--";
filename = "--no file--";
}

/*******************************************************************************
 * 
 *
 *
 */

MailboxNetin::NetInfo::NetInfo()
{
port = 0;
host = "--no host--";
pass = "--no pass--";
ready = 0;
}

extern "C" Mailbox* new_mailbox(){
return new MailboxNetin();
}

extern "C" void delete_mailbox(Mailbox*p){
delete p;
}

string MailboxNetin::GetName()throw()
{
return current.name;
}

string MailboxNetin::GetCommand()throw()
{
return current.command;
}

