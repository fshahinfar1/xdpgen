#ifndef RUNNER_ARGS_H
#define RUNNER_ARGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <net/if.h>
#include <assert.h>

struct parameters {
	char *binary_path;
	size_t repeat;
	char *progname;
	char *ifname;
	int ifindex;
};
extern struct parameters args;

void usage(void) {
	printf("loader:\n"
		"  --repeat     -r   [default 10^7]\n"
		"  --iface     -i    interface to use in cross test\n"
		"  --help      -h\n"
	);
}

void parse_args(int argc, char *argv[]) {
	int ret;

	struct option long_opts[] = {
		{"help",       no_argument,       NULL, 'h'},
		{"repeat",     required_argument, NULL, 'r'},
		{"iface",      required_argument, NULL, 'i'},
		/* End of option list ------------------- */
		{NULL, 0, NULL, 0},
	};

	/* Default values */
	args.repeat = 1000000;
	args.binary_path = "./build/bpf.o";
	args.progname = "prog";

	while (1) {
		ret = getopt_long(argc, argv, "hi:r:", long_opts, NULL);
		if (ret == -1)
			break;
		switch(ret) {
			case 'r':
				args.repeat = atoi(optarg);
				break;
			case 'i':
				args.ifname = strdup(optarg);
				args.ifindex = if_nametoindex(optarg);
				assert(args.ifindex > 0);
				break;
			case 'h':
				usage();
				exit(0);
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}
}
#endif
