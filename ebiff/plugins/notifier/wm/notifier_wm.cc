/******************************************************************************
 * notifier_wm.cc by Alessandro Pira
 * This file is part of ebiff (http://ebiff.sf.net)                           *
 * This file is distributed under the terms of GNU GPL license.               *
 ******************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>
#include <iterator.h>
#include <vector.h>

#include "notifier_wm.h"

#include "nomail.xpm"
#include "newmail.xpm"

Pixmap NotifierWm::pm_no, NotifierWm::pm_yes;
Display *NotifierWm::dis;
Window NotifierWm::win, NotifierWm::root, NotifierWm::icon;
GC NotifierWm::gc;

pthread_t NotifierWm::wm_thread;
pthread_mutex_t NotifierWm::status_mutex;
queue< pair<unsigned long,int> > NotifierWm::status;

unsigned long NotifierWm::idcount=0;

string command;

#if 0
void sigusr1(int i)
{
	// TODO: In a distant future we will receive mailbox data through a
	//       pipe or a socket...
	SET_IMAGE(IMG_YES);
	XCopyArea(dis, CURRENT_IMAGE, icon, gc, 0, 0, 56, 56, 0, 0);
	XFlush(dis);
	signal(SIGUSR1, sigusr1);
}
void sigusr2(int i)
{
	// TODO: In a distant future we will receive mailbox data through a
	//       pipe or a socket...
	SET_IMAGE(IMG_NO);
	XCopyArea(dis, CURRENT_IMAGE, icon, gc, 0, 0, 56, 56, 0, 0);
	XFlush(dis);
	signal(SIGUSR2, sigusr2);
}
void sigchld(int i)
{
	wait(NULL);
	signal(SIGCHLD, sigchld);
}
void sigterm(int i)
{
	XFreeGC(dis,gc);
	XUnmapWindow(dis,icon);
	XUnmapWindow(dis,win);
	XDestroyWindow(dis,icon);
	XDestroyWindow(dis,win);
	XFlush(dis);
	XCloseDisplay(dis);
	exit(0);
}
#endif

void NotifierWm::InitWin()
{
	XpmAttributes at;
	XWMHints wmhints;
	XSetWindowAttributes wat;
	int i;

	// FIXME: Check error all throughout this function
	dis=XOpenDisplay(NULL);
	root=RootWindow(dis, 0);

	bzero((void*)&at,sizeof(XpmAttributes));
	i = XCreatePixmapFromData(dis, root, newmail_xpm, &pm_yes, NULL, &at);
	bzero((void*)&at,sizeof(XpmAttributes));
	i = XCreatePixmapFromData(dis, root, nomail_xpm, &pm_no, NULL, &at);

	wat.background_pixel = BlackPixel(dis, 0);

	win = XCreateWindow(dis, root, 0, 0, 56, 56, 0, CopyFromParent,
	                    InputOutput, CopyFromParent,
	                    CWBackPixel, &wat);
	icon = XCreateWindow(dis, win, 0, 0, 56, 56, 0, CopyFromParent,
	                    InputOutput, CopyFromParent,
	                    CWBackPixel, &wat);

	wmhints.initial_state=WithdrawnState;
	wmhints.icon_window=icon;
	wmhints.window_group=win;
	wmhints.icon_x=0;
	wmhints.icon_y=0;
	wmhints.flags=StateHint|IconPositionHint|WindowGroupHint|IconWindowHint;

	XSetWMHints(dis, win, &wmhints);
	XSelectInput(dis,icon,ExposureMask | ButtonPressMask | StructureNotifyMask);
	XMapWindow(dis, icon);
	XMapWindow(dis, win);
	XFlush(dis);

	gc=XCreateGC(dis, icon, 0, 0);
	XSetForeground(dis, gc, WhitePixel(dis, DefaultScreen(dis)));
	XCopyArea(dis, pm_no, icon, gc, 0, 0, 56, 56, 0, 0);

	XFlush(dis);
}

void *NotifierWm::son_func(void *x)
{
	XEvent event;
	int image = 0;
	vector<mbdata> mailboxes;

	pair<queue < pair<unsigned long,int> > *,pthread_mutex_t* > *p;
	p = (pair< queue< pair<unsigned long,int> > *,pthread_mutex_t* > *) x;
	queue< pair<unsigned long,int> > *status = p->first;
	pthread_mutex_t* status_mutex = p->second;

	InitWin();

	for(;;)
	{
		sleep(1);

		pthread_mutex_lock(status_mutex);
		int size = status->size();
		pthread_mutex_unlock(status_mutex);

		while(size > 0)
		{
			pair<unsigned long,int> packet;
			pthread_mutex_lock(status_mutex);
			packet = status->front();
			status->pop();
			size = status->size();
			pthread_mutex_unlock(status_mutex);

			switch(packet.second)
			{
			case STATUS_NEWID:
				{
					mbdata *d = (mbdata *)malloc(sizeof(mbdata));
					d->id = packet.first;
					d->status = 0;
					mailboxes.push_back(*d);
					free(d);
					break;
				}
			case STATUS_DELID:
				{
					FIND_AND_DO(mailboxes, packet.first, mailboxes.erase(i));
					break;
				}
			case STATUS_NEWMAIL:
				{
					FIND_AND_DO(mailboxes, packet.first, i->status=1);
					break;
				}
			case STATUS_NOMAIL:
				{
					FIND_AND_DO(mailboxes, packet.first, i->status=0);
					break;
				}
			case STATUS_UPDATECMD:
				{
					//TODO: Must write this code
					break;
				}
			}
		}
		if (1)
		{
			vector<mbdata>::iterator i = mailboxes.begin();
			SET_IMAGE(IMG_NO);
			while (i != mailboxes.end())
			{
				if (i->status > 0)
				{
					SET_IMAGE(IMG_YES);
					break;
				}
				i++;
			}
			XCopyArea(dis, CURRENT_IMAGE, icon, gc, 0, 0, 56, 56, 0, 0);
			XFlush(dis);
		}

		while (XEventsQueued(dis,QueuedAlready))
		{
			XNextEvent(dis, &event);
			switch(event.type)
			{
			case Expose:
				XCopyArea(dis, CURRENT_IMAGE, icon, gc, 0, 0, 56, 56, 0, 0);
				XFlush(dis);
				break;
			case ButtonPress:
				{
					// TODO: Launch command
					if (vfork() == 0)
					{
						execlp("xterm", "xterm", "-e", "mutt", NULL);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

/******************************************************************************/

