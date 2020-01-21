#ifndef NET_H_
#define NET_H_
#include "iox.h"
int net_listen(const char *hostport, void (*foreach)(SOCKET fd, void *ptr), void *ptr);
#endif
