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
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "notifier_netout.h"

#define VERSION 1
#define MAX(a,b)	(((a)<(b))?(b):(a)) 
#define MIN(a,b)	(((a)<(b))?(a):(b)) 

/*******************************************************************************
 *
 *
 */
void NotifierNetout::SetString(string name, string value) throw(NotifierException)
{
if(name == "pass")
	param.pass = value;

param.ready++;
}
/*******************************************************************************
 *
 *
 */
void NotifierNetout::SetBool(string name, bool value) throw(NotifierException)
{

}
/*******************************************************************************
 *
 *
 */
void NotifierNetout::SetNumber(string name, double value) throw(NotifierException)
{
if(name == "port")
	param.port = (unsigned int)value;

param.ready++;
}
		
/*******************************************************************************
 *
 *
 */
void NotifierNetout::Notify(ReducedMailbox* m)
{
string out;
unsigned long n =  m->CountNew();
char tmpn[20],tmpo[20];

snprintf(tmpn,20,"%ld",n);
snprintf(tmpo,20,"%ld",m->CountOld() );

out = m->GetName() + "\r\n" +
	m->GetCommand() + "\r\n" +
	string(tmpn) + " " + string(tmpo) + "\r\n"; 

for(long unsigned int i=0 ; i < n ; i++)
	out  = out +  m->GetNewFrom(i) + "\r\n" +
		m->GetNewSubject(i) + "\r\n" +
		m->GetNewUid(i) + "\r\n";

pthread_mutex_lock(&queue_mutex);
param.outgoing.push(out);
pthread_mutex_unlock(&queue_mutex);

}
	
/*******************************************************************************
 *
 *
 */
NotifierNetout::NotifierNetout()
{

pthread_mutexattr_init(&queue_mutex_att);
pthread_mutex_init(&queue_mutex,&queue_mutex_att);

pthread_attr_init(&manager_att);

param.ready = 0;

manager = pthread_create(&manager,&manager_att,manager_f,
		new pair <pthread_mutex_t*,thread_param *>(
			&queue_mutex,&param));
	
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
static int readl(char* buf,int length,int s)
{
ssize_t l=0;
char buffer[1000];
int len = 0;
int size = 1000;
while(1)
{
	
l = TEMP_FAILURE_RETRY(recv(s,&(buffer[len]),size - len,0));

for( int i = 0;/*MAX(0,len-1) ;*/ i < l + len && i < size; i++)
	{
	if(buffer[i] == '\r' &&
	   i+1 < l + len &&
	   buffer[i+1] == '\n')
		{
		strncpy(buf,buffer,MIN(i+2,length));
		buf[MIN(i+2,length)] = '\0';
		for(int j = MIN(i+2,size),k=0 ; j < len + l ; j++,k++)
			buffer[k] = buffer[j];
		len = len + l - MIN(i+2,length);
		return i+2;
		}
	
	}
len = len + l;

if(len == size)
	{
	strncpy(buf,buffer,MIN(length,size));
	buf[MIN(size,length)] = '\0';
	len = 0;	
	return MIN(size,length);
	}

if(l == 0)
	return 0;

}

}

void *NotifierNetout::manager_f(void*x)
{
pair < pthread_mutex_t*,thread_param * >* w = 
	(pair < pthread_mutex_t* ,thread_param * >* )x;
	
pthread_mutex_t *q_mutex = w->first;
thread_param *p = w->second;

int rc = 0;
int s = -1, s1 = -1;
struct sockaddr_in ad;
socklen_t adl = sizeof(ad);
char line[1000];

while( p->ready != 2) sleep(1);

for (;;){
	sleep(5);
	s = socket(PF_INET, SOCK_STREAM, 0);
	if(s == -1)
		{
		fprintf(stderr,"Unable to create the socket\n");
		exit(1);
		}

	/* socket stuff */
	memset(&ad,0,adl);
	ad.sin_family = AF_INET;
	ad.sin_port = htons(p->port);
	ad.sin_addr.s_addr = INADDR_ANY;
	rc = bind(s,(struct sockaddr*)&ad,adl);
		
	if(rc != 0)
		{
		fprintf(stderr,"Unable to bind: %s\n",strerror(errno));
		close(s);
		exit(1);
		}

	rc = listen(s,1);

	if(rc != 0)
		{
		fprintf(stderr,"Unable to listen: %s\n",strerror(errno));
		close(s);
		exit(1);
		}

	s1 = accept(s,NULL,NULL);

	if(s1 == -1)
		{
		fprintf(stderr,"Unable to accept: %s\n",strerror(errno));
		close(s);
		exit(1);
		}

	close(s);
	s = -1;
	
	/* now we do not accept, s1 will be he new socket */
	char tmp[10];
	snprintf(tmp,10,"%d",VERSION);

	rc = sendl(tmp,strlen(tmp),s1);

	if(rc == -1)
		{
		fprintf(stderr,"Unable to send: %s\n",strerror(errno));
		close(s1);
		s1 = -1;
		continue;
		}

	rc = readl(line,1000,s1);

	if(line != p->pass + "\r\n")
		{
		fprintf(stderr,"Wrong password!\n");
		close(s1);
		s1 = -1;
		continue;
		}

	while(s1 != -1) /* while the socket is valid */
		{
		sleep(1);
		
		pthread_mutex_lock(q_mutex);
		int size = p->outgoing.size();
		pthread_mutex_unlock(q_mutex);

		while( size > 0 )
			{
			pthread_mutex_lock(q_mutex);
			string tmp = p->outgoing.front();
			p->outgoing.pop();
			pthread_mutex_unlock(q_mutex);

			rc = sendl(tmp.c_str(),tmp.length(),s1);

			if(rc == -1)
				{
				fprintf(stderr,"Unable to send: %s\n",
						strerror(errno));
				close(s1);
				s1 = -1;
				break;	
				}
			
			pthread_mutex_lock(q_mutex);
			size = p->outgoing.size();
			pthread_mutex_unlock(q_mutex);
			}
		}
	}
}


/*******************************************************************************
 *
 *
 */
NotifierNetout::~NotifierNetout()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierNetout();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



