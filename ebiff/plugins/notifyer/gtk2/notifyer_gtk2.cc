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

#include "notifyer_gtk2.h"

GThread* NotifyerGtk2::refresher = NULL;
queue<NotifyerGtk2::WInfo*> NotifyerGtk2::requests;
GStaticMutex NotifyerGtk2::requests_mutex = G_STATIC_MUTEX_INIT;

/*******************************************************************************
 *
 *
 */
void NotifyerGtk2::SetString(string name, string value) throw(NotifyerException)
{
if( name == string("position"))
	status.position = value;
}
/*******************************************************************************
 *
 *
 */
void NotifyerGtk2::SetBool(string name, bool value) throw(NotifyerException)
{
if( name == string("showall"))
	status.showall = value;
if( name == string("preview"))
	status.preview = value;
}
/*******************************************************************************
 *
 *
 */
void NotifyerGtk2::SetNumber(string name, double value) throw(NotifyerException)
{

}
		
/*******************************************************************************
 *
 *
 */
void NotifyerGtk2::Notify(ReducedMailbox* m)
{
if( status.window == NULL)
	{
	if( status.position != string("managed"))
		status.window = gtk_window_new(GTK_WINDOW_POPUP);
	else
		status.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_wmclass(GTK_WINDOW(status.window),
		"ebiff_gtk2","ebiff_gtk2");
	gtk_window_set_resizable(GTK_WINDOW(status.window),FALSE);
	g_signal_connect(status.window,
		"delete-event",G_CALLBACK(gtk_true),NULL);
	}

	
g_static_mutex_lock(&requests_mutex);
requests.push(UpdateStatus(m));
g_static_mutex_unlock(&requests_mutex);
}

/*******************************************************************************
 *
 *
 */
NotifyerGtk2::NotifyerGtk2()
{
int argn=1;
char* v[1] = {"ebiff"};
char** argv=(char**)v;

gtk_init_check(&argn,&argv);
status.window = NULL;
status.showall = false;
status.preview = false;
if(refresher == NULL)
	{
	if (!g_thread_supported ()) 
		g_thread_init (NULL);
	refresher = g_thread_create(thread,NULL,0,NULL);
	}
}

/*******************************************************************************
 *
 *
 */
NotifyerGtk2::WInfo::Box::Mail::Mail(string F,string S,string U)
{
From = F;
Subject = S;
Uid = U;
}

/*******************************************************************************
 *
 *
 */
NotifyerGtk2::WInfo* NotifyerGtk2::UpdateStatus(ReducedMailbox* m)
{
status.boxes[m] = NotifyerGtk2::WInfo::Box(m);

// copy current status on i
WInfo* i = new WInfo(status);

return i;
}

/*******************************************************************************
 *
 *
 */
NotifyerGtk2::WInfo::Box::Box(ReducedMailbox*m)
{
long unsigned int n = m->CountNew();

for(long unsigned int i=0 ; i < n ; i++)
	content.push_back(
		Mail(m->GetNewFrom(i),m->GetNewSubject(i),m->GetNewUid(i)));
}

/*******************************************************************************
 *
 *
 */
NotifyerGtk2::WInfo::Box::Box() {};

gboolean NotifyerGtk2::main_quit(gpointer)
{
if(gtk_main_level() > 0)
	gtk_main_quit();

return TRUE;
}

/*******************************************************************************
 *
 *
 */
void NotifyerGtk2::PositionWindow(WInfo* i)
{
GtkRequisition	req;
int wid = gdk_screen_width();
int hei = gdk_screen_height();
gtk_widget_size_request(i->window,&req);
		
if( i->position == string("top"))
	gtk_widget_set_uposition(
		i->window,
		(wid-req.width)/2,0);
else if ( i->position == string("bottom"))
	gtk_widget_set_uposition(
		i->window,(wid-req.width)/2,
		hei-req.height);
else if ( i->position == string("left"))
	gtk_widget_set_uposition(i->window,
		0,
		(hei-req.height)/2);
else if ( i->position == string("right"))
	gtk_widget_set_uposition(i->window,
		wid-req.width,
		(hei-req.height)/2);
else if ( i->position == string("topleft"))
	gtk_widget_set_uposition(i->window,
		0,
		0);
else if ( i->position == string("topright"))
	gtk_widget_set_uposition(i->window,
		wid-req.width,
		0);
else if ( i->position == string("bottomleft"))
	gtk_widget_set_uposition(i->window,
		0,
		hei-req.height);
else if ( i->position == string("bottomright"))
	gtk_widget_set_uposition(i->window,
		wid-req.width,
		hei-req.height);

}

