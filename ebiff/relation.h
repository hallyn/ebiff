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


#ifndef RELATION_H
#define RELATION_H

#include <stdlib.h>

#include "mailbox.h"
#include "notifier.h"

#include <iostream>

class Relation
	{
public:
	Mailbox* box;
	Notifier* nty;
	time_t interval, saved_interval;
	
	Relation();
	Relation(Mailbox* b,Notifier* n,time_t i);
	~Relation();

	static bool Relation_lt(const Relation *r1,const Relation *r2);
	bool operator< (const Relation& r);
	};

#endif
