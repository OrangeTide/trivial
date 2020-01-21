/* iox_unix.c : common Unix routines for IOX */
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
#include "grow.h"

struct fd_entry {
	void *ptr;
	int refcount;
	void (*free_cb)(void *ptr);
	struct iox_ops ops;
};

static unsigned fd_entry_len;
static struct fd_entry *fd_entry;

static struct fd_entry *
find_entry(SOCKET fd)
{
	if (fd > 0 && (unsigned)fd < fd_entry_len)
		return &fd_entry[fd];
	return NULL;
}

static int
expand(SOCKET fd)
{
	unsigned n = fd_entry_len;

	if (fd < 0)
		return IOX_ERR; /* invalid parameter */

	if (!n)
		n = 16;
	while ((unsigned)fd >= n) {
		n = n << 1;
	}

	return grow(&fd_entry, &fd_entry_len, n, sizeof(*fd_entry));
}

static struct fd_entry *
create_entry(SOCKET fd)
{
	if (fd < 0)
		return NULL;

	if ((unsigned)fd >= fd_entry_len) {
		if (expand(fd))
			return NULL;
	}

	return find_entry(fd);

}

int
iox_register(SOCKET fd, const struct iox_ops *ops, void *ptr, void (*free_cb)(void *))
{
	struct fd_entry *ent = create_entry(fd);
	if (!ent)
		return IOX_ERR; /* could not create */

	if (ent->refcount)
		return IOX_ERR; /* already in use */

	ent->ptr = ptr;
	ent->free_cb = free_cb;
	ent->refcount = 1;
	ent->ops = *ops;

	return IOX_OK;
}

static void
fd_retain(struct fd_entry *ent)
{
	ent->refcount++;
}

static void
fd_release(struct fd_entry *ent, SOCKET fd)
{
	/* free the entry if it is the last one */
	if (ent->refcount == 1) {
		if (ent->free_cb)
			ent->free_cb(ent->ptr);
		ent->ptr = NULL;
		iox_socket_close(fd);
	}

	ent->refcount--;
}

int
iox_retain(SOCKET fd)
{
	struct fd_entry *ent = find_entry(fd);
	if (!ent)
		return IOX_ERR; /* could not create */
	if (!ent->refcount)
		return IOX_ERR; /* not initialized */

	fd_retain(ent);

	return IOX_OK;
}

int
iox_release(SOCKET fd)
{
	struct fd_entry *ent = find_entry(fd);
	if (!ent)
		return IOX_ERR; /* could not create */
	if (!ent->refcount)
		return IOX_ERR; /* not initialized */

	fd_release(ent, fd);

	return IOX_OK;
}

int
iox_call(SOCKET fd, unsigned iox_event_flags)
{
	struct fd_entry *ent = find_entry(fd);

	if (!ent)
		return IOX_ERR;
	if (!ent->refcount)
		return IOX_ERR; /* not initialized */

	fd_retain(ent);
	if (ent->ops.event_cb)
		ent->ops.event_cb(fd, iox_event_flags, ent->ptr);
	fd_release(ent, fd);

	return IOX_OK;
}
