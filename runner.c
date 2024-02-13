/*
 * This program relies on the BPF_PROG_TEST_RUN.
 * */
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <net/if.h> /* if_nametoindex */
#include <linux/bpf.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>

#include "runner_args.h"
#include "runner_global_context.h"
#include "bpf_stats.h"
#include "bpf_loader_helpers.h"

#include "shared_types.h"

struct parameters args = {};
struct program_context context = {};

#define MAX_BUF 4096
#define DATA_OFFSET (sizeof(struct ethhdr) + sizeof(struct iphdr) + \
		sizeof(struct udphdr))

int run()
{
	context.live = 1;
	char *payload = "this is a test\n";
	char *output = calloc(1, MAX_BUF);

	const int key = 0;
	struct bpf_map *m;
	assert (args.ifindex > 0);
	struct xdp_config cfg = {
		.ifindex_out = args.ifindex,
		.src_ip = context.src_ip,
		.dst_ip = context.dst_ip,
		.src_port = context.src_port,
		.dst_port = context.dst_port,
		.protocol = IPPROTO_UDP,
		.target_is_local = 0,
	};
	if (args.local) {
		cfg.target_is_local = 1;
	}
	m = bpf_object__find_map_by_name(context.bpfobj, "config");
	bpf_map__update_elem(m, &key, sizeof(key), &cfg, sizeof(cfg), BPF_ANY);

	attach_xdp_program();
	send_payload(context.prog_fd, payload, output, MAX_BUF);
	/* sleep(300); */
	detach_xdp_program();
	return 0;
}

void interrupt_handler(int sig)
{
	if (args.ifindex != 0)
		detach_xdp_program();
	bpf_object__close(context.bpfobj);
	exit(EXIT_FAILURE);
}

#define SET_MAC(ptr, a,b,c,d,e,f) {char tmp[]={a,b,c,d,e,f};memcpy(ptr, tmp, 6);}

int main(int argc, char *argv[])
{
	cpu_set_t cpu_cores;
	CPU_ZERO(&cpu_cores);
	CPU_SET(0, &cpu_cores);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_cores);
	parse_args(argc, argv);
	printf("BPF binary: %s\n", args.binary_path);
	load_bpf_binary_and_get_program();
	printf("Program fd: %d\n", context.prog_fd);
	signal(SIGINT, interrupt_handler);

	SET_MAC(context.src_mac, 0x9c,0xdc,0x71,0x56,0xbf,0x25);
	SET_MAC(context.dst_mac, 0x9c,0xdc,0x71,0x4b,0x83,0x61);
	/* SET_MAC(context.src_mac, 0x00,0x00,0x00,0x00,0x00,0x00); */
	/* SET_MAC(context.dst_mac, 0x00,0x00,0x00,0x00,0x00,0x00); */
	inet_pton(AF_INET, "192.168.1.2", &context.src_ip);
	inet_pton(AF_INET, "192.168.1.1", &context.dst_ip);
	/* inet_pton(AF_INET, "127.0.0.2", &context.src_ip); */
	/* inet_pton(AF_INET, "127.0.0.1", &context.dst_ip); */
	context.src_port = htons(50123);
	context.dst_port = htons(8080);

	run();
	bpf_object__close(context.bpfobj);
	return 0;
}
