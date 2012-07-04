OBJ=$(shell ls -1 src/*.c | sed -r 's|^src/|target/out/|g;s|\.c|.o|g')
TST=$(shell ls -1 test/test*.c | sed -r 's|^test/|target/test/|g;s|\.c|.run|g')

CFLAGS ?= -g
RUN ?=

all: lib run-test doc
	echo

install: lib doc
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	env | grep yacjp
	echo
	if [ -d $(DESTDIR) ]; then find $(DESTDIR) -name libyacjp -exec echo {} : \; -exec ls {} \; ; fi
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	mkdir -p $(DESTDIR)/usr/lib/libyacjp
	mkdir -p $(DESTDIR)/usr/include/libyacjp
	mkdir -p $(DESTDIR)/usr/share/doc/libyacjp
	cp target/libyacjp.so $(DESTDIR)/usr/lib/libyacjp/libyacjp.so.1
	ln -sf libyacjp.so.1 $(DESTDIR)/usr/lib/libyacjp/libyacjp.so
	cp target/libyacjp.a $(DESTDIR)/usr/lib/libyacjp/libyacjp.a.1
	ln -sf libyacjp.a.1 $(DESTDIR)/usr/lib/libyacjp/libyacjp.a
	cp include/*.h $(DESTDIR)/usr/include/libyacjp/
	cp -a target/*.pdf target/doc/html $(DESTDIR)/usr/share/doc/libyacjp/

release: lib doc
	echo Releasing version $(shell cat target/version)
	cd target && tar cfz yacjp_$(shell cat target/version)_$(shell gcc -v 2>&1 | grep '^Target:' | sed 's/^Target: //').tgz libyacjp.so libyacjp.pdf libyacjp-htmldoc.tgz

lib: target/libyacjp.so target/libyacjp.a

doc: target/libyacjp.pdf target/libyacjp-htmldoc.tgz
	echo

run-test: $(TST)
	echo

clean:
	echo "Cleaning"
	rm -rf target
	grep ^Package: debian/control | awk -F: '{print $$2}' | while read pkg; do rm -rf debian/$$pkg; done
	rm -rf debian/tmp
	echo "Done."

target/test/%.run: target/out/%.exe target/test
	echo "  Running test: $<"
	LD_LIBRARY_PATH=target $< 2>&1 >$(@:.run=.log) && touch $@ || ( LD_LIBRARY_PATH=target $(RUN) $<; exit 1 )

target:
	mkdir -p target/out/data

target/test: $(shell find test/data -type f)
	mkdir -p target/test
	cp -a test/data/* target/out/data/

target/libyacjp.so: target $(OBJ)
	echo "Linking shared library: $@"
	$(CC) -shared -o $@ -lm $(OBJ)
	echo

target/libyacjp.a: target $(OBJ)
	echo "Linking static library: $@"
	ar rcs $@ $(OBJ)
	echo

target/libyacjp.pdf: target/doc/latex/refman.pdf
	echo "    Saving PDF"
	cp $< $@

target/doc/latex/refman.pdf: target/doc/latex/Makefile target/doc/latex/version.tex
	echo "  Building PDF"
	make -C target/doc/latex > target/doc/make.log 2>&1

target/libyacjp-htmldoc.tgz: target/doc/html/index.html
	echo "  Building HTML archive"
	(cd target/doc/html; tar cfz - *) > $@

target/doc/latex/version.tex: target/version
	cp $< $@

target/version: Changelog
	head -n 1 $< | egrep -o '[0-9]+\.[0-9]+\.[0-9]+' > $@

target/doc/latex/Makefile: target/doc/.doc
	sleep 1; touch $@

target/doc/html/index.html: target/doc/.doc
	sleep 1; touch $@

target/doc/.doc: Doxyfile target $(shell ls -1 src/*.c include/*.h doc/*)
	echo "Generating documentation"
	doxygen $< && touch $@

target/out/%.o: src/%.c include/*.h
	echo "Compiling library object: $<"
	$(CC) $(CFLAGS) -fPIC -fvisibility=hidden -I include -c $< -o $@

target/out/%.exe: test/%.c test/*.h target/libyacjp.so
	echo "Compiling test: $<"
	$(CC) $(CFLAGS) -I include -L target -lyacjp $< -o $@

.PHONY: all lib doc clean run-test release
.SILENT:
