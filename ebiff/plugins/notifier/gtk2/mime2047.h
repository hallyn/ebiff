/******************************************************************************
 * $Id$
 * This file is part of ebiff (http://ebiff.sf.net)                           *
 * This file is distributed under the terms of GNU GPL license.               *
 ******************************************************************************/


/******************************************************************************/
 /*!
  * \file   mime2047.h
  * \brief  mime2047 parser
  * 
  * \author Enrico Tassi <gareuselesinge@users.sourceforge.net>
  */
/******************************************************************************/

#ifndef MIME2047_H_
#define MIME2047_H_

struct mime2047_info {
	/* 'c' , 'b', 'r' (means that the chunk is not encoded) */
	char encoding; 
	/* something like iso8859-1 or '' if encoding = 'r'  */
	char* language; 
	/* in raw format (selected by encoding) */
	char* data; 
};

struct mime2047_info_list {
	int len;
	struct mime2047_info *chunks;
};

struct mime2047_info_list dfa_jumper(string s);
void free_mime2047_info_list(struct mime2047_info_list l);

#endif
