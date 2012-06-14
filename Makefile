OBJ=$(shell ls -1 src/*.c | sed -r 's|^src/|target/|g;s|\.c|.o|g')

INCLUDE=include

all: target target/yacjp.so

target:
	test -d target || mkdir target

target/yacjp.so: $(OBJ)
	ld -shared -o $@ $(OBJ)

target/%.o: src/%.c $(INCLUDE)/*.h
	cc -fvisibility=hidden -I $(INCLUDE) -c $< -o $@

clean:
	rm -rf target

.PHONY: all clean
#.SILENT:
