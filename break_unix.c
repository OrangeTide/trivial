/* break_unix.c : process Unix signals for termination and Ctrl-C */
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

#include "break.h"
#include "compiler.h"
#include <signal.h>

static volatile sig_atomic_t signal_flag;

static void
int_handler(int unused UNUSED)
{
	signal_flag = 1;
}

/*
static void
quit_handler(int unused UNUSED)
{
	signal_flag = 1;
}
*/

static void
term_handler(int unused UNUSED)
{
	signal_flag = 1;
}

int
break_init(void)
{
	signal_flag = 0;
	if (signal(SIGINT, int_handler) == SIG_ERR)
		return -1;
/*
	if (signal(SIGQUIT, quit_handler) == SIG_ERR)
		return -1;
*/
	if (signal(SIGTERM, term_handler) == SIG_ERR)
		return -1;
	return 0;
}

int
break_signaled(void)
{
	return signal_flag;
}

void
break_clear(void)
{
	signal_flag = 0;
}
