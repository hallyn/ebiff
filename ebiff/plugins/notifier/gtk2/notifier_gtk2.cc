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
#include <stdio.h>
#include <stdlib.h>

#include "notifier_gtk2.h"
#include "escape.h"
#include "base64.h"

#include "mime2047.h"

GThread* NotifierGtk2::refresher = NULL;
queue<NotifierGtk2::UInfo*> NotifierGtk2::requests;
GStaticMutex NotifierGtk2::requests_mutex = G_STATIC_MUTEX_INIT;
int NotifierGtk2::argn=1;
const char* NotifierGtk2::v[] = {"ebiff",NULL};
char** NotifierGtk2::argv=(char**)v;

/*******************************************************************************
 *
 *
 */
void NotifierGtk2::SetString(string name, string value) throw(NotifierException)
{
if( name == string("position"))
	status.position = value;
if( name == string("spannumber"))
	status.spannumber = value;
if( name == string("spanname"))
	status.spanname = value;
}
/*******************************************************************************
 *
 *
 */
void NotifierGtk2::SetBool(string name, bool value) throw(NotifierException)
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
void NotifierGtk2::SetNumber(string name, double value) throw(NotifierException)
{

}
		
/*******************************************************************************
 *
 *
 */
void NotifierGtk2::Notify(ReducedMailbox* m)
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
		"delete-event",G_CALLBACK(NotifierGtk2::exit),NULL);
	}

	
g_static_mutex_lock(&requests_mutex);
requests.push(UpdateStatus(m));
g_static_mutex_unlock(&requests_mutex);
}

/*******************************************************************************
 *
 *
 */
NotifierGtk2::NotifierGtk2()
{
gtk_init_check(&argn,&argv);
status.window = NULL;
status.showall = false;
status.preview = false;
status.spannumber="";
status.spanname="";
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
NotifierGtk2::Box::Mail::Mail(string F,string S,string U)
{
From = F;
Subject = S;
Uid = U;
}

/*******************************************************************************
 *
 *
 */
NotifierGtk2::UInfo* NotifierGtk2::UpdateStatus(ReducedMailbox* m)
{
// copy current status on i
UInfo* i = new UInfo(
	new Box(m),
	status.boxes[m],
	status.window,
	status.position,
	status.showall,
	status.preview,
	status.spannumber,
	status.spanname);

return i;
}

/*******************************************************************************
 *
 *
 */
NotifierGtk2::Box::Box(ReducedMailbox*m)
{
long unsigned int n = m->CountNew();

for(long unsigned int i=0 ; i < n ; i++)
	content.push_back(
		Mail(m->GetNewFrom(i),m->GetNewSubject(i),m->GetNewUid(i)));

name = m->GetName();
command = m->GetCommand();
}

/*******************************************************************************
 *
 *
 */
NotifierGtk2::Status::BoxInfo::BoxInfo()
{
position = -1;
}

/*******************************************************************************
 *
 *
 */
NotifierGtk2::UInfo::UInfo(Box*a,Status::BoxInfo &b,
	GtkWidget *c,string d,bool e,bool f,string num,string nam):
info(b)
{
mailbox = a;
window = c;
position = d;
showall = e;
preview = f;
spannumber = num;
spanname = nam;
}

/*******************************************************************************
 *
 *
 */
NotifierGtk2::UInfo::~UInfo()
{
delete mailbox;
}
/*******************************************************************************
 *
 *
 */

gboolean NotifierGtk2::main_quit(gpointer)
{
if(gtk_main_level() > 0)
	gtk_main_quit();

return TRUE;
}

/*******************************************************************************
 *
 *
 */
void NotifierGtk2::PositionWindow(UInfo* i)
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
gpointer NotifierGtk2::thread (gpointer p)
{
g_timeout_add(500,NotifierGtk2::main_quit,NULL);
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
		UInfo* i = requests.front();
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

		delete i;
		}
	}
return NULL;
}

/*******************************************************************************
 *
 *
 */
void NotifierGtk2::execute(GtkButton *button,gpointer user_data)
{
system((char*)user_data);
}
/*******************************************************************************
 *
 *
 */
void NotifierGtk2::show(GtkButton *button,gpointer user_data)
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

bool NotifierGtk2::hide(GtkWidget *w,GdkEvent *event,gpointer user_data)
{
gtk_widget_hide(GTK_WIDGET(w));
return TRUE;
}
/*******************************************************************************
 *
 *
 */
bool  NotifierGtk2::hide_and_toggle(GtkWidget *w,
		GdkEvent *event,gpointer user_data)
{
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(user_data),FALSE);
return TRUE;
}
/*******************************************************************************
 *
 *
 */

// support only one encoding in s
//
// THIS is the worst code I've ever written
// 

