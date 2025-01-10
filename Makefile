CC=clang
CFLAGS=-I/usr/include/
ODIR=obj

SRC_FILES = $(wildcard src/*.c) $(wildcard src/**/*.c)
_OBJ = $(patsubst src/%.c,%.o,$(SRC_FILES))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: make_dirs wlipsync.wasm

$(ODIR)/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) --target=wasm32 -mbulk-memory -nostdlib -O3 $(CFLAGS) -o $@ -c $<

wlipsync.wasm: $(OBJ)
	wasm-ld --no-entry --export-dynamic --export-all --lto-O3 --import-memory $^ -o www/$@

.PHONY: clean

clean:
	rm -rf $(ODIR) wlipsync.wasm

make_dirs:
	mkdir -p $(ODIR)