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
	
	class MailInfo 
		{
	public:	
		MailInfo(string sub, string fr, string file) throw();
		MailInfo() throw();
		string subject;
		string from;
		string filename;
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

	class MailBoxInfo 
		{
	public:
		MailBoxInfo()throw();
		string name,command;
		unsigned long int n_new,n_old;
		vector<MailInfo> mails;
		};

	queue<MailBoxInfo> infos;
	
	MailBoxInfo current;
		
	string buffer;
	
	pthread_t manager;
	pthread_attr_t manager_att;
	pthread_mutex_t queue_mutex;
	pthread_mutexattr_t queue_mutex_att;
	
	static void *manager_f(void*);

	NetInfo netinfo;

	queue<string> incoming;

	MailBoxInfo parse(string);
	
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

	virtual string GetName()throw();;
	virtual string GetCommand()throw();;
	};

// dlopen interface
extern "C" {
	Mailbox* new_mailbox();
	void delete_mailbox(Mailbox*p);

}

#endif
