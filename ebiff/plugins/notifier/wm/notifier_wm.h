/******************************************************************************
 * notifier_wm.h by Alessandro Pira
 * This file is part of ebiff (http://ebiff.sf.net)                           *
 * This file is distributed under the terms of GNU GPL license.               *
 ******************************************************************************/

#ifndef NOTIFIER_WM_H
#define NOTIFIER_WM_H


#include "notifier.h"
#include "notifier_exception.h"

#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <string>
#include <queue>

#define CURRENT_IMAGE (image ? pm_yes : pm_no)
#define SWITCH_IMAGE (image = 1 - image)
#define IMG_YES 1
#define IMG_NO  0
#define SET_IMAGE(x) (image = (x))

#define STATUS_NEWID     1
#define STATUS_DELID     2
#define STATUS_NEWMAIL   3
#define STATUS_NOMAIL    4
#define STATUS_UPDATECMD 5

typedef struct {
	unsigned long id;
	int status;
} mbdata;

#define FIND_AND_DO(setvar, index, func) \
	vector<mbdata>::iterator i = (setvar).begin(); \
	while (i != (setvar).end()) \
	{ \
		if (i->id == (index)) \
		{ \
			func; \
			break; \
		} \
		i++; \
	}


class NotifierWm: public Notifier
	{
protected:
	static queue< pair<unsigned long,int> > status;
	static pthread_t wm_thread;
	static pthread_mutex_t status_mutex;

	static void InitWin();
	static void *son_func(void *);

	static Pixmap pm_no, pm_yes;
	static Display *dis;
	static Window win, root, icon;
	static GC gc;

	static unsigned long idcount;

	string command;
	unsigned long id;

public:
	void SetString(string name, string value) throw(NotifierException);
	void SetBool(string name, bool value) throw(NotifierException);
	void SetNumber(string name, double value) throw(NotifierException);

	void Notify(ReducedMailbox* m);

	NotifierWm();
	~NotifierWm();
};

// dlopen interface
extern "C" {
	Notifier* new_notifier();
	void delete_notifier(Notifier*p);
}

#endif
