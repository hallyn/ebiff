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


#include <map>
#include <list>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "lposix.h"
}

#include <assert.h>

#include "relation.h"
#include "mailbox.h"
#include "notifier.h"
#include "pluginregistry.h"

#include "parser.c"

#define _RELEASE_

#ifndef _RELEASE_
void lua_printstack(lua_State* s)	
{
int i;
for(i=lua_gettop(s) ; i > 0 ; i-- )
	{
	printf("LUA_STACK(%2d) : %s",i,
		lua_typename(s,lua_type(s,i)));
	if(lua_isstring(s,i))
		printf(" \"%s\"\n",lua_tostring(s,i));
	else
		printf("\n");
	}
printf("LUA_STACK( 0) : --bottom--\n");
}
#else
void lua_printstack(lua_State* s)	
{
int i=lua_gettop(s);
if(lua_isstring(s,i))
	printf("ERROR: \"%s\"\n",lua_tostring(s,i));
}
#endif

#define lua_emptystack(s) {\
	while(lua_gettop(s) != 0)\
	lua_pop(s,1);\
	}

list<Relation*> *parse_config(char* file)
{
int rc;
lua_State* L = lua_open();
map<double,Mailbox*> m_mailbox;
map<double,Notifier*> m_notifier;
list<Relation*> *relations = new list<Relation*>();

luaopen_base(L);
luaopen_table(L);
luaopen_string(L);
luaopen_io(L);

luaopen_posix(L);

lua_emptystack(L);

#include "tableex.c"
#include "grammar.c"
#include "typecheck.c"

rc = luaL_loadfile(L,file);
if (rc != 0)
	lua_printstack(L);

rc = lua_pcall(L, 0, 0, 0);
if (rc != 0)
	lua_printstack(L);

lua_pushstring(L,"Relation_Table");
lua_gettable(L,LUA_GLOBALSINDEX);

lua_pushnil(L);
while (lua_next(L,-2) != 0)
	{
	relation_bag_t b;
	int rc;
	rc = relation_hook(L,&b);

	if(rc != 0)
		{
		fprintf(stderr,"Unable to parse the config file.\n");
		lua_printstack(L);
		break;
		}

	if( m_mailbox[MAILBOX_CAST(&b).id] == NULL)
		{
		Mailbox* m = NULL;

		switch(MAILBOX_CAST(&b).driver_content)
			{
			case MAILBOX_DRIVER_CONTENT_MAILDIR: {
				m = PluginRegistry::
					mailbox_plugin_new("maildir");
				if(m==NULL)
					break;
				
				m->SetString("path",MAILDIR_CAST(&b).path);
				
				}break;
			case MAILBOX_DRIVER_CONTENT_NETIN: {
				m = PluginRegistry::
					mailbox_plugin_new("netin");
				if(m==NULL)
					break;
				
				m->SetString("host",NETIN_CAST(&b).host);
				m->SetString("pass",NETIN_CAST(&b).pass);
				m->SetNumber("port",NETIN_CAST(&b).port);
				
				}break;				     
			case MAILBOX_DRIVER_CONTENT_MBOX:{
				m = PluginRegistry::
					mailbox_plugin_new("mbox");
				if(m==NULL)
					break;
				
				m->SetString("path",MBOX_CAST(&b).path);
				m->SetBool("newonly",MBOX_CAST(&b).newonly);
				
				}break;
			}
		m_mailbox[MAILBOX_CAST(&b).id] = m;
		}

	if( m_mailbox[MAILBOX_CAST(&b).id] == NULL)
		{
		fprintf(stderr,"Unable to load requested plugin\n");
		lua_pop(L,1);
		continue;
		}
	
	m_mailbox[MAILBOX_CAST(&b).id]->SetName(MAILBOX_CAST(&b).name);
	m_mailbox[MAILBOX_CAST(&b).id]->SetCommand(MAILBOX_CAST(&b).command);
	
	if( m_notifier[NOTIFIER_CAST(&b).id] == NULL)
		{

		Notifier* n=NULL;
	
		switch(NOTIFIER_CAST(&b).driver_content)
			{
			case NOTIFIER_DRIVER_CONTENT_STDOUT: {
				n=PluginRegistry::notifier_plugin_new("stdout");
				if(n==NULL)
					break;

				n->SetBool("printall",STDOUT_CAST(&b).printall);
				
				}break;
			case NOTIFIER_DRIVER_CONTENT_UTMP: {
				n=PluginRegistry::notifier_plugin_new("utmp");
				if(n==NULL)
					break;

				n->SetBool("printall",UTMP_CAST(&b).printall);
				
				}break;
			case NOTIFIER_DRIVER_CONTENT_NETOUT: {
				n=PluginRegistry::notifier_plugin_new("netout");
				if(n==NULL)
					break;

				n->SetString("pass",NETOUT_CAST(&b).pass);
				n->SetNumber("port",NETOUT_CAST(&b).port);		
				}break;		
			case NOTIFIER_DRIVER_CONTENT_SOX: {
				n=PluginRegistry::
					notifier_plugin_new("sox");
				if(n==NULL)
					break;

				n->SetString("file",SOX_CAST(&b).file);
				n->SetString("file_empty",
					SOX_CAST(&b).file_empty);
				
				}break;
			case NOTIFIER_DRIVER_CONTENT_FLITE: {
				n=PluginRegistry::notifier_plugin_new("flite");
				if(n==NULL)
					break;

				n->SetString("one",FLITE_CAST(&b).one);
				n->SetString("more",FLITE_CAST(&b).more);
				n->SetString("zero",FLITE_CAST(&b).zero);

				}break;				     
			case NOTIFIER_DRIVER_CONTENT_GTK2: {
				n = PluginRegistry::notifier_plugin_new("gtk2");
				if(n==NULL)
					break;

				n->SetString("position",GTK2_CAST(&b).position);
				n->SetBool("showall",GTK2_CAST(&b).showall);
				n->SetBool("preview",GTK2_CAST(&b).preview);
				
				}break;
			case NOTIFIER_DRIVER_CONTENT_XOSD: {
				n = PluginRegistry::notifier_plugin_new("xosd");
				if(n==NULL)
					break;

				n->SetString("align",XOSD_CAST(&b).align);
				n->SetString("pos",XOSD_CAST(&b).pos);
				n->SetNumber("voffset",XOSD_CAST(&b).voffset);
				n->SetNumber("hoffset",XOSD_CAST(&b).hoffset);
				n->SetNumber("timeout",XOSD_CAST(&b).timeout);
				n->SetString("color",XOSD_CAST(&b).color);
				n->SetNumber("shoadowoffset",
					XOSD_CAST(&b).shoadowoffset);
				n->SetNumber("outlineoffset",
					XOSD_CAST(&b).outlineoffset);
				n->SetString("outlinecolor",
					XOSD_CAST(&b).outlinecolor);
				n->SetString("font",XOSD_CAST(&b).font);
				
				}break;
				   
			}
		m_notifier[NOTIFIER_CAST(&b).id] = n;
		}

	if(m_notifier[NOTIFIER_CAST(&b).id] == NULL)
		{
		// FIX consiuder freeing the mailbox plugin,
		// but remember that you must use the dlopen hack,
		// since virtual destructor doesn't work.
		fprintf(stderr,"Unable to load requested plugin\n");
		lua_pop(L,1);
		continue;
		}
	
	relations->push_back(new Relation(m_mailbox[MAILBOX_CAST(&b).id],
			m_notifier[NOTIFIER_CAST(&b).id],
			(time_t)MAILBOX_CAST(&b).interval));
	
	lua_pop(L,1);
	}

lua_close(L);

return relations;
}
