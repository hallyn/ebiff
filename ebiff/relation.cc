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


#include <relation.h>

/*******************************************************************************
 *
 *
 */ 
Relation::Relation()
{
box = NULL;
nty = NULL;
interval = 0;
}

/*******************************************************************************
 *
 *
 */ 
Relation::Relation(Mailbox* b,Notifier* n,time_t i)
{
box = b;
nty = n;
saved_interval = interval = i;
}

/*******************************************************************************
 *
 *
 */ 
Relation::~Relation()
{

}

bool Relation::Relation_lt(const Relation *r1,const Relation* r2)
{
return r1->interval < r2->interval;
}


bool Relation::operator< (const Relation& r)
{
return interval < r.interval; 
}
