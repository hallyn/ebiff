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

#include "notifier_stdout.h"

/*******************************************************************************
 *
 *
 */
void NotifierStdout::SetString(string name, string value) throw(NotifierException)
{
}
/*******************************************************************************
 *
 *
 */
void NotifierStdout::SetBool(string name, bool value) throw(NotifierException)
{
if( name == "printall")
	printall = value;

}
/*******************************************************************************
 *
 *
 */
void NotifierStdout::SetNumber(string name, double value) throw(NotifierException)
{

}
		
/*******************************************************************************
 *
 *
 */
void NotifierStdout::Notify(ReducedMailbox* m)
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
NotifierStdout::NotifierStdout()
{
printall = 0;
}

/*******************************************************************************
 *
 *
 */
NotifierStdout::~NotifierStdout()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierStdout();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



