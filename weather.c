/*
 * weather - fetching weather stats from an GMXX-code
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

//#define DEBUG
//#define DBASE

#define BUFFER_SIZE 1024

#define APP_NAME "weather-cli"
#define APP_VERSION "1.6"
#define APP_HELP " [OPTION]\n\n\
  -s\t\tshort output\n\
  -f\t\tFORMAT\n\
  -l\t\tlinking image\n\n\
  -v\t\tversion\n\
  -h\t\thelp"
#define SHARED_FOLDER "/usr/share/weather-cli"

#ifdef DBASE
 #include "db.h"
#endif

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/param.h>
#include <unistd.h>

#include "string.c"
#include "output.c"
#include "tcp.c"
#include "file.c"

#define BIT1 1 // -s small output
#define BIT2 2 // -f format support
#define BIT3 4 // -l symlink image
#define BIT4 8
#define BIT5 16
#define BIT6 32
#define BIT7 64
#define BIT8 128

static char *bez[][2]={
 {"chill","%cwc"},
 {"city","%lc"},
 {"country","%lR"},
 {"current code","%cc"},
 {"current date","%cd"},
 {"current image","%ci"},
 {"current temp","%ct"},
 {"current text","%cm"},
 {"direction","%cwd"},
 {"humidity","%hu"},
 {"latitude","%la"},
 {"longitude","%lo"},
 {"pressure","%pr"},
 {"region","%lr"},
 {"rising","%ri"},
 {"speed","%cws"},
 {"sunrise","%sr"},
 {"sunset","%ss"},
 {"tomorrow code","%tc"},
 {"tomorrow date","%tD"},
 {"tomorrow day","%td"},
 {"tomorrow image","%ti"},
 {"tomorrow temp high","%tth"},
 {"tomorrow temp low","%ttl"},
 {"tomorrow text","%tm"},
 {"visibility","%vi"}
};

/**
 * fetching infos of buffer and copying it into array
 */
void parsing_infos(char **weather,char *input_buffer) {
 char *line[80];
 char *itarator=NULL;
 unsigned short int counter=0;

 itarator=strstr(input_buffer,"\n\r")+2;

 line[counter]=strtok(itarator,"=");
 while(line[counter-1]) {
  line[counter]=strtok(NULL,"=");
  strdelsp(line[counter]);
  counter++;
 }

 counter=0;
 for(;counter<26;++counter) {
  weather[counter]=malloc(strlen(line[counter])*sizeof(char));
  memcpy(weather[counter],line[counter],strlen(line[counter])+1);
 }
}

/**
 * symlinking new icon to actual.gif in .cache/weather/
 */
void symlinking_image(char *image) {
 char *filename_new=SHARED_FOLDER;
 char *filename_old=0;

 getCachePath(&filename_old,APP_NAME);
 strmcat(&filename_old,"actual.gif");
 strmcat(&filename_new,"/%1.gif");
 strreplace(&filename_new,"%1",image);

 unlink(filename_old);

 if(symlink(filename_new,filename_old)==-1)
  outerr("Error symlinking");

 free(filename_new);
 free(filename_old);
}

/**
 * generate output
 */
void gen_output(char **weather,unsigned short int *flags,char **format_code) {
 char *buffer=NULL;
 unsigned short int counter;

 if(*flags&BIT3) symlinking_image(weather[5]);

 if(*flags&BIT2) {
	strmcat(&buffer,*format_code);
	while(strreplace(&buffer,"\\n","\n"));
 }


 else if(*flags&BIT1) { // short
  strmcat(format_code,"%lc\n%cm, %ctC°\nWind: %cws, %cwd°\n\nForecats:\n%tm\n\
Temp: min %ttlC°, max %tthC°\n");
  strmcat(&buffer,*format_code);
 }

 else { // std long output
  counter=0;
  for(;counter<26;++counter) {
   char *tmp=NULL;
   strmcat(&tmp,"%1 : %2\n");
   strreplace(&tmp,"%1",bez[counter][0]);
   strreplace(&tmp,"%2",bez[counter][1]);
   strmcat(&buffer,tmp);
   free(tmp);
  }
 }


 // inserting Weatherstats
 counter=0;
 for(;counter<26;++counter)
   strreplace(&buffer,bez[counter][1],weather[counter]);

 outplain(buffer);
 free(buffer);
}


int main(int argc,char **argv) {
 SOCKET sock;                 // vars for server communication
 BUFFER buffer=NULL;
 char *gmxxcode=NULL;         // vars for optionhandling
 char *server_name=NULL;
 char *format_code=NULL;
 unsigned short int flags=0;
 int opt;
 char *weather[26];           // weather array witch stores the data

 while((opt=getopt(argc,argv,"f:slvh")) != -1) {
  switch(opt) {
   case 'f':
    strmcat(&format_code,optarg);flags|=BIT2;break;
   case 's':
    flags|=BIT1;break;
   case 'l':
    flags|=BIT3;break;
   case 'v':
    showversion(APP_NAME,APP_VERSION);exit(0);
   case 'h':
   default:
    showhelp(APP_NAME,APP_HELP);
    exit(0);
  }
 }

 // handling gmxx codes passes throug params or reading from db
 if(optind<=argc && argc!=1) {
  if((strncmp("GMXX",argv[optind],4)==0 || strncmp("gmxx",argv[optind],4)==0)
        && strlen(argv[optind])==8) {
   strmcat(&gmxxcode,argv[optind]);
  }
#ifdef DBASE
  else {
   gmxxcode=malloc(8*sizeof(char));
   if(getGmxx(argv[optind],gmxxcode)==1) gmxxcode=NULL;
  }
#endif
  if(gmxxcode==NULL) {
   outerr("no valid gmxx code\n");
   exit(1);
  }
 } else {outerr("gmxxcode forget\n");exit(1);}

 // building string an define servername
 server_name="weather.tuxnet24.de";
 buffer="GET /?id=%d HTTP/1.1\r\nHost: %h\r\nConnection: close\r\n\r\n\0";
 strreplace(&buffer,"%d",gmxxcode);
 strreplace(&buffer,"%h",server_name);
 if(gmxxcode!=NULL) free(gmxxcode);

 opensocket(&sock,server_name,"80");
 sendsplit(&sock,&buffer);
 recvsplit(&sock,&buffer);
 closesocket(&sock);

 parsing_infos(weather,buffer);
 gen_output(weather,&flags,&format_code);
 if(buffer!=NULL) free(buffer);
 if(format_code!=NULL) free(format_code);
 exit(0);
}
