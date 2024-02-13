ifndef KASHK_DIR
$(error "KASHK_DIR env variable is not set")
endif
CLANG := clang
LLC := llc
CC := $(CLANG)
CFLAGS := -g -O2 -Wall
LDFLAGS := -lbpf -lz
RUNNER := ./build/runner
HEADERS := $(wildcard *.h)
COMPILE_SCRIPT = $(KASHK_DIR)/compile_scripts/compile_bpf_source.sh
BPF := ./build/bpf.o

.PHONY: clean

default: $(RUNNER) $(BPF)

$(RUNNER): runner.c $(HEADERS)
	mkdir -p ./build/
	$(CC) $(CFLAGS) -o $@ runner.c $(LDFLAGS)

$(BPF): bpf.c
	sudo bash $(COMPILE_SCRIPT) ./bpf.c $@ > /dev/null
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
