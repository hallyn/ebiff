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


#include <list>

#include "relation.h"

class Failure {};

class Scheduler
	{
private:
	list<Relation*> *queue;
public:
	Scheduler(list<Relation*> *l) throw(Failure);
		
	time_t get_next_burst();
	list<Relation*> get_ready(time_t elapsed);
	};
