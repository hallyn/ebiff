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


#ifndef NOTIFYER_FLITE_H
#define NOTIFYER_FLITE_H


#include "notifyer.h"
#include "notifyer_exception.h"

#include <pthread.h>
#include <math.h>

#include <queue>
#include <string>

class NotifyerFlite: public Notifyer
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
	void SetString(string name, string value) throw(NotifyerException);
	void SetBool(string name, bool value) throw(NotifyerException);
	void SetNumber(string name, double value) throw(NotifyerException);
		
	void Notify(ReducedMailbox* m);
	
	NotifyerFlite();
	~NotifyerFlite();
	};

// dlopen interface
extern "C" {
	Notifyer* new_notifyer();
	void delete_notifyer(Notifyer*p);

}

#endif
