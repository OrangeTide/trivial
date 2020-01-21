/* log.c : logging system */
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

#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static const char *tag[] = {
	"EMERG", "ALERT", "CRIT", "ERR",
	"WARNING", "NOTICE", "INFO", "DEBUG"
};

static void
print_time(void)
{
	time_t now;
	struct tm *tm;
	char buf[64];

	now = time(0);
	tm = localtime(&now);
	if (!strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm))
		fputs("UNKNOWN", stderr);
	else
		fputs(buf, stderr);
}

void
log_open(const char *ident, int option, int facility)
{
	// TODO: implement this
}

void
log_close(void)
{
	// TODO: implement this
}

void
log_generic(enum log_level level, const char *fmt, ...)
{
	va_list ap;

	print_time();
	fputc(' ', stderr);
	fputs(tag[level], stderr);
	fputc(':', stderr);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
}
