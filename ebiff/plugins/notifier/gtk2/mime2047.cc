#include <string>
using namespace std;

#include "mime2047.h"

/* BUG: the structure mime2047_info contains strings that are not 
 *      valid objects (returned by realloc) and when assigned they 
 *      use the operator = ans sigsegv 
 */

/*****************************************************************************
 *
 * SAMPLE:
 *   aaa =?isobla?B?BCDEF?= bbb
 *
 * REGEX:
 *   =?[^?]+?[QqBb]?[^?]+?=
 * 
 * DFA:
 * 
 *  =      ?      ^?       ?      QqBb      ?        ?       =
 * ---> 1 ----> 2 ---> 3 ----> 4 ------> 5 ----> 6 ----> 7 ----> OK 
 *                  ^ ^? |                     | ^? ^
 *                  \____/                     \____/
 *                  
 * TABLE:	      
 *  
 *     | 0 1 2 3 4 5 6 7 
 * ----+-----------------
 * =   | 1             OK
 * ?   |   2   4   6 7
 * ^?  |     3 3     6
 * QqBb|     3 3 5   6
 *
 * 
 *****************************************************************************/

#define T_EQ 		0
#define T_QM 		1
#define T_NOT_QM 	2
#define T_QB 		3

#define S_BEGIN 	0
#define S_ERR		-1
#define S_SUCCESS	-2


short int trans_table [] [8] = {
              /*    0      1      2      3      4      5      6          7 */ 
/* =    */    {     1, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR,     6, S_SUCCESS},
/* ?    */    {     0,     2, S_ERR,     4, S_ERR,     6,     7, S_ERR    },
/* ^?   */    {     0, S_ERR,     3,     3, S_ERR, S_ERR,     6, S_ERR    },
/* QqBb */    {     0, S_ERR,     3,     3,     5, S_ERR,     6, S_ERR    }
};


static int token_of_char(char c){
	if ( c == '?' ) return T_QM;
	if ( c == '=' ) return T_EQ;
	if ( toupper(c) == 'B' || toupper (c) == 'Q' ) return T_QB;
	return T_NOT_QM;
}

struct mime2047_info_list dfa_jumper(string s){
	short int state = S_BEGIN;
	struct mime2047_info_list l = {0,NULL};
	unsigned int position = 0;
	int token;
	int lang_begin = 0;
	int lang_end = 0;
	int chunk_begin = 0;
	int data_begin = 0;
	int data_end = 0;
	char encoding;
	unsigned int last_chunk = 0;

	for (position=0; position < s.size() || state == S_SUCCESS; position++){

		if (position < s.size()){
			token = token_of_char(s.at(position));
			//printf("%c %d %d\n",s.at(position),token,state);
		}

		if ( state == 1 ) chunk_begin = position;
		if ( state == 2 ) lang_begin = position;
		if ( state == 4 ) 
			lang_end = position;
		if ( state == 5 ) 
			data_begin = position;
		if ( state == 7 )
			data_end = position;
		if (state == S_SUCCESS ) {
			/* now we find 
			 *
			 * (1)bla=?(2)lang?(4)e?(5)data?=(7)....
			 * 
			 * 1) chunk_begin, 2) lang_begin, 4) lang_end
			 * 5) data_begin, 7) data_end
			 */

			/* insert 'bla' chunk */
			l.len++;
			l.chunks = (mime2047_info*)realloc(l.chunks,
					sizeof(mime2047_info) * l.len);
			l.chunks[l.len - 1].encoding = 'r';
			l.chunks[l.len - 1].language = strdup("");
			l.chunks[l.len - 1].data = strdup(s.substr(last_chunk,
				lang_begin - last_chunk - 2).c_str());

			/* insert the mime chunk */
			l.len++;
			l.chunks = (mime2047_info*)realloc(l.chunks,
					sizeof(mime2047_info) * l.len);
			l.chunks[l.len - 1].encoding = 
				tolower(s.at(lang_end));	
			l.chunks[l.len - 1].language = strdup(s.substr(
				lang_begin,lang_end - lang_begin - 1).c_str());
			l.chunks[l.len - 1].data = strdup(s.substr(
				data_begin + 1,
				data_end - data_begin - 2).c_str());
			/* hack to execute following reset code*/	
			state = S_ERR; 
			last_chunk = data_end+1;
		}
		if ( state == S_ERR ) {
			lang_begin =
			lang_end = 
			chunk_begin =
			data_begin =
			data_end = -1;
			encoding = '\0';
			state = S_BEGIN;
		}
		
		
		state = trans_table[token][state];
	}
	
	/* last abc chunk */
	if( last_chunk != s.size() ) {
		l.len++;
		l.chunks = (mime2047_info*)realloc(l.chunks,
				sizeof(mime2047_info) * l.len);
		l.chunks[l.len - 1].encoding = 'r';
		l.chunks[l.len - 1].language = strdup("");
		l.chunks[l.len - 1].data = strdup(s.substr(last_chunk,
			s.size() - last_chunk).c_str());

	}
	

	return l;
}

/*
static void print_chunk(struct mime2047_info m) {
	if (m.encoding == 'r') 
		printf("'%s' ",m.data);
	else
		printf("(%s,%c)'%s' ",m.language,m.encoding,m.data);
			
}
*/

void free_mime2047_info_list(struct mime2047_info_list l){
	for(int i = 0; i < l.len ; i++){
		::free(l.chunks[i].language);
		::free(l.chunks[i].data);
	}
	::free(l.chunks);
}

/*
int main(){
	//struct mime2047_info_list l = dfa_jumper("abc=?iso88?B?ABCD?=def");
	//struct mime2047_info_list l = dfa_jumper("=?iso88?B?ABCD?=def");
	struct mime2047_info_list l = dfa_jumper("=?iso88?B?ABCD?=");

	for ( int i = 0 ; i < l.len ; i++) 
		print_chunk(l.chunks[i]);		
	printf("\n");	
	
	return 0;
}
*/
