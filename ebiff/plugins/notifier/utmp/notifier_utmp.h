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


#ifndef NOTIFIER_UTMP_H
#define NOTIFIER_UTMP_H


#include "notifier.h"
#include "notifier_exception.h"

class NotifierUtmp: public Notifier
	{
protected:
	int printall;
public:
	void SetString(string name, string value) throw(NotifierException);
	void SetBool(string name, bool value) throw(NotifierException);
	void SetNumber(string name, double value) throw(NotifierException);
		
	void Notify(ReducedMailbox* m);
	
	NotifierUtmp();
	~NotifierUtmp();
	};

// dlopen interface
extern "C" {
	Notifier* new_notifier();
	void delete_notifier(Notifier*p);

}

#endif
