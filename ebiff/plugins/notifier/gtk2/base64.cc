/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2004, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id$
 ***************************************************************************/

/* Base64 encoding/decoding
 *
 * Test harnesses down the bottom - compile with -DTEST_ENCODE for
 * a program that will read in raw data from stdin and write out
 * a base64-encoded version to stdout, and the length returned by the
 * encoding function to stderr. Compile with -DTEST_DECODE for a program that
 * will go the other way.
 *
 * This code will break if int is smaller than 32 bits
 */

//#include "setup.h"

#include <stdlib.h>
#include <string.h>

//#define _MPRINTF_REPLACE /* use our functions only */
//#include <curl/mprintf.h>

#include "base64.h"
//#include "memory.h"

/* include memdebug.h last */
//#include "memdebug.h"


static void decodeQuantum(unsigned char *dest, const char *src)
{
  unsigned int x = 0;
  int i;
  for(i = 0; i < 4; i++) {
    if(src[i] >= 'A' && src[i] <= 'Z')
      x = (x << 6) + (unsigned int)(src[i] - 'A' + 0);
    else if(src[i] >= 'a' && src[i] <= 'z')
      x = (x << 6) + (unsigned int)(src[i] - 'a' + 26);
    else if(src[i] >= '0' && src[i] <= '9')
      x = (x << 6) + (unsigned int)(src[i] - '0' + 52);
    else if(src[i] == '+')
      x = (x << 6) + 62;
    else if(src[i] == '/')
      x = (x << 6) + 63;
    else if(src[i] == '=')
      x = (x << 6);
  }

  dest[2] = (unsigned char)(x & 255);
  x >>= 8;
  dest[1] = (unsigned char)(x & 255);
  x >>= 8;
  dest[0] = (unsigned char)(x & 255);
}

/*
 * Curl_base64_decode()
 *
 * Given a base64 string at src, decode it into the memory pointed to by
 * dest. Returns the length of the decoded data.
 */
size_t base64_decode(const char *src, char *dest)
{
  int length = 0;
  int equalsTerm = 0;
  int i;
  int numQuantums;
  unsigned char lastQuantum[3];
  size_t rawlen=0;

  while((src[length] != '=') && src[length])
    length++;
  while(src[length+equalsTerm] == '=')
    equalsTerm++;

  numQuantums = (length + equalsTerm) / 4;

  rawlen = (numQuantums * 3) - equalsTerm;

  for(i = 0; i < numQuantums - 1; i++) {
    decodeQuantum((unsigned char *)dest, src);
    dest += 3; src += 4;
  }

  decodeQuantum(lastQuantum, src);
  for(i = 0; i < 3 - equalsTerm; i++)
    dest[i] = lastQuantum[i];

  return rawlen;
}