/*******************************************************************************
 *
 *
 */
gpointer NotifyerGtk2::thread (gpointer p)
{
g_timeout_add(500,NotifyerGtk2::main_quit,NULL);
while(true)
	{
	gtk_main();
		
	//get the size
	g_static_mutex_lock(&requests_mutex); //++
	int size = requests.size();
	g_static_mutex_unlock(&requests_mutex); //--
	
	while( size > 0 )
		{
		// etract the request
		g_static_mutex_lock(&requests_mutex); //++
		WInfo* i = requests.front();
		requests.pop();
		g_static_mutex_unlock(&requests_mutex); //--
		
		size--;
		
		// clean old events
		//gtk_widget_hide(i->window);
		gtk_widget_set_sensitive(i->window,FALSE);
		while (gtk_events_pending())
			gtk_main_iteration();
		
		// do the work
		UpdateWindow(i);
		
		// position it
		PositionWindow(i);
				
		gtk_widget_set_sensitive(i->window,TRUE);
		gtk_widget_show(i->window);
		}
	}
return NULL;
}

/*******************************************************************************
 *
 *
 */
void NotifyerGtk2::execute(GtkButton *button,gpointer user_data)
{
system((char*)user_data);
}
/*******************************************************************************
 *
 *
 */
void NotifyerGtk2::show(GtkButton *button,gpointer user_data)
{
if(GTK_WIDGET_VISIBLE(GTK_WIDGET(user_data)))
	gtk_widget_hide(GTK_WIDGET(user_data));
else
	gtk_widget_show(GTK_WIDGET(user_data));
}
/*******************************************************************************
 *
 *
 */

bool NotifyerGtk2::hide(GtkWidget *w,GdkEvent *event,gpointer user_data)
{
gtk_widget_hide(GTK_WIDGET(w));
return TRUE;
}
/*******************************************************************************
 *
 *
 */
bool  NotifyerGtk2::hide_and_toggle(GtkWidget *w,
		GdkEvent *event,gpointer user_data)
{
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(user_data),FALSE);
return TRUE;
}
/*******************************************************************************
 *
 *
 */