static string escape_xml(string before)
{
	unsigned int loc=0;

	//dirty hack
	before = " " + before;
	
	for(;;){ //&
		loc = before.find("&",loc+1);
		if (loc == string::npos ) break;
		before = before.substr(0,loc) + 
			string("&amp;") + before.substr(loc+1);
	}
	loc=0;
	for(;;){ //<
		loc = before.find("<",loc+1);
		if (loc == string::npos ) break;
		before = before.substr(0,loc) + 
			string("&lt;") + before.substr(loc+1);
	}
	loc=0;
	for(;;){ //>
		loc = before.find(">",loc+1);
		if (loc == string::npos ) break;
		before = before.substr(0,loc) + 
			string("&gt;") + before.substr(loc+1);
	}
	
	return before.substr(1);
}
 
GtkWidget* NotifierGtk2::UnicodeLabelOf(string s)
{
GtkWidget*lab;

struct mime2047_info_list l = dfa_jumper(s);
string label_markup;
bool needs_special_encoding = false;

for ( int i = 0 ;  i < l.len ; i++){
	if (l.chunks[i].encoding == 'r') {
		label_markup += escape_xml(l.chunks[i].data);
	} else if (l.chunks[i].encoding == 'b') {
		char * data = (char*)malloc(sizeof(char) * 
				strlen(l.chunks[i].data) / 4 * 3);
		base64_decode(l.chunks[i].data,data);
		label_markup += string("<span lang=\"") + 
			string(l.chunks[i].language) +
		 	string("\" >") + escape_xml(data) +
			string("</span>");
		::free(data);
		needs_special_encoding = true;
	} else if (l.chunks[i].encoding == 'q') {
		label_markup +=	string("<span lang=\"") + 
			string(l.chunks[i].language) +
		 	string("\" >") + 
			escape_xml(unquote(l.chunks[i].data)) +
			string("</span>");
		needs_special_encoding = true;
	}
}

free_mime2047_info_list(l);
	
//render it
lab = gtk_label_new(s.c_str());
if ( needs_special_encoding ){
	gtk_label_set_markup(GTK_LABEL(lab),label_markup.c_str());
}

return lab;
}

