OBJ=$(shell ls -1 src/*.c | sed -r 's|^src/|target/|g;s|\.c|.o|g')

INCLUDE=include

all: target target/libyacjp.so run-test

run-test: target/test
	LD_LIBRARY_PATH=target target/test

target:
	test -d target || mkdir target

target/libyacjp.so: $(OBJ)
	ld -shared -o $@ -lm $(OBJ)

target/%.o: src/%.c $(INCLUDE)/*.h
	$(CC) -g -fPIC -fvisibility=hidden -I $(INCLUDE) -c $< -o $@

target/test: test/test.c
	$(CC) -g -I $(INCLUDE) -L target -lyacjp $< -o $@

clean:
	rm -rf target

.PHONY: all clean run-test
#.SILENT:
