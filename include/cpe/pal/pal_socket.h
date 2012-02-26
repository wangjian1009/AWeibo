#ifndef CPE_PAL_SOCKET_H
#define CPE_PAL_SOCKET_H

#ifdef _MSC_VER

#include <WinSock.h>

#define cpe_sock_close closesocket
#define cpe_sock_errno() WSAGetLastError()
#define cpe_sock_errstr() ""

#else

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define cpe_sock_close close
#define cpe_sock_errno() errno
#define cpe_sock_errstr() strerror(errno)

#endif

int cpe_sock_set_block(int fd, int is_non_block);

#endif
