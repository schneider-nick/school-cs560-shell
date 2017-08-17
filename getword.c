 /*
Nicholas Schneider 
816879772
John Carroll
CS570 Fall 2016
San Diego State University
*/

#include <stdio.h>
#include <string.h>
#include <strings.h>

#define STORAGE 255
      /* This is one more than the max wordsize that getword() can handle */
int getword(char *w) {
	//extern int slash_flag;
	int inchar,slash_flag;
	int count =0;
	int slash = 0;
	int dollor =0;
//infaante for loop runs till it fines a new line or  EOF or space.
	while(count < STORAGE-1 && (inchar=getchar()) != EOF) {		//test for EOF and if input will fit in to buffer
		if(inchar == '\\'|| slash == 1){
			if(slash == 0){
				slash = 1;
				continue;
			}else if(inchar == '\\' && slash == 1){
				w[count] = (char) inchar;
				count++;
				slash = 0;
				continue;
				}
			}
		if(slash == 0){						//if / happend then all of the other tests skip to add the mete to arry
			if(inchar == '<') {				//test for <  
				if(count == 0){
					w[count] = (char) inchar;
					count++;
					break;
					}
					
				(void)ungetc(inchar,stdin);
				break;
				}		
			if(inchar == '>') {				//test for >
				if(count == 0){
					w[count] = (char) inchar;
					count++;
					break;
					}
				(void)ungetc(inchar,stdin);
				break;		
			}
			if(inchar == '|'){				//test for |
				if(count != 0){
					(void)ungetc(inchar,stdin);
					break;
				}else {
					w[count] = (char) inchar;
					count++;
					break;
					}
				}	
			if(inchar == '$' && count == 0){		//test for $ and set dollor to send neg count
				dollor = 1;
				}			
			if(inchar == '&'){				//test for &
				if(count == 0){
				
					w[count] = (char) inchar;
					count++;
					break;
					}
				(void)ungetc(inchar,stdin);
				break;			
				}
			if(inchar == ' ') {				//test for space
				if(count !=0 && w[count-1] != ' ' ) {
					break;
					}
				continue;
				}
			}
		if(slash ==1){
			switch(inchar){
			case '<' : slash_flag =1;
			case '>' : slash_flag =1;
			case '|' : slash_flag =1;
			}
			slash=0;
			}	
		if(inchar == '\n') {	           			 //test for new line
			if(count == 0){								//tests for eof to add null and return -1
				w[count] = 00;
				return 0;
				}
			(void)ungetc(inchar,stdin);
			break;	
			}
		w[count] = (char) inchar;				//puts char in arry	
		count++;	
	}
	if(inchar == EOF) {
		if(count == 0){								//tests for eof to add null and return -1
			w[count] = 00;
			return -1;
			}
		}
	if(dollor ==1){
	// fix so get word send no $ over when it is only one							//test for dollor set to send a neg count
		w[count] = 00;
		return -count;
		}
	w[count] = 00;							//null terminates array
	return count;
}


/* (Note: the preceding line is an ANSI C prototype statement for getword().
    It will work fine with rohan's gcc or cc compiler.)

* The getword() function gets one word from the input stream.
* It returns -1 if end-of-file is encountered;
* otherwise, it returns the number of characters in the word
*
* INPUT: a pointer to the beginning of a character string
* OUTPUT: the number of characters in the word (or the negative of that number)
* SIDE EFFECTS: bytes beginning at address w will be overwritten.
*   Anyone using this routine should have w pointing to an
*   available area at least STORAGE bytes long before calling getword().

Upon return, the string pointed to by w contains the next word in the line
from stdin. A "word" is a string containing a single metacharacter or a string
consisting of non-metacharacters delimited by blanks or metacharacters.
*/
