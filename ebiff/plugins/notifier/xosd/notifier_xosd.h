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


#ifndef NOTIFIER_XOSD_H
#define NOTIFIER_XOSD_H


#include "notifier.h"
#include "notifier_exception.h"

#include <xosd.h>
#include <pthread.h>
#include <math.h>

#include <queue>
#include <string>

class NotifierXosd: public Notifier
	{
protected:
	queue< pair<xosd*,string> > display;
	pthread_t manager;
	pthread_attr_t manager_att;
	pthread_mutex_t display_mutex;
	pthread_mutexattr_t display_mutex_att;
	static void *manager_f(void*);

	int hoffset;
        int outlineoffset;
        int shadowoffset;
        int timeout;
        int voffset;
        string color;
        string font;
        string outlinecolor;
        xosd_align align;
        xosd_pos pos;
	
public:
	void SetString(string name, string value) throw(NotifierException);
	void SetBool(string name, bool value) throw(NotifierException);
	void SetNumber(string name, double value) throw(NotifierException);
		
	void Notify(ReducedMailbox* m);
	
	NotifierXosd();
	~NotifierXosd();
	};

// dlopen interface
extern "C" {
	Notifier* new_notifier();
	void delete_notifier(Notifier*p);

}

#endif
