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

	// this data duplication is orrible, but avoids concurrency problems.
	class WInfo
		{
		public:
		class Box
			{
			public:
			class Mail
				{
				public:
				string From,Subject,Uid;
				Mail(string From,string Subject,string Uid);
				};
			list<Mail> content;
			Box(ReducedMailbox*m);
			Box();
			};
		map<ReducedMailbox*,Box> boxes;
		GtkWidget *window;
		string position;
		bool showall;
		bool preview;
		};
	
	WInfo status;
	WInfo* UpdateStatus(ReducedMailbox* m);
	
	static queue<WInfo*> requests;
	static GThread* refresher;
	static GStaticMutex requests_mutex;
	static gpointer thread(gpointer data);
	static void UpdateWindow(WInfo* i);
	static void PositionWindow(WInfo* i);
	static GtkWidget* CreatePreview(WInfo*i,
		map<ReducedMailbox*,NotifyerGtk2::WInfo::Box>::iterator x);
	static gboolean main_quit(gpointer);
	static void execute(GtkButton *button,gpointer user_data);
	static void show(GtkButton *button,gpointer user_data);
	static bool hide(GtkWidget *w,GdkEvent *event,gpointer user_data);
	static bool hide_and_toggle(GtkWidget *w,
			GdkEvent *event,gpointer user_data);
	static bool destroy(GtkWidget *w,gpointer user_data);
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
