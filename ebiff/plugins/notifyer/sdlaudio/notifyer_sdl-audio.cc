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


#include <SDL.h>

#include "notifyer_sdl-audio.h"

queue< string > NotifyerSDLaudio::files;
pthread_t NotifyerSDLaudio::manager;
pthread_attr_t NotifyerSDLaudio::manager_att;
pthread_mutex_t NotifyerSDLaudio::files_mutex;
pthread_mutexattr_t NotifyerSDLaudio::files_mutex_att;

/*******************************************************************************
 *
 *
 */
void NotifyerSDLaudio::SetString(string name, string value) throw(NotifyerException)
{
if(name == "file")
	file = value;
}
/*******************************************************************************
 *
 *
 */
void NotifyerSDLaudio::SetBool(string name, bool value) throw(NotifyerException)
{

}
/*******************************************************************************
 *
 *
 */
void NotifyerSDLaudio::SetNumber(string name, double value) throw(NotifyerException)
{
}

/*******************************************************************************
 *
 *
 */

void NotifyerSDLaudio::fill_audio(void *udata, Uint8 *stream, int len)
    {
	pair<Uint8*,Uint32*> *p = (pair<Uint8*,Uint32*> *)udata;
	    
        /* Only play if we have data left */
        if ( p->second == 0 )
            return;

        /* Mix as much data as possible */
        len = ( (Uint32)len > *(p->second) ? *(p->second) : len );
        SDL_MixAudio(stream, p->first, len, SDL_MIX_MAXVOLUME);
        p->first += len;
        *(p->second) -= len;
    }



void NotifyerSDLaudio::Notify(ReducedMailbox* m)
{
pthread_mutex_lock(&files_mutex);
files.push(file);
pthread_mutex_unlock(&files_mutex);

}

void *NotifyerSDLaudio::manager_f(void*x)
{
pair<queue< string > *,pthread_mutex_t*> *p;
p = (pair<queue< string > *,pthread_mutex_t*> *) x;
queue< string > *files = p->first;
pthread_mutex_t* files_mutex = p->second;

while(true)
	{
	sleep(1);
	
	pthread_mutex_lock(files_mutex);
	int size = files->size();
	pthread_mutex_unlock(files_mutex);
	
	while(size > 0)
		{
		string o;

		pthread_mutex_lock(files_mutex);
		o = files->front();
		files->pop();
		size = files->size();
		pthread_mutex_unlock(files_mutex);
		
		// play file o
		SDL_AudioSpec wav_spec;
		Uint32 wav_length=0;
		Uint8 *wav_buffer=NULL;
			
		if(!SDL_LoadWAV(o.c_str(),&wav_spec,&wav_buffer,&wav_length)){
		  	fprintf(stderr,
				"Could not open %s: %s\n",o.c_str(),
				SDL_GetError());
			continue;
		}
		
		//fprintf(stderr,"-> (%lu)\n",wav_length);
		
		wav_spec.callback = NotifyerSDLaudio::fill_audio;
		pair<Uint8*,Uint32*> d(wav_buffer,&wav_length);
		wav_spec.userdata = &d;
		
		int retry = 100;
		
		while ( SDL_OpenAudio(&wav_spec,NULL) < 0 ){
			retry--;
			usleep(500000);
			
			if(retry == 0)
				{
				fprintf(stderr,
					"Couldn't open audio: %s\n",
					SDL_GetError());
				SDL_FreeWAV(wav_buffer);
				continue;
				}
		}	
		
		SDL_PauseAudio(0);
		while(wav_length > 0)
			usleep(500000);

		SDL_PauseAudio(1);
		SDL_CloseAudio();
		SDL_FreeWAV(wav_buffer);
		
		usleep(500000);
		}
	
	
	}
}
/*******************************************************************************
 *
 *
 */
NotifyerSDLaudio::NotifyerSDLaudio()
{
if (!SDL_WasInit(SDL_INIT_AUDIO));	
	SDL_Init(SDL_INIT_AUDIO|SDL_INIT_NOPARACHUTE);
	
file = "";
	
pthread_mutexattr_init(&files_mutex_att);
pthread_mutex_init(&files_mutex,&files_mutex_att);
pthread_attr_init(&manager_att);
manager = pthread_create(&manager,&manager_att,manager_f,
	(void*) new pair< queue<string> *, pthread_mutex_t*>
		(&files,&files_mutex)
	);
}

/*******************************************************************************
 *
 *
 */
NotifyerSDLaudio::~NotifyerSDLaudio()
{

}

extern "C" Notifyer* new_notifyer(){
return new NotifyerSDLaudio();
}

extern "C" void delete_notifyer(Notifyer*p){
delete p;
}



