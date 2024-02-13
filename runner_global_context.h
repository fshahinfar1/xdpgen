#ifndef RUNNER_GLOBAL_CONTEXT_H
#define RUNNER_GLOBAL_CONTEXT_H
struct program_context {
	struct bpf_object *bpfobj;
	struct bpf_program *prog;
	int prog_fd;
	double last_test_duration;
	int live;
	char src_mac[6];
	char dst_mac[6];
	int src_ip; /* network byte order */
	int dst_ip; /* network byte order */
	short src_port; /* network byte order */
	short dst_port; /* network byte order */
};
extern struct program_context context;
#endif
