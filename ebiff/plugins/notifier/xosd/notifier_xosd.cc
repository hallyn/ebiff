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


#include <xosd.h>

#include "notifier_xosd.h"

/*******************************************************************************
 *
 *
 */
void NotifierXosd::SetString(string name, string value) throw(NotifierException)
{
//printf("%s=%s\n",name.c_str(),value.c_str());
if( name == "color")
	color=value;
if( name == "font")
        font=value;
if( name == "outlinecolor")
        outlinecolor=value;
if( name == "align")
	if(value == "left")
        	align=XOSD_left;
	else if(value == "center")
        	align=XOSD_center;
	else if(value == "right")
        	align=XOSD_right;
if( name == "pos")
        if(value == "top")
        	pos=XOSD_top;
	else if(value == "middle")
        	pos=XOSD_middle;
	else if(value == "bottom")
        	pos=XOSD_bottom;
	//else printf("EEEEEEEEEEEEEE\n");
	
}
/*******************************************************************************
 *
 *
 */
void NotifierXosd::SetBool(string name, bool value) throw(NotifierException)
{

}
/*******************************************************************************
 *
 *
 */
void NotifierXosd::SetNumber(string name, double value) throw(NotifierException)
{
	
if( name == "hoffset")
	hoffset = (int)value;
if( name == "outlineoffset")
	outlineoffset = (int)value;
if( name == "shoadowoffset")
        shadowoffset = (int)value;
if( name == "timeout")
        timeout = (int)value;
if( name == "voffset")
        voffset = (int)value;

}
		
/*******************************************************************************
 *
 *
 */
void NotifierXosd::Notify(ReducedMailbox* m)
{
long unsigned int n = m->CountNew();
xosd *osd;
osd = xosd_create (1);

xosd_set_pos(osd, pos);
xosd_set_align(osd, align);
xosd_set_vertical_offset(osd, voffset);
xosd_set_horizontal_offset(osd, hoffset);
xosd_set_colour(osd, color.c_str());
xosd_set_shadow_offset(osd, shadowoffset);
xosd_set_outline_offset(osd, outlineoffset);
xosd_set_outline_colour(osd, outlinecolor.c_str());
xosd_set_font(osd, font.c_str());

xosd_set_timeout(osd, timeout);

char num[5];
snprintf(num,5,"%lu",n);

string s;
if(n > 1)
	s = string("There are ") + string(num) +
		(" new mails in mailbox ") + m->GetName();
else if (n == 1)
	s = string("There is a new mail in mailbox ") + m->GetName();
else
	s = string("There is no mail in mailbox ") + m->GetName();
	

pthread_mutex_lock(&display_mutex);
display.push( pair<xosd*,string>(osd,s));
pthread_mutex_unlock(&display_mutex);

}

void *NotifierXosd::manager_f(void*x)
{
pair<queue< pair <xosd*, string > > *,pthread_mutex_t*> *p;
p = (pair<queue< pair <xosd*, string > > *,pthread_mutex_t*> *) x;
queue< pair<xosd*,string> > *display = p->first;
pthread_mutex_t* display_mutex = p->second;
while(true)
	{
	sleep(1);
	
	pthread_mutex_lock(display_mutex);
	int size = display->size();
	pthread_mutex_unlock(display_mutex);
	
	while(size > 0)
		{
		pair<xosd *,string> o;

		pthread_mutex_lock(display_mutex);
		o = display->front();
		display->pop();
		size = display->size();
		pthread_mutex_unlock(display_mutex);
		
		
		xosd_display (o.first, 0, XOSD_string, o.second.c_str() );

		xosd_wait_until_no_display(o.first);
	
		xosd_destroy(o.first);
		usleep(500000);
		}
	
	
	}
}
/*******************************************************************************
 *
 *
 */
NotifierXosd::NotifierXosd()
{
pthread_mutexattr_init(&display_mutex_att);
pthread_mutex_init(&display_mutex,&display_mutex_att);
pthread_attr_init(&manager_att);
manager = pthread_create(&manager,&manager_att,manager_f,
	(void*) new pair<queue< pair<xosd*,string> > *,
		pthread_mutex_t*>
		(&display,&display_mutex)
	);
}

/*******************************************************************************
 *
 *
 */
NotifierXosd::~NotifierXosd()
{

}

extern "C" Notifier* new_notifier(){
return new NotifierXosd();
}

extern "C" void delete_notifier(Notifier*p){
delete p;
}



