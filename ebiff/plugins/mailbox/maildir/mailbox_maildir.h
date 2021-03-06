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


#ifndef MAILBOX_MAILDIR_H
#define MAILBOX_MAILDIR_H

#include <string>
#include <vector>

#include <regex.h>

#include "mailbox.h"

using namespace std;

class MailboxMaildir: public Mailbox 
	{
protected:
	void Reset();
	
	void UpdateTimestamps() throw(MailboxException);
	void UpdateCounters() throw(MailboxException);
	unsigned long int CountFiles(string path) throw(MailboxException);

	void ParseMessage(string file,string &subject,string &from) 
			throw(MailboxException);

	class MailInfo 
		{
	public:	
		MailInfo(string sub, string fr, string file) throw();
		MailInfo() throw();
		string GetSubject() throw();
		string GetFrom() throw();
		string GetFilename() throw();
		void Mark() throw();
		void Unmark() throw();
		bool IsMarked() throw();
	protected:		
		string subject;
		string from;
		string filename;
		int marked;
		};

	vector<MailInfo> mails;
	time_t last_modify_new;
	time_t last_modify_old;
	
	unsigned long int n_new;
	unsigned long int n_old;
	
	string path;
	regex_t from_ex,subject_ex;
	
public:
	MailboxMaildir() throw();
	~MailboxMaildir() throw();
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
