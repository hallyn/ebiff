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


#include "mailbox.h"

void Mailbox::SetName(string s) throw() {name = s;}
string Mailbox::GetName() throw() {return name;}
void Mailbox::SetCommand(string s) throw() {command = s;}
string Mailbox::GetCommand() throw() {return command;}

