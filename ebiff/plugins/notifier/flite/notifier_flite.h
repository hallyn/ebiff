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


#ifndef NOTIFIER_FLITE_H
#define NOTIFIER_FLITE_H


#include "notifier.h"
#include "notifier_exception.h"

#include <pthread.h>
#include <math.h>

#include <queue>
#include <string>

class NotifierFlite: public Notifier
	{
protected:
	queue< string > talks;
	pthread_t manager;
	pthread_attr_t manager_att;
	pthread_mutex_t display_mutex;
	pthread_mutexattr_t display_mutex_att;
	static void *manager_f(void*);

	string one,zero,more;

	string replace_metas(string s,ReducedMailbox* m);
	
public:
	void SetString(string name, string value) throw(NotifierException);
	void SetBool(string name, bool value) throw(NotifierException);
	void SetNumber(string name, double value) throw(NotifierException);
		
	void Notify(ReducedMailbox* m);
	
	NotifierFlite();
	~NotifierFlite();
	};

// dlopen interface
extern "C" {
	Notifier* new_notifier();
	void delete_notifier(Notifier*p);

}

#endif
