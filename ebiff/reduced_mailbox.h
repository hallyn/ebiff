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


#ifndef REDUCED_MAILBOX_H
#define REDUCED_MAILBOX_H

#include <string>

#include "mailbox_exception.h"

using namespace std;

class ReducedMailbox // pure virtual class
	{
protected:
	string name;			
	string command;
public:
	virtual unsigned long int CountNew() throw(MailboxException) = 0;
	virtual unsigned long int CountOld() throw(MailboxException) = 0;

	// n higher means more recent
	virtual string GetNewFrom(unsigned long int n) 
			throw(MailboxException) = 0;
	virtual string GetNewSubject(unsigned long int n) 
			throw(MailboxException) = 0;
	virtual string GetNewUid(unsigned long int n) 
			throw(MailboxException) = 0;
	
	// return a mailbox name
	virtual string GetName() throw() = 0;
	virtual string GetCommand() throw() = 0;
	
	virtual ~ReducedMailbox()  throw() {};
	};

#endif
