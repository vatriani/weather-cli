/*
 * output.c - define some easy fuctions
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
#include <stdlib.h>


/**
 * __DATE__ support over Makefile CFLAGS=-DDATE="\"`date +'%Y'`\""
 */
#ifndef __DATE__
 #define __DATE__ "2013"
#endif

/**
 * set some usable color codes
 */
#ifdef COLOR
 #define CSI_RED     "\x1b[31;1m"
 #define CSI_GREEN   "\x1b[32;1m"
 #define CSI_YELLOW  "\x1b[33;1m"
 #define CSI_BLUE    "\x1b[34;1m"
 #define CSI_MAGENTA "\x1b[35;1m"
 #define CSI_CYAN    "\x1b[36;1m"
 #define CSI_RESET   "\x1b[0m"
#endif

// some wrapper function for write to stdout
void outplain(char *tmp) {write(1,tmp,strlen(tmp));}

// some wrapper function for write to stderr
void outplainerr(char *tmp) {write(2,tmp,strlen(tmp));}


#ifdef COLOR
void styledoutput(char *type, char *mesg, char *color) {
 outplain("[");
 outplain(color);
 outplain(type);
 outplain("]");
 outplain(CSI_RESET);
 outplain(mesg);
 outplain("\n");
}
#else
void styledoutput(char *type, char *mesg) {
 outplain("[");
 outplain(type);
 outplain("]");
 outplain(mesg);
 outplain("\n");
}
#endif


// extended wrapper for write to stderr with colors
void outerr(char *tmp) {
#ifdef COLOR
 styledoutput("err",tmp,CSI_RED);
#else
 styledoutput("err",tmp);
#endif
}

void outfatal(char *tmp) {
 outerr(tmp);
 exit(1);
}

// extended wrapper for write to stdout as debug messages
#ifdef DEBUG
void outdeb(char *tmp) {
#ifdef COLOR
 styledoutput("deb",tmp,CSI_YELLOW);
#else
 styledoutput("deb",tmp);
#endif
}

void outedeb(char *msg,char *content) {
 outdeb(msg);
 outplain(" : ");
 outplain(content);
 outplain("\n");
}
#endif

void outstat(char *tmp) {
 outplain("  =>  ");
 outplain(tmp);
 outplain("\n");
}


/**
 * output programname -h for help message
 */
extern void showhelp(char *app_name, char *app_help) {
 outplain(app_name);
 outplain("  for Linux, BSD and Windows\nuse: ");
 outplain(app_name);
 outplain(" ");
 outplain(app_help);
 outplain("\n\n");
}

/**
 * output programname -v for version message
 */
extern void showversion(char *app_name, char *app_version) {
 outplain(app_name);
 outplain(" ");
 outplain(app_version);
 outplain("\nCopyright (C) ");
 outplain(__DATE__);
 outplain(" Niels Neumann  <vatriani.nn@googlemail.com\n\
License GPLv3+: GNU GPL Version 3 or later <http://gnu.org/licenses/gpl.html>.\
 \nThis is free software: you are free to change and redistribute it.\
 \nThere is NO WARRANTY, to the extent permitted by law.\n\n");
}
#endif
