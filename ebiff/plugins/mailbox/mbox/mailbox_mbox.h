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


#ifndef MAILBOX_MBOX_H
#define MAILBOX_MBOX_H

#include <string>
#include <vector>

#include <regex.h>

#include "mailbox.h"

using namespace std;

class MailboxMbox: public Mailbox 
	{
protected:
	
	void UpdateTimestamp() throw(MailboxException);

	class MailInfo 
		{
	public:	
		MailInfo(string sub, string fr,string id) throw();
		MailInfo() throw();
		string GetSubject() throw();
		string GetFrom() throw();
		string GetUid() throw();
	protected:		
		string subject;
		string from;
		string uid;
		};

	vector<MailInfo> mails;
	
	time_t last_modify;
	fpos_t end_position;
	string end_string;
	
	unsigned long int n_new;
	unsigned long int n_old;
	
	string path;
	bool newonly;
	
	regex_t from_ex,subject_ex;
	
	bool firsttime;
public:
	MailboxMbox() throw();
	~MailboxMbox() throw();
	void SetString(string name,string value) throw(MailboxException);
	void SetBool(string name,bool value) throw(MailboxException);
	void SetNumber(string name,double value) throw(MailboxException);
	
	bool IsChanged() throw(MailboxException);
	void Sync() throw(MailboxException);
	
	unsigned long int CountNew() throw(MailboxException);
	unsigned long int CountOld() throw(MailboxException);

	string GetNewFrom(unsigned long int n) throw(MailboxException);
	string GetNewSubject(unsigned long int n) throw(MailboxException);
	string GetNewUid(unsigned long int n) throw(MailboxException);
	};

// dlopen interface
extern "C" {
	Mailbox* new_mailbox();
	void delete_mailbox(Mailbox*p);

}

#endif
