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


#include <dlfcn.h>

#include "pluginregistry.h"
#include "config.h"

#define MAX(a,b) (((b)>(a))?(b):(a))

map<string,void*> PluginRegistry::plugins_mailbox;
map<string,void*> PluginRegistry::plugins_notifyer;

void PluginRegistry::load_mailbox_plugin(const char*name,new_mailbox_t**m,
			delete_mailbox_t**d)
{
const char* pwd = getenv("PWD");
size_t len = 	MAX(strlen(SO_STD),strlen(pwd)+1+strlen(SO_DEVEL))+
		17+ // "libebiff_mailbox_"
		strlen(name)+
		4; // ".so\0"
char file[len];
void* handle;

if( plugins_mailbox[string(name)] == NULL)
	{
	snprintf(file,len,"%slibebiff_mailbox_%s.so",SO_STD,name);
	handle = dlopen(file,RTLD_GLOBAL|RTLD_NOW);
	if(handle == NULL)
		{
		snprintf(file,len,"%s/%slibebiff_mailbox_%s.so",pwd,
			SO_DEVEL,name);
		handle = dlopen(file,RTLD_GLOBAL|RTLD_NOW);
		if(handle == NULL)
			{
			fprintf(stderr,"Unable to dlopen %s\n",dlerror());
			return;
			}
		}
	
	fprintf(stderr,"Loaded %s\n",file);
	plugins_mailbox[string(name)] = handle;
	}

if(m != NULL)
	{
	*m = (new_mailbox_t*)dlsym(plugins_mailbox[string(name)],"new_mailbox"); 
	
	if( *m == NULL)
		fprintf(stderr,"Unable to correctly load %s: %s\n",
			file,dlerror());
	}

if(d != NULL)
	{
	*d = (delete_mailbox_t*)dlsym(plugins_mailbox[string(name)],
		"delete_mailbox"); 
	
	if( *d == NULL)
		fprintf(stderr,"Unable to correctly load %s: %s\n",
			file,dlerror());
	}
}

void PluginRegistry::load_notifyer_plugin(const char*name,new_notifyer_t**m,
			delete_notifyer_t**d)
{
const char* pwd = getenv("PWD");
size_t len = 	MAX(strlen(SO_STD),strlen(pwd)+1+strlen(SO_DEVEL)) + 
		18+ // "libebiff_notifyer_"
		strlen(name)+
		4; // ".so\0"

char file[len];
void* handle;

if( plugins_notifyer[string(name)] == NULL)
	{
	snprintf(file,len,"%slibebiff_notifyer_%s.so",SO_STD,name);
	handle = dlopen(file,RTLD_GLOBAL|RTLD_NOW);
	if(handle == NULL)
		{
		snprintf(file,len,"%s/%slibebiff_notifyer_%s.so",
			pwd,SO_DEVEL,name);
		handle = dlopen(file,RTLD_GLOBAL|RTLD_NOW);
		if(handle == NULL)
			{
			fprintf(stderr,"Unable to dlopen %s\n",dlerror());
			return;
			}
		}
	fprintf(stderr,"Loaded %s\n",file);
	plugins_notifyer[string(name)] = handle;
	}

if(m != NULL)
	{
	*m = (new_notifyer_t*)dlsym(plugins_notifyer[string(name)],
		"new_notifyer"); 
	
	if( *m == NULL)
		fprintf(stderr,"Unable to correctly load %s: %s\n",
			file,dlerror());
	}

if(d != NULL)
	{
	*d = (delete_notifyer_t*)dlsym(plugins_notifyer[string(name)],
		"delete_notifyer"); 
	
	if( *d == NULL)
		fprintf(stderr,"Unable to correctly load %s: %s\n",
			file,dlerror());
	}
}

Mailbox* PluginRegistry::mailbox_plugin_new(char* name)
{
new_mailbox_t* n=NULL;

load_mailbox_plugin(name,&n,NULL);

return (n != NULL) ? n() : NULL;
}

Notifyer* PluginRegistry::notifyer_plugin_new(char* name)
{
new_notifyer_t* n=NULL;

load_notifyer_plugin(name,&n,NULL);

return (n != NULL) ? n() : NULL;
}

