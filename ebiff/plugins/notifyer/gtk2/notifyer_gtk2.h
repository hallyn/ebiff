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


#ifndef NOTIFYER_STDOUT_H
#define NOTIFYER_STDOUT_H


#include "notifyer.h"
#include "notifyer_exception.h"

#include <gtk/gtk.h>

#include <list>
#include <queue>
#include <string>
#include <map>

class NotifyerGtk2: public Notifyer
	{
protected:

	// Plugin staus
	class Status {
		public:
			
		// things that must be kept of a mailbox
		class BoxInfo {
			public:
			int position;

			BoxInfo();
		};
		
		
		// status variables
		map<ReducedMailbox*,BoxInfo> boxes;
		GtkWidget *window;
		string position;
		bool showall;
		bool preview;
		};

	// the data structure passed to the thread to represent mailbox status
	class Box {
		public:
		
		class Mail {
			public:
			string From,Subject,Uid;
			Mail(string From,string Subject,string Uid);
		};
	
		list<Mail> content;

		string name;
		string command;

		Box(ReducedMailbox*m);
	};

	// Update Info for the Drawing Thread
	class UInfo {
		public:
		// the content
		Box* mailbox;
		
		// the position in the table
		Status::BoxInfo &info;
			
		// the root widget
		GtkWidget *window;

		// some options
		string position;
		bool showall;
		bool preview;

		UInfo(Box*,Status::BoxInfo&,GtkWidget *,string,bool,bool);
		~UInfo();
	};
	
	// The main status
	Status status;
	
	// create the object to enqueue for updation
	UInfo* UpdateStatus(ReducedMailbox* m);
	
	// Global datas
	static queue<UInfo*> requests;
	static GThread* refresher;
	static GStaticMutex requests_mutex;
	
	// functions for the thread
	static gpointer thread(gpointer data);
	static void UpdateWindow(UInfo* i);
	static void PositionWindow(UInfo* i);
	static GtkWidget* CreatePreview(UInfo*i,NotifyerGtk2::Box *x);

	// gtk thread stuff
	static gboolean main_quit(gpointer);
	static void execute(GtkButton *button,gpointer user_data);
	static void show(GtkButton *button,gpointer user_data);
	static bool hide(GtkWidget *w,GdkEvent *event,gpointer user_data);
	static bool hide_and_toggle(GtkWidget *w,
			GdkEvent *event,gpointer user_data);
	static bool destroy(GtkWidget *w,gpointer user_data);
	static bool free(GtkWidget *w,gpointer user_data);
public:
	void SetString(string name, string value) throw(NotifyerException);
	void SetBool(string name, bool value) throw(NotifyerException);
	void SetNumber(string name, double value) throw(NotifyerException);
		
	void Notify(ReducedMailbox* m);
	
	NotifyerGtk2();
	~NotifyerGtk2();
	};

// dlopen interface
extern "C" {
	Notifyer* new_notifyer();
	void delete_notifyer(Notifyer*p);

}

#endif
