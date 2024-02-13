CLANG := clang-11
LLC := llc-11
CC := $(CLANG)
CFLAGS := -g -O2 -Wall
LDFLAGS := -lbpf -lz
RUNNER := ./build/runner
HEADERS := $(wildcard *.h)
BPF := ./build/bpf.o

.PHONY: clean

default: $(RUNNER) $(BPF)

$(RUNNER): runner.c $(HEADERS)
	mkdir -p ./build/
	$(CC) $(CFLAGS) -o $@ runner.c $(LDFLAGS)

$(BPF): bpf.c
	$(CLANG) -S \
		-target bpf \
		-D __BPF_TRACING__ \
		-Wall \
		-Wno-unused-value \
		-Wno-pointer-sign \
		-Wno-compare-distinct-pointer-types \
		-O2 -emit-llvm -c -g -o ./build/bpf.ll bpf.c
	$(LLC) -mcpu=probe -march=bpf -filetype=obj -o $@ ./build/bpf.ll

clean:
	rm  build/runner
	rm  build/bpf.*
