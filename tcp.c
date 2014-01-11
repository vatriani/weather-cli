/*
 * tcp.c - wrapper for TCP programming
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

#ifndef __TCP_H__
#define __TCP_H__

#ifndef __OUTPUT_H__
 #include "output.c"
#endif
#ifndef __STRING_H__
 #include "string.c"
#endif

#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

// define some Datatypes for easy reading
typedef int SOCKET;		// Socket
typedef char* ADDRESS;	// Server address
typedef char* PORT;		// Server port
typedef char* BUFFER;	// buffer for messages
typedef char* MESSAGE;

#define DATA_SIZE_TCP 1452 // from standart


/**
 * function to wraparound gethostbyname_r to easy use like gethostbyname()
 */
static struct hostent *getHostByName(char *host){
 struct hostent hostbuf;
 struct hostent *hp;
 size_t hstbuflen;
 char *tmphstbuf;
 int res;
 int herr;

 tmphstbuf=malloc(DATA_SIZE_TCP);

 while((res=gethostbyname_r(host,&hostbuf,tmphstbuf,DATA_SIZE_TCP,&hp,&herr))!=0) {
  hstbuflen*=2;
  tmphstbuf=realloc(tmphstbuf,DATA_SIZE_TCP);
 }

 if(res||hp==NULL) return NULL;

 free(tmphstbuf);
 return hp;
}

/**
 * opening new socket to a specified server
 */
void opensocket(SOCKET *sock,ADDRESS server_name,PORT port) {
 struct sockaddr_in server;
 struct hostent *host_info;
 unsigned long addr;

 *sock=socket(PF_INET,SOCK_STREAM,0);
 if(*sock<0) outfatal("failed to create socket\n");

 memset(&server,0,sizeof(server));
 if((addr=inet_addr(server_name))!=INADDR_NONE) {
  memcpy((char *)&server.sin_addr,&addr,sizeof(addr));
 } else {
  host_info=getHostByName(server_name);
  if(NULL==host_info) outfatal("unknown server");
  memcpy((char *)&server.sin_addr,host_info->h_addr,host_info->h_length);
 }

 server.sin_family=AF_INET;
 server.sin_port=htons(strtol(port,&port,0));

 if(connect(*sock,(struct sockaddr*)&server,sizeof(server))<0)
  outfatal("can't connect to server");
}

/**
 * closes socket
 */
void closesocket(SOCKET *sock) {close(*sock);}

/**
 * send message to server as splittet packages
 */
void sendsplit(SOCKET *sock,MESSAGE *mesg) {
 char *pointer;
 unsigned int iterrations=0;
 char *temp;
 int errorSend;

 iterrations=strlen((char *)*mesg)/DATA_SIZE_TCP;
 if(strlen((char *)*mesg)%DATA_SIZE_TCP>0)
  iterrations++;

 pointer=*mesg;
 while(iterrations>0) {
  if(strlen(pointer)+1>=DATA_SIZE_TCP) {
   temp = malloc(DATA_SIZE_TCP);
   memcpy(temp,pointer,DATA_SIZE_TCP);
   errorSend=send(*sock,temp,DATA_SIZE_TCP,0);
  } else {
   temp = malloc(strlen(pointer)+1);
   memcpy(temp,pointer,strlen(pointer)+1);
   errorSend=send(*sock,temp,strlen(pointer)+1,0);
  }
  if(errorSend==-1) outfatal("send to failure");

  --iterrations;
  if(iterrations!=0) pointer=pointer+DATA_SIZE_TCP;
  free(temp);
 }
}

/**
 * fetches data from server as default tcp package frame size
 */
void recvsplit(SOCKET *sock,MESSAGE *mesg) {
 char temp[DATA_SIZE_TCP];
 int bytes;
 char *pointer;
 unsigned long int recB=0;

 if(*mesg==0) free(*mesg);
 *mesg=malloc(1);
 pointer=*mesg;

 do {
  bytes=recv(*sock,&temp,DATA_SIZE_TCP,0);
  if(bytes==-1) outfatal("server error");
  if(bytes==0) return;
  recB+=bytes;
  *mesg=(char *)realloc(*mesg,recB);
  if(*mesg==0) outfatal("realoc failed");
  pointer=*mesg+recB-bytes;
  memcpy(pointer,&temp[0],bytes);
 } while(bytes>0);
}

/**
 * parsing header informations and get an special key from it
 */
void fetchfromheader(char  *header,char *key, char **value) {
 char *fetchingkeypos;

 fetchingkeypos=strstr(header,key);
 if(fetchingkeypos==NULL) outfatal("key does not exists");

 fetchingkeypos=strstr(fetchingkeypos,": ")+2;
 if(fetchingkeypos==NULL) outfatal("no standard formating");

 *strstr(fetchingkeypos,"\r\n")='\0';
 strmcat(value,fetchingkeypos);
}
#endif
