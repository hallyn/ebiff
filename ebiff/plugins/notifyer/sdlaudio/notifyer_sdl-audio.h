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

class NotifyerSDLaudio: public Notifyer
	{
protected:
	static queue< string > files;
	
	static pthread_t manager;
	static pthread_attr_t manager_att;
	static pthread_mutex_t files_mutex;
	static pthread_mutexattr_t files_mutex_att;
	
	static void *manager_f(void*);

	static void fill_audio(void *udata, Uint8 *stream, int len);

	string file;
	
public:
	void SetString(string name, string value) throw(NotifyerException);
	void SetBool(string name, bool value) throw(NotifyerException);
	void SetNumber(string name, double value) throw(NotifyerException);
		
	void Notify(ReducedMailbox* m);
	
	NotifyerSDLaudio();
	~NotifyerSDLaudio();
	};

// dlopen interface
extern "C" {
	Notifyer* new_notifyer();
	void delete_notifyer(Notifyer*p);

}

#endif
