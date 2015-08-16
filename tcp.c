/*
 * common_tcp.c - wrapper for TCP programming
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

#include "tcp.h"

#ifndef __STRING_H__
#include "string.h"
#endif

#ifndef __OUTPUT_H__
 #include "output.h"
#endif

#include <arpa/inet.h>
#include <netdb.h>


void exit(int);
long int strtol(const char*,char**,int);

/**
 * function to wraparound gethostbyname_r to easy use like gethostbyname()
 */
struct hostent *getHostByName(char *host){
 struct hostent hostbuf;
 struct hostent *hp;
 size_t hstbuflen;
 char *tmphstbuf;
 int res;
 int herr;

 hstbuflen=1024;
 tmphstbuf=malloc(hstbuflen);

 while((res=gethostbyname_r(host,&hostbuf,tmphstbuf,hstbuflen,&hp,&herr))!=0) {
  hstbuflen*=2;
  tmphstbuf=realloc(tmphstbuf,hstbuflen);
 }

 if(res||hp==NULL) return NULL;

 free(tmphstbuf);
 return hp;
}


void openSock(SOCKET *sock,ADDRESS server_name,PORT port) {
 struct sockaddr_in server;
 struct hostent *host_info;
 unsigned long addr;

 *sock=socket(PF_INET,SOCK_STREAM,0);
 if(*sock<0){
  outerr("failed to create socket\n");
  exit(1);
 }

 memmset(&server,0,sizeof(server));
 if((addr=inet_addr(server_name))!=INADDR_NONE) {
  memcpy((char *)&server.sin_addr,&addr,sizeof(addr));
 } else {
  host_info=getHostByName(server_name);
  if(NULL==host_info) {
   outerr("unknown server\n");
   exit(1);
  }
  memcpy((char *)&server.sin_addr,host_info->h_addr,host_info->h_length);
 }

 server.sin_family=AF_INET;
 server.sin_port=htons(strtol(port,&port,0));

 if(connect(*sock,(struct sockaddr*)&server,sizeof(server))<0) {
  outerr("can't connect to server\n");
  exit(1);
 }
}

void closeSock(SOCKET *sock) {shutdown(*sock,2);}

void sendSplit(SOCKET *sock,MESSAGE *mesg) {
 char *pointer;
 unsigned int iterrations=0;
 char *temp;
 int errorSend;

 iterrations=strmlen((char *)*mesg)/DATA_SIZE_TCP;
 if(strmlen((char *)*mesg)%DATA_SIZE_TCP>0)
  iterrations++;

 pointer=*mesg;
 while(iterrations>0) {
  if(strmlen(pointer)+1>=DATA_SIZE_TCP) {
   temp = malloc(DATA_SIZE_TCP);
   memcpy(temp,pointer,DATA_SIZE_TCP);
   errorSend=send(*sock,temp,DATA_SIZE_TCP,0);
  } else {
   temp = malloc(strmlen(pointer)+1);
   memcpy(temp,pointer,strmlen(pointer)+1);
   errorSend=send(*sock,temp,strmlen(pointer)+1,0);
  }
  if(errorSend==-1) outerr("send to failure");

  --iterrations;
  if(iterrations!=0) pointer=pointer+DATA_SIZE_TCP;
  free(temp);
 }
}

void recvSplit(SOCKET *sock,MESSAGE *mesg) {
 char temp[DATA_SIZE_TCP];
 int bytes;
 char *pointer;
 unsigned long int recB=0;

 if(*mesg==0) free(*mesg);
 *mesg=malloc(1);
 pointer=*mesg;

 do {
  bytes=recv(*sock,&temp,DATA_SIZE_TCP,0);
  if(bytes==-1) outerr("server error");
  if(bytes==0) return;
  recB+=bytes;
  *mesg=(char *)realloc(*mesg,recB);
  if(*mesg==0) outerr("realoc failed");
  pointer=*mesg+recB-bytes;
  memcpy(pointer,&temp[0],bytes);
 } while(bytes>0);
}

void fetchFromHeader(char *header,char *key, char **value) {
 char *fetchingkeypos;

 fetchingkeypos=strmstr(header,key);
 if(fetchingkeypos==NULL) outerr("key does not exists");

 fetchingkeypos=strmstr(fetchingkeypos,": ")+2;
 if(fetchingkeypos==NULL) outerr("no standard formating");

 *strmstr(fetchingkeypos,"\r\n")='\0';
 strmcat(value,fetchingkeypos);
}
