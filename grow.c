/* grow.c : dynamically resize allocated arrays */
/* Copyright (c) 2013-2015, 2020 Jon Mayo <jon@rm-f.net>
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

#include "grow.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int
grow(void *ptr, unsigned *max, unsigned min, size_t elem)
{
	size_t old = *max * elem;
	size_t new = min * elem;
	char *p;

	if (new <= old) {
		return 0;
	}
	/* round up to next power-of-2 bigger than a pointer */
	new += sizeof(void*);
	new--;
	new |= new >> 1;
	new |= new >> 2;
	new |= new >> 4;
	new |= new >> 8;
	new |= new >> 16;
	new++;
	new -= sizeof(void*);
	*max = new / elem;
	assert(ptr != NULL);
	assert(old <= new);
	p = realloc(*(char**)ptr, new);
	if (!p) {
		perror(__func__);
		return -1;
	}
	memset(p + old, 0, new - old);
	*(void**)ptr = p;
	assert(*(void**)ptr != NULL);
	return 0;
}
