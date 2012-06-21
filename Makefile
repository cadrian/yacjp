OBJ=$(shell ls -1 src/*.c | sed -r 's|^src/|target/out/|g;s|\.c|.o|g')
TST=$(shell ls -1 test/test*.c | sed -r 's|^test/|target/test/|g;s|\.c|.run|g')

CFLAGS ?= -g
RUN ?=

all: target/libyacjp.so run-test
	echo "Done."

run-test: $(TST)

clean:
	echo "Cleaning"
	rm -rf target
	echo "Done."

target/test/%.run: target/out/%.exe
	echo "  Running test: $<"
	LD_LIBRARY_PATH=target $(RUN) $< && touch $@
	echo

target:
	mkdir -p target/out
	mkdir -p target/test

target/libyacjp.so: target $(OBJ)
	echo "Linking library: $@"
	$(CC) -shared -o $@ -lm $(OBJ)
	echo

target/out/%.o: src/%.c include/*.h
	echo "Compiling library object: $<"
	$(CC) $(CFLAGS) -fPIC -fvisibility=hidden -I include -c $< -o $@

target/out/%.exe: test/%.c test/*.h target/libyacjp.so
	echo "Compiling test: $<"
	$(CC) $(CFLAGS) -I include -L target -lyacjp $< -o $@

.PHONY: all clean run-test
.SILENT:
