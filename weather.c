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
#define APP_VERSION "1.5"
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

static char *bez[]={"longitude","latitude","city","region","country",
"humidity","visibility","pressure","rising","sunrise","sunset","current code",
"current date","current temp","current text","current image","speed","direction",
"chill","tomorrow day","tomorrow date","tomorrow temp low","tomorrow temp high",
"tomorrow text","tomorrow code","tomorrow image"};

static char *format[]={"%lo","%la","%lc","%lr","%lR","%hu","%vi","%pr","%ri",
"%sr","%ss","%cc","%cd","%ct","%cm","%ci","%cws","%cwd","%cwc","%td","%tD",
"%ttl","%tth","%tT","%tc","%ti"};


/**
 * fetching infos of buffer and copying it into array
 */
void parsing_infos(char **weather,char *input_buffer) {
 char *line[80];
 unsigned int i=0;
 unsigned short int counter=0;

 line[0]=strtok(input_buffer,"=");
 while(line[i]) {
  line[++i]=strtok(NULL,"=");
  strdelsp(line[i]);
 }

 for(;counter<26;++counter)  {
  weather[counter]=malloc(strlen(line[3+counter])*sizeof(char));
  memcpy(weather[counter],line[3+counter],strlen(line[3+counter])+1);
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

 if(*flags&BIT3) symlinking_image(weather[11]);

 if(*flags&BIT2) strmcat(&buffer,*format_code);
 else if(*flags&BIT1) { // short
  strmcat(format_code,"%lc\n%cm, %ctC°\nWind: %cws, %cwd°\n\nForecats:\n%tT\n\
Temp: min %ttlC°, max %tthC°\n");
  strmcat(&buffer,*format_code);
 } else { // std long output
  counter=0;
  for(;counter<26;++counter) {
   char *tmp="%1 : %2\n";
   strreplace(&tmp,"%1",bez[counter]);
   strreplace(&tmp,"%2",format[counter]);
   strmcat(&buffer,tmp);
   free(tmp);
  }
 }

 // inserting Weatherstats
 counter=0;
 for(;counter<26;++counter)
   strreplace(&buffer,format[counter],weather[counter]);

 outplain(buffer);
 free(buffer);
}


int main(int argc,char **argv) {
 SOCKET sock;                 // vars for server communication
 BUFFER buffer;
 char *gmxxcode=NULL;         // vars for optionhandling
 char *server_name;
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
    showVersion(APP_NAME,APP_VERSION);exit(0);
   case 'h':
   default:
    showHelp(APP_NAME,APP_HELP);
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
 free(gmxxcode);

 openSock(&sock,server_name,"80");
 send_split(&sock,&buffer);
 recv_split(&sock,&buffer);
 closeSocket(&sock);

 parsing_infos(weather,buffer);
 gen_output(weather,&flags,&format_code);
 free(buffer);
 exit(0);
}
