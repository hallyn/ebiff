/******************************************************************************
 * $Id$
 * This file is part of ebiff (http://ebiff.sf.net)                           *
 * This file is distributed under the terms of GNU GPL license.               *
 ******************************************************************************/

/******************************************************************************
 * File description:
 *	bla bla bla
 * Notes:
 *	blablabla
 * Authors:
 * 	Enrico Tassi <gareuselesinge@users.sourceforge.net>
 ******************************************************************************/


#include "scheduler.h"

#include <stdio.h>

#include <algorithm>

Scheduler::Scheduler(list<Relation*> *l) throw(Failure):
queue(l)
{
if(queue->size() == 0)
	throw Failure();
}

class Min
	{
	private:
		time_t min;
	public:
		Min();
		time_t result();
		void operator() (Relation*&r);

	};

Min::Min() { min = 99999999;}
time_t Min::result() {return min;}
void Min::operator() (Relation*&r) {
min = min < r->interval ? min : r->interval;
}

time_t Scheduler::get_next_burst()
{
return for_each(queue->begin(),queue->end(),Min()).result();
}


class Extract
	{
	private:
		list<Relation*> l;
		time_t e;
	public:
		Extract(time_t elapsed);
		list<Relation*> result();
		void operator() (Relation*r);

	};
Extract::Extract(time_t elapsed) {e = elapsed;}
list<Relation*> Extract::result() {return l;}
void Extract::operator() (Relation*r) {
if ( r->interval <= e )
	{
	l.push_back(r);
	r->interval = r->saved_interval;
	}
else
	{
	r->interval = r->interval - e;
	}
}

list<Relation*> Scheduler::get_ready(time_t elapsed)
{
return for_each(queue->begin(),queue->end(),Extract(elapsed)).result();
}

