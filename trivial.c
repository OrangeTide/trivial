/* trivial.c : trivial TELNET server */
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

#include "iox.h"
#include "compiler.h"
#include "log.h"
#include "break.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <getopt.h>

static const char component[] = "main";
static int opt_verbose = 0;

static void
report(const char *msg)
{
	dprintf(STDERR_FILENO, "%s:%s\n", component, msg);
}

static const struct option long_options[] = {
	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'V'},
	{"verbose", 0, 0, 'v'},
	{"quiet", 0, 0, 'q'},
};

static void
usage(void)
{
	unsigned i;

	dprintf(STDERR_FILENO, "usage: %s [-hVvq]\n", program_invocation_short_name);
	for (i = 0; i < sizeof(long_options) / sizeof(*long_options); i++) {
		dprintf(STDERR_FILENO, "    --%-26sTBD\n", long_options[i].name);
	}
}

static int
process_args(int argc, char **argv)
{
	int longindex;
	int c;

	while(c = getopt_long(argc, argv, "hVvq", long_options, &longindex), c != -1) {
		switch (c) {
		case 'h':
			usage();
			exit(1);
		case 'V':
			dprintf(STDERR_FILENO, "%s version %s\n", program_invocation_short_name, __DATE__ " " __TIME__);
			exit(1);
		case 'v':
			if (opt_verbose >= 0)
				opt_verbose++;
			break;
		case 'q':
			opt_verbose = -1;
			break;
		}
	}

	return 0;
 }

int
main(int argc, char *argv[])
{
	if (process_args(argc, argv))
		return report("bad argument"), EXIT_FAILURE;

	if (break_init())
		return report("Unable to install signal handler"), EXIT_FAILURE;

	log_open("trivial", 0, 0);

	if (iox_init())
		return report("Unable to initialize I/O subsystem"), EXIT_FAILURE;

	struct iox_loop *main_loop;
	if (iox_loop_open(&main_loop) != IOX_OK)
		return report("cannot create event loop"), EXIT_FAILURE;

	log_info("Starting ...");
	while (!break_signaled() && iox_poll(main_loop, 1000) == 0) {
		log_info("Tick");
		// TODO: ...
	}

	if (break_signaled())
		log_info("User break signaled");

	iox_loop_close(main_loop);

	log_close();

	return 0;
}
