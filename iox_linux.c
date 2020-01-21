/* iox_linux.c : epoll routines for IOX */
/* Copyright (c) 2020 Jon Mayo <jon@rm-f.net>
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

#include "iox.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include <unistd.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

struct iox_loop {
	struct epoll_event ev, events[MAX_EVENTS];
	int epollfd;
};

static void
ignore_sigpipe(void)
{
	sigset_t m;

	sigemptyset(&m);
	sigaddset(&m, SIGPIPE);
	sigprocmask(SIG_BLOCK, &m, 0);
}

static unsigned
get_iox_event_flags(unsigned epoll_flags)
{
	unsigned iox_flags;

	iox_flags = epoll_flags & EPOLLIN ? IOX_EV_READ : 0;
	iox_flags |= epoll_flags & EPOLLOUT ? IOX_EV_WRITE : 0;

	return iox_flags;
}

static unsigned
get_epoll_event_flags(unsigned iox_flags)
{
	unsigned epoll_flags;

	epoll_flags = iox_flags & IOX_EV_READ ? EPOLLIN : 0;
	epoll_flags |= iox_flags & IOX_EV_WRITE ? EPOLLOUT : 0;

	return epoll_flags;
}

int
iox_init(void)
{
	ignore_sigpipe();

	return IOX_OK;
}

int
iox_loop_open(iox_loop_t **loop)
{
	iox_loop_t *l = calloc(1, sizeof(*l));

	l->epollfd = epoll_create1(EPOLL_CLOEXEC);
	if (l->epollfd == -1) {
	    perror("epoll_create1");
	    free(l);
	    return -1;
	}

	*loop = l;

	return 0;
}

void
iox_loop_close(iox_loop_t *loop)
{
	if (loop && loop->epollfd != -1) {
		close(loop->epollfd);
		loop->epollfd = -1;
	}
}

int
iox_poll(iox_loop_t *loop, int msec)
{
	int nfds, n;

	for (;;) {
		nfds = epoll_wait(loop->epollfd, loop->events, MAX_EVENTS, msec);
		if (nfds == -1) {
			if (errno == EINTR)
				return 0;
			perror("epoll_wait");
			return -1;
		}

		for (n = 0; n < nfds; n++) {
			SOCKET fd = loop->events[n].data.fd;
			unsigned events = get_iox_event_flags(loop->events[n].events);
			iox_call(fd, events);
		}
	}

	return 0;
}

int
iox_event_add(iox_loop_t *loop, SOCKET fd, unsigned iox_event_flags)
{
	struct epoll_event ev;

	ev.events = get_epoll_event_flags(iox_event_flags);
	ev.data.fd = fd;

	if (epoll_ctl(loop->epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
	    perror("epoll_ctl: adding socket");
	    return -1;
	}

	return 0;
}

int
iox_event_del(iox_loop_t *loop, SOCKET fd)
{
	struct epoll_event ev = {0, {0} }; /* hack around older linux bug */

	if (epoll_ctl(loop->epollfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
		perror("epoll_ctl: removing socket");
		return -1;
	}

	return 0;
}

int
iox_socket_close(SOCKET fd)
{
	if (fd == -1)
		return -1;
	if (close(fd))
		return perror("close"), -1;

	return 0;
}