void NotifierWm::SetString(string name, string value) throw(NotifierException)
{
	// TODO: Send to son
	if(name == "command")
		command = value;
}

void NotifierWm::SetBool(string name, bool value) throw(NotifierException)
{
}

void NotifierWm::SetNumber(string name, double value) throw(NotifierException)
{
}

/******************************************************************************/

void NotifierWm::Notify(ReducedMailbox* m)
{
	pair<unsigned long,int> p(id,((m->CountNew()>0) ? STATUS_NEWMAIL:STATUS_NOMAIL ));
	printf("N: %lu\n",m->CountNew());
	pthread_mutex_lock(&status_mutex);
	status.push(p);
	pthread_mutex_unlock(&status_mutex);
}

/******************************************************************************/

NotifierWm::NotifierWm()
{
	id = ++idcount;
	if (id == 0)
	{
		// FIXME:
		//   This is very unlikely, but we have an overflow on idcount
		fprintf(stderr,"Overflow on idcount (NotifierWm class)\n"
		               "...Try to use less mailbox on the wm plugin...\n");
		exit(1);
	}
	command = "";

	if (id == 1)
	{
		XInitThreads();
		pthread_mutex_init(&status_mutex,NULL);
		wm_thread = pthread_create(&wm_thread, NULL, son_func,
		             (void*) new pair< queue<pair<unsigned long,int> > *,
		                               pthread_mutex_t*>
						    (&status,&status_mutex)
		            );
	}

	pthread_mutex_lock(&status_mutex);
	status.push(pair<unsigned long,int>(id,STATUS_NEWID));
	pthread_mutex_unlock(&status_mutex);
}

NotifierWm::~NotifierWm()
{
	pthread_mutex_lock(&status_mutex);
	status.push(pair<unsigned long,int>(id,STATUS_DELID));
	pthread_mutex_unlock(&status_mutex);
}

extern "C" Notifier* new_notifier()
{
	return new NotifierWm();
}

extern "C" void delete_notifier(Notifier*p)
{
	delete p;
}

