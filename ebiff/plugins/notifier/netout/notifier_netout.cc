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

#include <iostream>
#include <sstream>

#include "notifier_netout.h"

/*******************************************************************************
 *
 *
 */
void NotifierNetout::SetString(string name, string value) throw(NotifierException)
{
}
/*******************************************************************************
 *
 *
 */
void NotifierNetout::SetBool(string name, bool value) throw(NotifierException)
{

}
/*******************************************************************************
 *
 *
 */
void NotifierNetout::SetNumber(string name, double value) throw(NotifierException)
{

}
		
/*******************************************************************************
 *
 *
 */
void NotifierNetout::Notify(ReducedMailbox* m)
{
string out;
unsigned long n =  m->CountNew();
char tmpn[20],tmpo[20];

snprintf(tmpn,20,"%ld",n);
snprintf(tmpo,20,"%ld",m->CountOld() );

cout << "---------------------\n";

out = m->GetName() + "\r\n" +
 m->GetCommand() + "\r\n" +
 string(tmpn) + " " + string(tmpo) + "\r\n"; 

for(long unsigned int i=0 ; i < n ; i++)
	out  = out +  m->GetNewFrom(i) + "\r\n" +
		m->GetNewSubject(i) + "\r\n" +
		m->GetNewUid(i) + "\r\n";

out = out + "\r\n";

cout << out;
cout << "---------------------\n";
}
	
/*******************************************************************************
 *
 *
 */
NotifierNetout::NotifierNetout()
{
printall = 0;
}

/*******************************************************************************
 *
 *
 */
NotifierNetout::~NotifierNetout()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierNetout();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



