﻿/*
 * YangUserDataConnection.cpp
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */

#include "yangutil/sys/YangLog.h"

#include <string.h>
#include <yangmeeting/YangMeetingMessageI.h>
#include <yanguserdata/YangDataChannelConnection.h>
#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)

#define sleep(n)	Sleep(n*1000)
#define msleep(n)	Sleep(n)
#define SET_RCVTIMEO(tv,s)	int32_t tv = s*1000
#else /* !_WIN32 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netdb.h>
#include <yangutil/yang_unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include "stddef.h"
#define GetSockError()	errno
#define SetSockError(e)	errno = e
#undef closesocket
#define closesocket(s)	close(s)
#define msleep(n)	yang_usleep(n*1000)
#define SET_RCVTIMEO(tv,s)	struct timeval tv = {s,0}
#endif
#include <yangutil/yang_unistd.h>
#include "fcntl.h"

YangDataChannelConnection::YangDataChannelConnection() {
	m_loop = 0;
	m_socket = 0;
	m_isStart = 0;
    memset(m_heartBuf,0,3);
}

YangDataChannelConnection::~YangDataChannelConnection() {

}

int32_t YangDataChannelConnection::init(char *serverip, int32_t pport) {
	//printf("\nYangDataChannelConnection..........server=%s,port=%d", serverip,pport);
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif

m_serverAddress.sin_family = AF_INET;
m_serverAddress.sin_port = htons(pport);
#ifdef _WIN32
    m_serverAddress.sin_addr.S_un.S_addr=inet_addr(serverip);
#else
    m_serverAddress.sin_addr.s_addr = inet_addr(serverip);

#endif
	return Yang_Ok;
}

int32_t YangDataChannelConnection::connectServer() {
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int32_t on = 1;

	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on));
#ifndef _WIN32
    struct timeval tv = { 1, 0 };
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(tv));
#endif


    if (connect(m_socket, (sockaddr*) &m_serverAddress, sizeof(sockaddr)) == -1) {
		closesocket(m_socket);
        //return ERROR_SOCKET;

       return yang_error_wrap(GetSockError(),"user datachannel socket error");
	}

	return Yang_Ok;
}

int32_t YangDataChannelConnection::disConnectServer() {
	closesocket(m_socket);
	return Yang_Ok;
}

int32_t YangDataChannelConnection::publish(uint8_t *p, int32_t plen) {
    int32_t nBytes = send(m_socket,(char*) p, plen, 0);
	if (nBytes < 0) {

        yang_error("%s,  send user data error %d (%d bytes)", __FUNCTION__, GetSockError(),
				plen);
		return ERROR_SOCKET_Close_Wr;
	}
	return Yang_Ok;
}

void YangDataChannelConnection::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangDataChannelConnection::stop() {
	stopLoop();
}
void YangDataChannelConnection::stopLoop() {
	m_loop = 0;
}
int32_t YangDataChannelConnection::sendHeartbeat(){
    return publish(m_heartBuf,3);
}
void YangDataChannelConnection::startLoop() {
	m_loop = 1;
	int32_t nBytes = 0;
	int32_t bufLen = 1024 * 8;
    uint8_t* buf=new uint8_t[bufLen];
    YangAutoFreeA(uint8_t,buf);
	while (m_loop) {
        nBytes = recv(m_socket,(char*) buf, bufLen, 0);
		if (nBytes > 0) {
			decodeMessage(buf, nBytes);
		} else if (nBytes == -1) {
			int32_t sockerr = GetSockError();
			if (sockerr == EINTR)
				continue;
			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN) {
				nBytes = 0;
				continue;
			}
			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
					__FUNCTION__, nBytes, sockerr, strerror(sockerr));
			m_loop = 0;
			yang_post_message(YangM_Sys_ConnectServerInterrupt, 0, NULL);
			break;
		} else if (nBytes == 0) {
			m_loop = 0;
			yang_post_message(YangM_Sys_ConnectServerInterrupt, 0, NULL);
			break;
		}
	}
	closesocket(m_socket);
  //  m_socket=INVALID_SOCKET;
}
