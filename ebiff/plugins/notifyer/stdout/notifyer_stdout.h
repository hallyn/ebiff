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


#ifndef NOTIFYER_STDOUT_H
#define NOTIFYER_STDOUT_H


#include "notifyer.h"
#include "notifyer_exception.h"

class NotifyerStdout: public Notifyer
	{
protected:
	int printall;
public:
	void SetString(string name, string value) throw(NotifyerException);
	void SetBool(string name, bool value) throw(NotifyerException);
	void SetNumber(string name, double value) throw(NotifyerException);
		
	void Notify(ReducedMailbox* m);
	
	NotifyerStdout();
	~NotifyerStdout();
	};

// dlopen interface
extern "C" {
	Notifyer* new_notifyer();
	void delete_notifyer(Notifyer*p);

}

#endif
