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

#include <unistd.h>
#include <stdio.h>

#include "notifier_sox.h"

queue< string > NotifierSox::files;
pthread_t NotifierSox::manager;
pthread_attr_t NotifierSox::manager_att;
pthread_mutex_t NotifierSox::files_mutex;
pthread_mutexattr_t NotifierSox::files_mutex_att;

/*******************************************************************************
 *
 *
 */
void NotifierSox::SetString(string name, string value) throw(NotifierException)
{
if(name == "file")
	file = value;
}
/*******************************************************************************
 *
 *
 */
void NotifierSox::SetBool(string name, bool value) throw(NotifierException)
{

}
/*******************************************************************************
 *
 *
 */
void NotifierSox::SetNumber(string name, double value) throw(NotifierException)
{
}

void NotifierSox::Notify(ReducedMailbox* m)
{
pthread_mutex_lock(&files_mutex);
files.push(file);
pthread_mutex_unlock(&files_mutex);

}

void *NotifierSox::manager_f(void*x)
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
NotifierSox::NotifierSox()
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
NotifierSox::~NotifierSox()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierSox();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