GtkWidget* NotifyerGtk2::CreatePreview(WInfo*i,
	map<ReducedMailbox*,NotifyerGtk2::WInfo::Box>::iterator x)
{
GtkWidget* ret;
char str[5];
snprintf(str,5,"%u",x->second.content.size());
	
if (!i->preview)
	ret = gtk_label_new(str);
else
	{
	GtkWidget* w1,*l1,*l2,*l3,*tbl,*s1,*s2,*s3,*scrolledwindow1,*viewport1;
	unsigned long int J=x->second.content.size();
	unsigned long int j=2;

	ret = gtk_toggle_button_new_with_label(str);
	
	w1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(w1),GTK_WIN_POS_CENTER_ALWAYS);
	//g_signal_connect(w1,"delete-event",G_CALLBACK(gtk_true),NULL);
	g_signal_connect(w1,"delete-event",G_CALLBACK(hide_and_toggle),ret);
	gtk_widget_set_size_request (w1, 530, 230);
	
	g_signal_connect(ret,"clicked",G_CALLBACK(NotifyerGtk2::show),w1);
	g_signal_connect(ret,"destroy",G_CALLBACK(NotifyerGtk2::destroy),w1);
	
	tbl=gtk_table_new(5,x->second.content.size()+2,FALSE);
		
	l1 = gtk_label_new("#");
	l2 = gtk_label_new("From");
	l3 = gtk_label_new("Subject");
	
	gtk_label_set_markup(GTK_LABEL(l1),"<b>#</b>");
	gtk_label_set_markup(GTK_LABEL(l2),"<b>From</b>");
	gtk_label_set_markup(GTK_LABEL(l3),"<b>Subject</b>");
	
	gtk_table_attach(GTK_TABLE(tbl),l1,0,1,0,1,GTK_FILL,GTK_SHRINK,2,0);
	gtk_table_attach(GTK_TABLE(tbl),l2,2,3,0,1,GTK_FILL,GTK_SHRINK,2,0);
	gtk_table_attach(GTK_TABLE(tbl),l3,4,5,0,1,GTK_FILL,GTK_SHRINK,2,0);
	
	gtk_misc_set_alignment (GTK_MISC (l1), 0.5, 0.5);
	gtk_misc_set_alignment (GTK_MISC (l2), 0.0, 0.5);
	gtk_misc_set_alignment (GTK_MISC (l3), 0.0, 0.5);
	
	gtk_widget_show(l1);
	gtk_widget_show(l2);
	gtk_widget_show(l3);
	
	s1 = gtk_hseparator_new();
	s2 = gtk_vseparator_new();
	s3 = gtk_vseparator_new();
	
	gtk_table_attach(GTK_TABLE(tbl),s1,0,5,1,2,GTK_FILL,GTK_SHRINK,0,0);
	gtk_table_attach(GTK_TABLE(tbl),s2,1,2,0,J+2,GTK_SHRINK,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(tbl),s3,3,4,0,J+2,GTK_SHRINK,GTK_FILL,0,0);

	gtk_widget_show(s1);
	gtk_widget_show(s2);
	gtk_widget_show(s3);
		
	list<WInfo::Box::Mail>::reverse_iterator i=x->second.content.rbegin();
	for(; i != x->second.content.rend(); i++,j++)
		{
		GtkWidget* l1,*l2,*l3;
		char str[5];
		snprintf(str,5,"%lu",J-j+2);

		l1 = gtk_label_new(str);
		l2 = gtk_label_new(i->From.c_str());
		l3 = gtk_label_new(i->Subject.c_str());
		
		gtk_misc_set_alignment (GTK_MISC (l1), 1.0, 0.5);
		gtk_misc_set_alignment (GTK_MISC (l2), 0.0, 0.5);
		gtk_misc_set_alignment (GTK_MISC (l3), 0.0, 0.5);
		
		gtk_label_set_selectable(GTK_LABEL(l3),TRUE);
		gtk_label_set_selectable(GTK_LABEL(l2),TRUE);
		gtk_label_set_selectable(GTK_LABEL(l1),TRUE);
		
		gtk_label_set_justify(GTK_LABEL(l1),GTK_JUSTIFY_LEFT);
		
		gtk_table_attach(GTK_TABLE(tbl),l1,0,1,j,j+1,
			GTK_FILL,GTK_SHRINK,2,0);
		gtk_table_attach(GTK_TABLE(tbl),l2,2,3,j,j+1,
			GTK_FILL,GTK_SHRINK,2,0);
		gtk_table_attach(GTK_TABLE(tbl),l3,4,5,j,j+1,
			GTK_FILL,GTK_SHRINK,2,0);
	
		gtk_widget_show(l1);
		gtk_widget_show(l2);
		gtk_widget_show(l3);
		}
	
	gtk_widget_show(tbl);
		
	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow1);
		
	gtk_scrolled_window_set_policy(	GTK_SCROLLED_WINDOW (scrolledwindow1), 
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	viewport1 = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport1);
	gtk_container_add (GTK_CONTAINER (scrolledwindow1),viewport1);
		
	gtk_container_add (GTK_CONTAINER (viewport1), tbl);
	gtk_container_add(GTK_CONTAINER(w1),scrolledwindow1);
	}

return ret;
}

/*******************************************************************************
 *
 *
 */
// undocumented signal
bool NotifyerGtk2::destroy(GtkWidget *w,gpointer user_data)
{
gtk_widget_destroy(GTK_WIDGET(user_data));
return FALSE;
}
/*******************************************************************************
 *
 *
 */

