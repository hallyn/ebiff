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

#include "notifyer_stdout.h"

/*******************************************************************************
 *
 *
 */
void NotifyerStdout::SetString(string name, string value) throw(NotifyerException)
{
}
/*******************************************************************************
 *
 *
 */
void NotifyerStdout::SetBool(string name, bool value) throw(NotifyerException)
{
if( name == "printall")
	printall = value;

}
/*******************************************************************************
 *
 *
 */
void NotifyerStdout::SetNumber(string name, double value) throw(NotifyerException)
{

}
		
/*******************************************************************************
 *
 *
 */
void NotifyerStdout::Notify(ReducedMailbox* m)
{
long unsigned int n = m->CountNew();

cout << "New mail in mailbox " << m->GetName();
if(printall != 0)
	cout << ":\n";
else
	cout << endl;

if(printall != 0)
	for(long unsigned int i=0 ; i < n ; i++)
		cout << "  " << i << " From: " << m->GetNewFrom(i) << 
			" Subject: " << m->GetNewSubject(i)<<endl ;

}
	
/*******************************************************************************
 *
 *
 */
NotifyerStdout::NotifyerStdout()
{
printall = 0;
}

/*******************************************************************************
 *
 *
 */
NotifyerStdout::~NotifyerStdout()
{

}

extern "C" Notifyer* new_notifyer(){
return new NotifyerStdout();
}

extern "C" void delete_notifyer(Notifyer*p){
delete p;
}



