/*
 * common_output.c - define some easy fuctions
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

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <unistd.h>
#include <string.h>
#include <malloc.h>

/**
 * __DATE__ support over Makefile CFLAGS=-DDATE="\"`date +'%Y'`\""
 */
#ifndef __DATE__
 #define __DATE__ "unknown"
#endif

/**
 * set some usable color codes
 */
#define CSI_RED     "\x1b[31;1m"
#define CSI_GREEN   "\x1b[32;1m"
#define CSI_YELLOW  "\x1b[33;1m"
#define CSI_BLUE    "\x1b[34;1m"
#define CSI_MAGENTA "\x1b[35;1m"
#define CSI_CYAN    "\x1b[36;1m"
#define CSI_RESET   "\x1b[0m"


// some wrapper function for write to stdout
extern void outplain(char *tmp) {write(1,tmp,strlen(tmp));}

// some wrapper function for write to stderr
extern void outplainerr(char *tmp) {write(2,tmp,strlen(tmp));}

/**
 * extends strcat and combined with strcpy with malloc needed space
 * needs to be free() befor terminated programm
 */
void strmcat(char **base_str,char *append_str) {
 char *tmp;

 if(*base_str==NULL) {
 *base_str=malloc(strlen(append_str)+1);
 memcpy(*base_str,append_str,strlen(append_str)+1);
 } else {
  tmp=*base_str;
  *base_str=malloc(strlen(tmp)+strlen(append_str)+1);
  memcpy(*base_str,tmp,strlen(tmp));
  memcpy(*base_str+strlen(tmp),append_str,strlen(append_str)+1);
 }
}

/**
 * replace in a string the replace_hint with insert_word
 * need to be free() befor terminating
 */
void strreplace(char **string,char *replace_hint,char *insert_word) {
 int oldLen;
 int newLen;
 char *tmp[3];
 char *pointer;

 pointer=strstr(*string,replace_hint);
 if(pointer==NULL) return;

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
}
// extended wrapper for write to stderr with colors
void outerr(char *tmp) {
 outplain("[");
 outplain(CSI_RED);
 outplain("err");
 outplain(CSI_RESET);
 outplain("] ");
 outplain(tmp);
 outplain("\n");
}

// extended wrapper for write to stdout as debug messages
void outdeb(char *tmp) {
#ifdef DEBUG
 outplain("[");
 outplain(CSI_YELLOW);
 outplain("deb");
 outplain(CSI_RESET);
 outplain("] ");
 outplain(tmp);
 outplain("\n");
#endif
}

void outstat(char *tmp) {
 outplain("  =>  ");
 outplain(tmp);
 outplain("\n");
}

/**
 * output programname -h for help message
 */
extern void showHelp(char *app_name, char *app_help) {
 char *buffer=NULL;

 strmcat(&buffer,"%1  for Linux, BSD and Windows\nuse: %1 %2\n\n");
 strreplace(&buffer,"%1",app_name);
 strreplace(&buffer,"%1",app_name);
 strreplace(&buffer,"%2",app_help);
 outplain(buffer);
 free(buffer);
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

/**
 * output programname -v for version message
 */
extern void showVersion(char *app_name, char *app_version) {
 char *buffer=NULL;

 strmcat(&buffer,"%1 %2\nCopyright (C) %3 Niels Neumann  <vatriani.nn@googlemail.com\n\
License GPLv3+: GNU GPL Version 3 or later <http://gnu.org/licenses/gpl.html>.\
 \nThis is free software: you are free to change and redistribute it.\
 \nThere is NO WARRANTY, to the extent permitted by law.\n\n");
 strreplace(&buffer,"%1",app_name);
 strreplace(&buffer,"%2",app_version);
 strreplace(&buffer,"%3",__DATE__);
 outplain(buffer);
 free(buffer);
}
#endif