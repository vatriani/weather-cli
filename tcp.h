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

#ifndef __TCP_H__
#define __TCP_H__

// define some Datatypes for easy reading
typedef int SOCKET;
typedef char* ADDRESS;
typedef char* PORT;
typedef char* BUFFER;
typedef char* MESSAGE;

#define DATA_SIZE_TCP 1452 // from standart


/**
 * function to wraparound gethostbyname_r to easy use like gethostbyname()
 */
struct hostent *getHostByName(char *host);

void openSock(SOCKET *sock,ADDRESS server_name,PORT port);
void closeSock(SOCKET *sock);
void sendSplit(SOCKET *sock,MESSAGE *mesg);
void recvSplit(SOCKET *sock,MESSAGE *mesg);
void fetchFromHeader(char *header,char *key, char **value);

#endif
