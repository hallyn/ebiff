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


#ifndef NOTIFYER_EXCEPTION_H
#define NOTIFYER_EXCEPTION_H

#include <string>

using namespace std;

class NotifyerException 
	{
public:
	NotifyerException(string message);	

	string msg;		
	};

#endif
