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


#include "notifyer_sox.h"

queue< string > NotifyerSox::files;
pthread_t NotifyerSox::manager;
pthread_attr_t NotifyerSox::manager_att;
pthread_mutex_t NotifyerSox::files_mutex;
pthread_mutexattr_t NotifyerSox::files_mutex_att;

/*******************************************************************************
 *
 *
 */
void NotifyerSox::SetString(string name, string value) throw(NotifyerException)
{
if(name == "file")
	file = value;
}
/*******************************************************************************
 *
 *
 */
void NotifyerSox::SetBool(string name, bool value) throw(NotifyerException)
{

}
/*******************************************************************************
 *
 *
 */
void NotifyerSox::SetNumber(string name, double value) throw(NotifyerException)
{
}

void NotifyerSox::Notify(ReducedMailbox* m)
{
pthread_mutex_lock(&files_mutex);
files.push(file);
pthread_mutex_unlock(&files_mutex);

}

void *NotifyerSox::manager_f(void*x)
{
pair<queue< string > *,pthread_mutex_t*> *p;
p = (pair<queue< string > *,pthread_mutex_t*> *) x;
queue< string > *files = p->first;
pthread_mutex_t* files_mutex = p->second;

while(true)
	{
	sleep(1);
	
	pthread_mutex_lock(files_mutex);
	int size = files->size();
	pthread_mutex_unlock(files_mutex);
	
	while(size > 0)
		{
		string o;

		pthread_mutex_lock(files_mutex);
		o = files->front();
		files->pop();
		size = files->size();
		pthread_mutex_unlock(files_mutex);
		
		// play file o
		char cmd[255];
		snprintf(cmd,255,"play %s",o.c_str());
			
		system(cmd);
		
		usleep(500000);
		}
	
	
	}
}
/*******************************************************************************
 *
 *
 */
NotifyerSox::NotifyerSox()
{
file = "";
	
pthread_mutexattr_init(&files_mutex_att);
pthread_mutex_init(&files_mutex,&files_mutex_att);
pthread_attr_init(&manager_att);
manager = pthread_create(&manager,&manager_att,manager_f,
	(void*) new pair< queue<string> *, pthread_mutex_t*>
		(&files,&files_mutex)
	);
}

/*******************************************************************************
 *
 *
 */
NotifyerSox::~NotifyerSox()
{

}

extern "C" Notifyer* new_notifyer(){
return new NotifyerSox();
}

extern "C" void delete_notifyer(Notifyer*p){
delete p;
}



