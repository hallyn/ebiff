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


#ifndef NOTIFYER_H
#define NOTIFYER_H


#include "reduced_mailbox.h"
#include "notifyer_exception.h"

class Notifyer
	{
public:
	virtual void SetString(string name, string value) 
			throw(NotifyerException)= 0;
	virtual void SetBool(string name, bool value) 
			throw(NotifyerException) = 0;
	virtual void SetNumber(string name, double value) 
			throw(NotifyerException) = 0;
		
	virtual void Notify(ReducedMailbox* m) = 0;
	
	virtual ~Notifyer() {};
	};

//dlopen stuff
typedef Notifyer* new_notifyer_t();
typedef void delete_notifyer_t(Notifyer*);


#endif
