/******************************************************************************
 * $Id$
 * This file is part of ebiff (http://ebiff.sf.net)                           *
 * This file is distributed under the terms of GNU GPL license.               *
 ******************************************************************************/


/******************************************************************************/
 /*!
  * \file   filename.h
  * \brief  short description
  * eventual long description
  * \author Enrico Tassi <gareuselesinge@users.sourceforge.net>
  */
/******************************************************************************/


#ifndef PLUGINREGISTRY_H
#define PLUGINREGISTRY_H

#include <map>
#include <mailbox.h>
#include <notifier.h>

class PluginRegistry
	{
private:	
	static map<string,void*> plugins_mailbox;
	static map<string,void*> plugins_notifier;
	static void load_mailbox_plugin(const char*name,new_mailbox_t**m,
			delete_mailbox_t**d);
	static void load_notifier_plugin(const char*name,new_notifier_t**m,
			delete_notifier_t**d);

public:
	static Mailbox* mailbox_plugin_new(char* name);
	static Notifier* notifier_plugin_new(char* name);
	};

#endif
