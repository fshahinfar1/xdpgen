#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H
#include <linux/bpf.h>
struct xdp_config {
	__u32 ifindex_out;
};
#endif
