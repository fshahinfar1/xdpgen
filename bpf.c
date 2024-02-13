#include <linux/bpf.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#include "shared_types.h"

char _license[] SEC("license") = "GPL";

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 1);
	__type(key, __u32);
	__type(value, struct xdp_config);
} config SEC(".maps");

SEC("xdp")
int prog(struct xdp_md *ctx)
{
	__u32 zero = 0;
	struct xdp_config *cfg = NULL;
	cfg = bpf_map_lookup_elem(&config, &zero);
	if (cfg == NULL)
		return XDP_ABORTED;
	bpf_printk("on: %d", ctx->ingress_ifindex);
	bpf_printk("here, redirect to ifindex: %d", cfg->ifindex_out);
	int ret = bpf_redirect(cfg->ifindex_out, 0);
	bpf_printk("ret: %d", ret);
	return ret;
	/* return XDP_TX; */
}
