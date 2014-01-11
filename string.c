/*
 * string.c - define some extended string functions
 * Copyright © 2013 - Niels Neumann  <vatriani.nn@googlemail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __STRING_H__
#define __STRING_H__

#include <malloc.h>
#include <string.h>


/**
 * extends strcat and combined with strcpy with malloc needed space
 * needs to be free() befor terminated programm
 */
void strmcat(char **base_str,char *append_str) {
 char *tmp;
 const int len=strlen(append_str)+1;

 if(*base_str==NULL) {
 *base_str=malloc(len);
 memcpy(*base_str,append_str,len);
 } else {
  tmp=*base_str;
  *base_str=malloc(strlen(tmp)+len);
  memcpy(*base_str,tmp,strlen(tmp));
  memcpy(*base_str+strlen(tmp),append_str,len);
 }
}

/**
 * replace in a string the replace_hint with insert_word
 * need to be free() befor terminating
 */
int strreplace(char **string,char *replace_hint,char *insert_word) {
 int oldLen;
 int newLen;
 char *tmp[3];
 char *pointer;

 pointer=strstr(*string,replace_hint);
 if(pointer==NULL) return 0;

 oldLen=strlen(*string)+1;
 newLen=oldLen-strlen(replace_hint)+strlen(insert_word)+1;

 tmp[0]=malloc(oldLen);
 tmp[1]=malloc(oldLen);
 tmp[2]=malloc(oldLen+strlen(insert_word));
 memcpy(tmp[0],*string,oldLen);
 *string=NULL;

 pointer=strstr(tmp[0],replace_hint);
 pointer+=strlen(replace_hint);
 memcpy(tmp[2],pointer,strlen(pointer)+1);

 pointer=strstr(tmp[0],replace_hint);
 *pointer='\0';
 memcpy(tmp[1],tmp[0],strlen(tmp[0])+1);

 *string=malloc(newLen+1);
 pointer=*string;
 memcpy(pointer,tmp[1],strlen(tmp[1]));
 pointer+=strlen(tmp[1]);
 memcpy(pointer,insert_word,strlen(insert_word));
 pointer+=strlen(insert_word);
 memcpy(pointer,tmp[2],strlen(tmp[2])+1);

 free(tmp[0]);
 free(tmp[1]);
 free(tmp[2]);
 return 1;
}

/**
 * removing tailing spaces and non printable chars
 */
void strdelsp(char *s) {
 if(s!=NULL) {
  char test[strlen(s)+1];
  unsigned int counter=1;

  memcpy(test,s,strlen(s)+1);
  for(;counter<(strlen(test));++counter) {
   test[counter-1]=test[counter];
   if(test[counter]<32) test[--counter]=0;
  }
  memcpy(s,test,strlen(test)+1);
 }
}
#endif
