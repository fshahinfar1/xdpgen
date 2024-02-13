#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H
#include <linux/bpf.h>
struct xdp_config {
	__u32 ifindex_out;
	__u32 src_ip;
	__u32 dst_ip;
	__u16 src_port;
	__u16 dst_port;
	__u8  protocol;
	__u8  target_is_local;
};
#endif
