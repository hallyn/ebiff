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


#ifndef NOTIFIER_NETOUT_H
#define NOTIFIER_NETOUT_H

#include <queue>

#include "notifier.h"
#include "notifier_exception.h"

class NotifierNetout: public Notifier
	{
protected:

	pthread_t manager;
	pthread_attr_t manager_att;
	pthread_mutex_t queue_mutex;
	pthread_mutexattr_t queue_mutex_att;
	
	static void *manager_f(void*);

	class thread_param
		{
	public:
		string pass;
		unsigned int port;
		
		int ready;
		queue<string> outgoing;
		};
	
	thread_param param;
		
public:
	void SetString(string name, string value) throw(NotifierException);
	void SetBool(string name, bool value) throw(NotifierException);
	void SetNumber(string name, double value) throw(NotifierException);
		
	void Notify(ReducedMailbox* m);
	
	NotifierNetout();
	~NotifierNetout();
	};

// dlopen interface
extern "C" {
	Notifier* new_notifier();
	void delete_notifier(Notifier*p);

}

#endif
