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


#ifndef NOTIFIER_EXCEPTION_H
#define NOTIFIER_EXCEPTION_H

#include <string>

using namespace std;

class NotifierException 
	{
public:
	NotifierException(string message);	

	string msg;		
	};

#endif