GtkWidget* NotifierGtk2::CreatePreview(UInfo*i,NotifierGtk2::Box *x)
{
GtkWidget* ret;
char str[5];
string snumber = i->spannumber;
string label;

snprintf(str,5,"%d",x->content.size());

label = string(str);

if (!i->preview){
	ret = gtk_label_new(label.c_str());
	gtk_label_set_markup(GTK_LABEL(ret),
		(string("<span ") + escape_xml(snumber) + string(" >") + 
		 escape_xml(str) + string("</span>")).c_str());

} else {
	GtkWidget* w1,*l1,*l2,*l3,*tbl,*s1,*s2,*s3,*scrolledwindow1,*viewport1;
	unsigned long int J=x->content.size();
	unsigned long int j=2;
	GtkWidget *lab;

	lab = gtk_label_new(label.c_str());
	gtk_label_set_markup(GTK_LABEL(lab),
		(string("<span ") + escape_xml(snumber) + string(" >") + 
		 escape_xml(label.c_str()) + string("</span>")).c_str());
	ret = gtk_toggle_button_new();
	gtk_container_add(GTK_CONTAINER(ret),lab);
	gtk_widget_show(lab);
	
	w1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(w1),GTK_WIN_POS_CENTER_ALWAYS);
	//g_signal_connect(w1,"delete-event",G_CALLBACK(gtk_true),NULL);
	g_signal_connect(w1,"delete-event",G_CALLBACK(hide_and_toggle),ret);
	gtk_widget_set_size_request (w1, 530, 230);
	
	g_signal_connect(ret,"clicked",G_CALLBACK(NotifierGtk2::show),w1);
	g_signal_connect(ret,"destroy",G_CALLBACK(NotifierGtk2::destroy),w1);
	
	tbl=gtk_table_new(5,x->content.size()+2,FALSE);
		
	l1 = gtk_label_new("#");
	l2 = gtk_label_new("From");
	l3 = gtk_label_new("Subject");
	/*	
	gtk_label_set_markup(GTK_LABEL(l1),"<b>#</b>");
	gtk_label_set_markup(GTK_LABEL(l2),"<b>From</b>");
	gtk_label_set_markup(GTK_LABEL(l3),"<b>Subject</b>");
	*/
	
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
		
	list<Box::Mail>::reverse_iterator i=x->content.rbegin();
	for(; i != x->content.rend(); i++,j++)
		{
		GtkWidget* l1,*l2,*l3;
		char str[5];

		snprintf(str,5,"%d",j-2);

		l1 = gtk_label_new(str);
		l2 = UnicodeLabelOf(i->From);
		l3 = UnicodeLabelOf(i->Subject);
		
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
bool NotifierGtk2::destroy(GtkWidget *w,gpointer user_data)
{
gtk_widget_destroy(GTK_WIDGET(user_data));
return FALSE;
}
/*******************************************************************************
 *
 *
 */
bool NotifierGtk2::free(GtkWidget *w,gpointer user_data)
{
::free(user_data);
return FALSE;
}
/*******************************************************************************
 *
 *
 */
void NotifierGtk2::response(
		GtkDialog *dialog,gint arg1,gpointer user_data)
{
switch (arg1)
	{
	case GTK_RESPONSE_ACCEPT:
		::exit(0);
	}
}

bool NotifierGtk2::exit(GtkWidget *w,gpointer user_data)
{
GtkWidget* dialog;
GtkWidget*label = gtk_label_new ("\nDo you really want to close ebiff?\n\n");

dialog = gtk_dialog_new_with_buttons("Question",NULL,
                                  GTK_DIALOG_MODAL,
				  "Yes",
                                  GTK_RESPONSE_ACCEPT,
                                        "No",
                                                 GTK_RESPONSE_REJECT,
                                                 NULL);
g_signal_connect_swapped (GTK_OBJECT (dialog), 
                             "response", 
                             G_CALLBACK (NotifierGtk2::response),
                             GTK_OBJECT (dialog));
gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                      label);
gtk_widget_show_all(dialog);

gtk_dialog_run (GTK_DIALOG (dialog));
gtk_widget_destroy (dialog);

return TRUE;
}

/*******************************************************************************
 *
 *
 */

void NotifierGtk2::UpdateWindow(UInfo* i)
{
//map<ReducedMailbox*,Box>::iterator x;
int size = 0;
int j;
GtkWidget* t = NULL,*w = i->window;
GList* l = NULL;
string sname;

// if t exists -> size != 0.. read it!
l = gtk_container_get_children(GTK_CONTAINER(w));
if(l != NULL)
	if(i->position == string("top") ||
	   i->position == string("bottom"))
		g_object_get(G_OBJECT(l->data),"n-columns",&size,NULL);
	else
		g_object_get(G_OBJECT(l->data),"n-rows",&size,NULL);

// create/resize the table
// and give t <- != NULL
if(l  == NULL)
	{	
	if(i->position == string("top") ||
	   i->position == string("bottom"))
		t = gtk_table_new(2,1,FALSE);
	else
		t = gtk_table_new(1,2,FALSE);

	gtk_container_add(GTK_CONTAINER(w),t);
	gtk_widget_show(t);
	}
else 
	{
	t = GTK_WIDGET(GTK_TABLE(l->data));

	if(i->position == string("top") ||
	   i->position == string("bottom"))
		g_object_get(G_OBJECT(t),"n-columns",&size,NULL);
	else
		g_object_get(G_OBJECT(t),"n-rows",&size,NULL);
	
	// we need a resize
	if( i->info.position == -1 )
		{
		t = GTK_WIDGET(GTK_TABLE(l->data));
		if(i->position == string("top") ||
		   i->position == string("bottom"))
			gtk_table_resize(GTK_TABLE(t),2,size+1);
		else
			gtk_table_resize(GTK_TABLE(t),size+1,2);
		}
	}

// set the position to the last one, or recall the saved one and 
// remove old widgets
if( i->info.position == -1 )
	i->info.position = size; // this passes it back! shit c++ 
else
	{
	// remove old widgets...
	l = gtk_container_get_children(GTK_CONTAINER(t));
	int done = 0;
	while (l != NULL && done < 2)
		{
		int pos;
		
		if(i->position == string("top") ||
		   i->position == string("bottom"))
			gtk_container_child_get(GTK_CONTAINER(t),
				GTK_WIDGET(l->data),
				"left-attach",&pos,NULL);
		else
			gtk_container_child_get(GTK_CONTAINER(t),
				GTK_WIDGET(l->data),
				"top-attach",&pos,NULL);

		if( pos == i->info.position)
			{
			done ++;
			gtk_widget_destroy(GTK_WIDGET(l->data));
			}
		l = l->next;
		}

	if(done < 2)
		fprintf(stderr,"internal error, a previously "
				"added widget is lost :(\n");
	}

// j means the position of the widget that will be attached
j = i->info.position;
sname = i->spanname;

Box* x = i->mailbox;

// create the button
GtkWidget*b = gtk_button_new();
GtkWidget*lbl = gtk_label_new(x->name.c_str());
gtk_label_set_markup(GTK_LABEL(lbl),(string("<span ") + 
		escape_xml(sname) + string(" >") + 
		escape_xml(x->name) + string("</span>")).c_str());

gtk_widget_show(lbl);
gtk_container_add(GTK_CONTAINER(b),lbl);

// since command is freed we make a copy and we free 
// it when destroyed.
char * tmp  =strdup(x->command.c_str());
g_signal_connect(b,"clicked",
	G_CALLBACK(NotifierGtk2::execute),
	(gpointer)tmp);
g_signal_connect(b,"destroy",
	G_CALLBACK(NotifierGtk2::free),
	(gpointer)tmp);

// create the label
GtkWidget*l2 = CreatePreview(i,x);

// attach the created button+label
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

// hilight the buttons
if(x->content.size() > 0 )
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
/*******************************************************************************
 *
 *
 */
NotifierGtk2::~NotifierGtk2()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierGtk2();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



