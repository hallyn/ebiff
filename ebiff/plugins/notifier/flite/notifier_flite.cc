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


#include <flite/flite.h>

#include "notifier_flite.h"

/*******************************************************************************
 *
 *
 */
void NotifierFlite::SetString(string name, string value) throw(NotifierException)
{
if(name == "one")
	one = value;
if(name == "zero")
	zero = value;
if(name == "value")
	more = value;
}
/*******************************************************************************
 *
 *
 */
void NotifierFlite::SetBool(string name, bool value) throw(NotifierException)
{

}
/*******************************************************************************
 *
 *
 */
void NotifierFlite::SetNumber(string name, double value) throw(NotifierException)
{
}

string NotifierFlite::replace_metas(string s,ReducedMailbox* m)
{
long unsigned int n = m->CountNew();
string name = m->GetName();
char num[5];
snprintf(num,5,"%lu",n);
string number = num;

string::size_type b;
b = s.find("$n$");
if( b != string::npos )
	s = s.substr(0,b) + number + s.substr(b+3,s.length());
b = s.find("$b$");
if( b != string::npos )
	s = s.substr(0,b) + name + s.substr(b+3,s.length());

return s;
}

/*******************************************************************************
 *
 *
 */
void NotifierFlite::Notify(ReducedMailbox* m)
{
long unsigned int n = m->CountNew();

char num[5];
snprintf(num,5,"%lu",n);

string s;

if(n > 1)
	s = replace_metas(more,m);
else if (n == 1)
	s = replace_metas(one,m);
else
	s = replace_metas(zero,m);

pthread_mutex_lock(&display_mutex);
talks.push(s);
pthread_mutex_unlock(&display_mutex);

}

extern "C" cst_voice* register_cmu_us_kal();

void *NotifierFlite::manager_f(void*x)
{
pair<queue< string > *,pthread_mutex_t*> *p;
p = (pair<queue< string > *,pthread_mutex_t*> *) x;
queue< string > *talks = p->first;
pthread_mutex_t* display_mutex = p->second;

cst_voice* v;

v = register_cmu_us_kal();

while(true)
	{
	sleep(1);
	
	pthread_mutex_lock(display_mutex);
	int size = talks->size();
	pthread_mutex_unlock(display_mutex);
	
	while(size > 0)
		{
		string o;

		pthread_mutex_lock(display_mutex);
		o = talks->front();
		talks->pop();
		size = talks->size();
		pthread_mutex_unlock(display_mutex);
		
		// play o
		flite_text_to_speech(o.c_str(),v,"play");
		
		usleep(500000);
		}
	
	
	}
}
/*******************************************************************************
 *
 *
 */
NotifierFlite::NotifierFlite()
{
flite_init();
	
pthread_mutexattr_init(&display_mutex_att);
pthread_mutex_init(&display_mutex,&display_mutex_att);
pthread_attr_init(&manager_att);
manager = pthread_create(&manager,&manager_att,manager_f,
	(void*) new pair< queue<string> *, pthread_mutex_t*>
		(&talks,&display_mutex)
	);
}

/*******************************************************************************
 *
 *
 */
NotifierFlite::~NotifierFlite()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierFlite();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



