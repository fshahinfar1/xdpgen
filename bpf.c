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
	struct xdp_config *cfg = NULL;
	void *data = (void *)(__u64)ctx->data;
	void *data_end = (void *)(__u64)ctx->data_end;
	struct ethhdr *eth = data;
	struct iphdr *ip = (void *)(eth + 1);
	struct udphdr *udp = (void *)(ip + 1);

	if ((void *)(udp + 1) > data_end)
		return XDP_PASS;

	__u32 zero = 0;
	cfg = bpf_map_lookup_elem(&config, &zero);
	if (cfg == NULL)
		return XDP_ABORTED;

	/* Check if the packet is sent from the traffic generator to be sent
	 * toward DUT */
	if (eth->h_proto != bpf_htons(ETH_P_IP))
		return XDP_PASS;
	if (ip->protocol != cfg->protocol)
		return XDP_PASS;
	if (ip->saddr != cfg->src_ip ||
			ip->daddr != cfg->dst_ip ||
			udp->source != cfg->src_port ||
			udp->dest != cfg->dst_port)
		return XDP_PASS;

	/* bpf_printk("on: %d", ctx->ingress_ifindex); */
	/* bpf_printk("here, redirect to ifindex: %d", cfg->ifindex_out); */
	int ret = bpf_redirect(cfg->ifindex_out, 0);
	/* bpf_printk("ret: %d", ret); */
	return ret;
	/* return XDP_TX; */
}
