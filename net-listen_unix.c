/* net-listen_unix.c : network socket routines for binding and listening */
/*
 * Copyright (c) 2020 Jon Mayo <jon@rm-f.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "net.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define NET_LISTEN_QUEUE 20

static const char component[] = "net";

static void
report(const char *msg)
{
	dprintf(STDERR_FILENO, "%s:%s\n", component, msg);
}

static int
setup_nonblocking(SOCKET fd)
{
	return fcntl(fd, F_SETFL, O_NONBLOCK);
}

static SOCKET
net_bind_listen(struct addrinfo *ai)
{
	int res;
	SOCKET fd;

	fd = socket(ai->ai_family, ai->ai_socktype, 0);
	if (fd == -1)
		return perror("socket()"), -1;

	if (ai->ai_family == AF_INET || ai->ai_family == AF_INET6) {
		const int yes = 1;
		struct linger li = {
			.l_onoff = 0, /* disable linger, except for exit() */
			.l_linger = 10, /* 10 seconds */
		};

		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&yes, sizeof(yes));
		setsockopt(fd, SOL_SOCKET, SO_LINGER, (const void*)&li, sizeof(li));
	}

	res = bind(fd, ai->ai_addr, (socklen_t)ai->ai_addrlen);
	if (res) {
		perror("bind");
		close(fd);
		return -1;
	}

	res = setup_nonblocking(fd);
	if (res) {
		perror("O_NONBLOCK");
	}

	res = listen(fd, NET_LISTEN_QUEUE);
	if (res) {
		perror("listen");
		close(fd);
		return -1;
	}

	return fd;
}

int
net_listen(const char *hostport, void (*foreach)(SOCKET fd, void *ptr), void *ptr)
{
	int res;
	struct addrinfo *ai_res, *curr;
	struct addrinfo ai_hints[] = {
		{
			.ai_flags = AI_PASSIVE | AI_V4MAPPED | AI_ADDRCONFIG,
			.ai_family = AF_UNSPEC,
			.ai_protocol = 0,
			.ai_socktype = SOCK_STREAM,
			.ai_next = NULL
		},
	};
	SOCKET fd;
	char hostbuf[64];
	const char *host, *port;

	if (hostport && *hostport) {
		/* parse the host:port */
		port = strrchr(hostport, '/');
		if (port) {
			if (hostport == port) { /* no host specified: "/yourport" */
				host = NULL;
			} else { /* both specified: "yourhost/yourport" */
				snprintf(hostbuf, sizeof(hostbuf), "%.*s", (int)(port - hostbuf), hostport);
				host = hostbuf;
			}
			port++;
		} else { /* no port specified: "yourhost" */
			host = hostport;
			port = NULL;
		}
	} else { /* neither specified - NULL or empty string */
		host = port = NULL;
	}

	res = getaddrinfo(host, port, ai_hints, &ai_res);
	if (res != 0) {
		report(gai_strerror(res));
		return -1;
	}

	/* looks for the first AF_INET or AF_INET6 entry */
	for (curr = ai_res; curr; curr = curr->ai_next) {
		if (curr->ai_family == AF_INET6 || curr->ai_family == AF_INET) {
			break;
		}
	}

	if (!curr) {
		freeaddrinfo(ai_res);
		report("unable to find interface");
		return -1; /* failure */
	}

	fd = net_bind_listen(curr);
	if (fd == -1) {
		freeaddrinfo(ai_res);
		report("unable to bind address");
		return -1; /* failure */
	}

	// TODO: run this against every possible entry, not just the first
	foreach(fd, ptr);

	freeaddrinfo(ai_res);

	return 0;
}
