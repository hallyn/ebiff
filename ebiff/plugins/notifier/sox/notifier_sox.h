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

class NotifierSox: public Notifier
	{
protected:
	static queue< string > files;
	
	static pthread_t manager;
	static pthread_attr_t manager_att;
	static pthread_mutex_t files_mutex;
	static pthread_mutexattr_t files_mutex_att;
	
	static void *manager_f(void*);

	string file;
	string player;
	string file_empty;
	
public:
	void SetString(string name, string value) throw(NotifierException);
	void SetBool(string name, bool value) throw(NotifierException);
	void SetNumber(string name, double value) throw(NotifierException);
		
	void Notify(ReducedMailbox* m);
	
	NotifierSox();
	~NotifierSox();
	};

// dlopen interface
extern "C" {
	Notifier* new_notifier();
	void delete_notifier(Notifier*p);

}

#endif
