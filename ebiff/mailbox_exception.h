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


#ifndef MAILBOX_EXCEPTION_H
#define MAILBOX_EXCEPTION_H


#include <string>

using namespace std;

class MailboxException 
	{
public:
	MailboxException(string message);	

	string msg;		
	};

#endif