void NotifyerGtk2::UpdateWindow(WInfo* i)
{
map<ReducedMailbox*,NotifyerGtk2::WInfo::Box>::iterator x;
int size = i->boxes.size();
int j;
GtkWidget* t,*w = i->window;
GList* l;

if(i->position == string("top") ||
   i->position == string("bottom"))
	t = gtk_table_new(size,2,FALSE);
else
	t = gtk_table_new(2,size,FALSE);
	
for(j = 0 , x = i->boxes.begin() ; x != i->boxes.end() ; x++,j++)
	{
	
	GtkWidget*b = gtk_button_new();
	GtkWidget*l = gtk_label_new(x->first->GetName().c_str());
	if(x->second.content.size() > 0 )
		gtk_label_set_markup(GTK_LABEL(l),
			(string("<span foreground='blue'"
				" weight='ultrabold'"
				" stretch='condensed'>") + 
			 x->first->GetName().c_str() + 
			 string("</span>")).c_str());

	gtk_widget_show(l);
	gtk_container_add(GTK_CONTAINER(b),l);
	g_signal_connect(b,"clicked",
		G_CALLBACK(NotifyerGtk2::execute),
		(gpointer)x->first->GetCommand().c_str());
	
		
	GtkWidget*l2 = CreatePreview(i,x);
	
	if(i->position == string("topleft") ||
	   i->position == string("left") ||
	   i->position == string("bottomleft") ||
	   i->position == string("managed"))
		{
		gtk_table_attach(GTK_TABLE(t),b,0,1,j,j+1,
				GTK_FILL,GTK_SHRINK,0,0);
		gtk_table_attach(GTK_TABLE(t),l2,1,2,j,j+1,
				GTK_FILL,GTK_SHRINK,0,0);
		}
	else if (i->position == string("topright") ||
	   i->position == string("right") ||
	   i->position == string("bottomright"))
		{
		gtk_table_attach(GTK_TABLE(t),b,1,2,j,j+1,
				GTK_FILL,GTK_SHRINK,0,0);
		gtk_table_attach(GTK_TABLE(t),l2,0,1,j,j+1,
				GTK_FILL,GTK_SHRINK,0,0);
		}
	else if(i->position == string("top"))
		{
		gtk_table_attach(GTK_TABLE(t),b,j,j+1,0,1,
				GTK_FILL,GTK_SHRINK,0,0);
		gtk_table_attach(GTK_TABLE(t),l2,j,j+1,1,2,
				GTK_FILL,GTK_SHRINK,0,0);
		}
	else if(i->position == string("bottom"))
		{
		gtk_table_attach(GTK_TABLE(t),b,j,j+1,1,2,
				GTK_FILL,GTK_SHRINK,0,0);
		gtk_table_attach(GTK_TABLE(t),l2,j,j+1,0,1,
				GTK_FILL,GTK_SHRINK,0,0);
		}

	if(x->second.content.size() > 0 )
		{
		gtk_widget_set_sensitive(b,TRUE);
		gtk_widget_set_sensitive(l2,TRUE);
		gtk_button_set_relief(GTK_BUTTON(b),GTK_RELIEF_NORMAL);
		if (i->preview)
			gtk_button_set_relief(GTK_BUTTON(l2),GTK_RELIEF_NORMAL);
		gtk_widget_set_state(b,GTK_STATE_SELECTED);
		gtk_widget_show(b);
		gtk_widget_show(l2);
		}
	else
		{
		if( i->showall)
			{
			gtk_widget_set_sensitive(l2,FALSE);
			gtk_button_set_relief(GTK_BUTTON(b),GTK_RELIEF_NONE);
			if (i->preview)
				gtk_button_set_relief(
					GTK_BUTTON(l2),GTK_RELIEF_NONE);
			gtk_widget_set_sensitive(b,FALSE);
			gtk_widget_show(b);
			gtk_widget_show(l2);
			}
		else
			{
			gtk_widget_hide(b);
			gtk_widget_hide(l2);
			}
		}

	
		}
gtk_widget_show(t);

l = gtk_container_get_children(GTK_CONTAINER(w));

if(l != NULL)
	gtk_widget_destroy(GTK_WIDGET(l->data));

gtk_container_add(GTK_CONTAINER(w),t);
}
/*******************************************************************************
 *
 *
 */
NotifyerGtk2::~NotifyerGtk2()
{

}

extern "C" Notifyer* new_notifyer(){
return new NotifyerGtk2();
}

extern "C" void delete_notifyer(Notifyer*p){
delete p;
}



