# What is it?

Generate UDP traffic using `BPF_PROG_TEST_RUN` and `XDP`. It is a simple
replications of `xdp-trafficgen` [1]. The idea was to get familiar with the
technique and also a simpler version that is easier to adjust for different
possible needs.

[1] https://github.com/xdp-project/xdp-tools/tree/master/xdp-trafficgen

## Running

> The source/destination address are hard-coded in `runner.c::main()`

```
make
sudo ./build/runner -i <iface>
```

## Requirements

It requires a new kernel. It did not worked on v5.15. I tested with 6.7.3 and
libbpf v1.0.0.
