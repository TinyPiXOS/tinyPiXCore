/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#ifndef __NET_H
#define __NET_H

#include "typesDef.h"

//for sever host
#ifndef INADDR_ANY
#define INADDR_ANY		0x00000000
#endif

#ifndef INADDR_NONE
#define INADDR_NONE		0xFFFFFFFF
#endif

TP_DEF_VOID_TYPE_VAR(IPiAddress);
TP_DEF_VOID_TYPE_VAR(IPiTCPSocket);
TP_DEF_VOID_TYPE_VAR(IPiSocketSet);
TP_DEF_VOID_TYPE_VAR(IPiSocket);
TP_DEF_VOID_TYPE_VAR(IPiSocketVector);

#ifdef __cplusplus
extern "C"{
#endif

/**
*construct a simple vector to store socket point
*/
extern DECLSPEC IPiSocketVector * STDCALL socket_vector_create(IPiSocketSet *set);

/**
*push a socket
*/
extern DECLSPEC tpBool STDCALL socket_vector_push(IPiSocketVector *vector, IPiTCPSocket *sock);

/**
*del a socket
*/
extern DECLSPEC tpBool STDCALL socket_vector_del(IPiSocketVector *vector, IPiTCPSocket *sock);

/**
*get a socket
*/
extern DECLSPEC IPiTCPSocket * STDCALL socket_vector_at(IPiSocketVector *vector, int index);

/**
*clear vector
*/
extern DECLSPEC tpBool STDCALL socket_vector_clear(IPiSocketVector *vector);

/**
*get lenght for vector
*/
extern DECLSPEC unsigned int STDCALL socket_vector_length(IPiSocketVector *vector);

/**
*free vector
*/
extern DECLSPEC void STDCALL socket_vector_free(IPiSocketVector *vector);

/**
*create IPiAddress
*/
extern DECLSPEC IPiAddress * STDCALL ipaddress_create(const char *host, unsigned short port);

/**
*free IPiAddress, onley validate for ipaddress_create
*/
extern DECLSPEC void STDCALL ipaddress_free(IPiAddress *ipaddress);

/**
*create TCPSocket via IP
*/
extern DECLSPEC IPiTCPSocket * STDCALL tcpsocket_create(IPiAddress *ip);

/**
*get connect socket ip address
*/
extern DECLSPEC const char * STDCALL resolve_ipaddress(IPiAddress *ip);

/**
*create TCPSocket via domain socket
*/
extern DECLSPEC IPiTCPSocket * STDCALL domainsocket_create(char *filename, tpBool is_server);

/**
*accept TCPSocket
*/
extern DECLSPEC IPiTCPSocket * STDCALL tcpsocket_accept(IPiTCPSocket *server);

/**
*get TCPSocket peer address
*/
extern DECLSPEC IPiAddress * STDCALL tcpsocket_get_peer_address(IPiTCPSocket *sock);

/**
*send data via tcp sock
*/
extern DECLSPEC tpBool STDCALL tcpsocket_send(IPiTCPSocket *sock, const void *data, int len);

/**
*recv data vid tcp sock
*/
extern DECLSPEC tpBool STDCALL tcpsocket_recv(IPiTCPSocket *sock, void *data, int maxlen);

/**
*close tcp sock
*/
extern DECLSPEC void STDCALL tcpsocket_close(IPiTCPSocket *sock);

/* generate a sock set
 */
extern DECLSPEC IPiSocketSet * STDCALL socketset_create(int maxsockets);

/* Add a socket to a set of sockets to be checked for available data */
extern DECLSPEC int STDCALL socketset_add(IPiSocketSet *set, IPiTCPSocket *sock);

/* Remove a socket from a set of sockets to be checked for available data */
extern DECLSPEC int STDCALL socketset_del(IPiSocketSet *set, IPiTCPSocket *sock);

/* check sock existed */
extern DECLSPEC tpBool STDCALL socketset_exist(IPiSocketSet *set, IPiTCPSocket *sock);

/* check a sock if exist, or renturn false
*/
extern DECLSPEC tpBool STDCALL socketset_check(IPiSocketSet *set, IPiSocketVector *vecotr, unsigned int timeout);

/**
*confirm a socket is active
*/
extern DECLSPEC tpBool STDCALL socketset_active(IPiTCPSocket *sock);

/**
*free socketset
*/
extern DECLSPEC tpBool STDCALL socketset_free(IPiSocketSet *set);

#ifdef __cplusplus
}
#endif

#endif
