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


#ifndef MAILBOX_NETIN_H
#define MAILBOX_NETIN_H

#include <string>
#include <vector>
#include <queue>

#include <regex.h>
#include <pthread.h>

#include "mailbox.h"

using namespace std;

class MailboxNetin: public Mailbox 
	{
protected:
	
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

	class NetInfo
		{
	public:		
		unsigned int port;
		string host;
		string pass;
		int ready;
		NetInfo();
		};

	vector<MailInfo> mails;
		
	unsigned long int n_new;
	unsigned long int n_old;

	string buffer;
	
	pthread_t manager;
	pthread_attr_t manager_att;
	pthread_mutex_t queue_mutex;
	pthread_mutexattr_t queue_mutex_att;
	
	static void *manager_f(void*);

	NetInfo netinfo;

	queue<string> incoming;

public:
	MailboxNetin() throw();
	~MailboxNetin() throw();
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
