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


#ifndef NOTIFIER_H
#define NOTIFIER_H


#include "reduced_mailbox.h"
#include "notifier_exception.h"

class Notifier
	{
public:
	virtual void SetString(string name, string value) 
			throw(NotifierException)= 0;
	virtual void SetBool(string name, bool value) 
			throw(NotifierException) = 0;
	virtual void SetNumber(string name, double value) 
			throw(NotifierException) = 0;
		
	virtual void Notify(ReducedMailbox* m) = 0;
	
	virtual ~Notifier() {};
	};

//dlopen stuff
typedef Notifier* new_notifier_t();
typedef void delete_notifier_t(Notifier*);


#endif
