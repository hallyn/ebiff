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


#ifndef MAILBOX_H
#define MAILBOX_H

#include <string>

#include "mailbox_exception.h"
#include "reduced_mailbox.h"

using namespace std;

class Mailbox: public ReducedMailbox // pure virtual class
	{
public:
	//! used for config file aruments
	virtual void SetString(string name, string value) 
			throw(MailboxException)= 0;
	virtual void SetBool(string name, bool value) 
			throw(MailboxException) = 0;
	virtual void SetNumber(string name, double value) 
			throw(MailboxException) = 0;

	virtual bool IsChanged() throw(MailboxException) = 0;

	virtual void Sync() throw(MailboxException) = 0;
	
	// return a mailbox name
	string GetName() throw();
	void SetName(string n) throw();
	string GetCommand() throw();
	void SetCommand(string n) throw();
	
	virtual ~Mailbox()  throw() {};
	};

//dlopen stuff
typedef Mailbox* new_mailbox_t();
typedef void delete_mailbox_t(Mailbox*);

#endif
