ARCH=$(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/ppc/powerpc/)
VERSION=0.0.1

SRC=$(shell ls *.cc)
OBJ=$(subst .cc,.o,$(SRC))

CPPFLAGS=-Wall -g3 -I. $(shell lua-config --include) -Icontrib/
CPPLDFLAGS=$(shell lua-config --libs) -rdynamic -ldl

PREFIX=$(DESTDIR)/usr/
ifeq ($(PREFIX),"")
PREFIX=/usr/local/
endif

all: contrib/bin2c ebiff plugins doc

contrib/bin2c: contrib/bin2c.c
	cd contrib;gcc -Wall -O3 -o bin2c bin2c.c \
		$(shell lua-config --include --libs) -ldl
	
plugins:
	rm -f *.so
	make -C plugins
	find plugins -name \*.so -exec ln -s \{\} . 2>/dev/null \;

ebiff: $(OBJ) lposix.o
	g++ -o ebiff $(OBJ) lposix.o $(CPPLDFLAGS)

lposix.o: contrib/lposix.h
	gcc -c contrib/lposix.c $(CPPFLAGS)

clean:
	rm -f *.o ebiff .*.d *.c *.luac core* *.so doc/*.xml doc/*.html 
	rm -f contrib/bin2c lposix.o
	make -C plugins clean

distclean: clean
	rm -rf dist-* tmp
	dh_clean

tgz: distclean
	TMP=`mktemp`;\
	PWD=`pwd`;\
	OLD=`basename $$PWD`;\
	cd ..;\
	find $$OLD -name CVS > $$TMP;\
	tar -czf ebiff-$(VERSION).tgz -X $$TMP $$OLD;\
	rm $$TMP

deb: distclean
	dpkg-buildpackage -rfakeroot -us -uc -d
	
release: 
	@echo 
	@echo "----------------------> tgz <--------------------------"
	@echo 
	make tgz
	rm -fr dist-tgz
	mkdir -p dist-tgz
	mv ../ebiff-$(VERSION).tgz dist-tgz
	@echo 
	@echo "----------------------> deb <--------------------------"
	@echo
	rm -fr tmp
	mkdir tmp
	tar -xzf dist-tgz/ebiff-*.tgz -C tmp
	make -C tmp/ebiff*/ deb
	rm -fr dist-deb
	mkdir -p dist-deb/ebiff/
	mv tmp/ebiff_$(VERSION).tar.gz dist-deb/ebiff
	mv tmp/ebiff_$(VERSION).dsc dist-deb/ebiff
	mv tmp/ebiff_$(VERSION)_$(ARCH).changes dist-deb/ebiff
	mv tmp/ebiff_$(VERSION)_$(ARCH).deb dist-deb/ebiff
	mv tmp/ebiff-notifier*_$(ARCH).deb dist-deb/ebiff
	mv tmp/ebiff-mailbox*_$(ARCH).deb dist-deb/ebiff
	touch dist-deb/ebiff/overridefile
	cd dist-deb/ebiff/;dpkg-scanpackages . overridefile > Packages
	cd dist-deb/ebiff/;dpkg-scansources . overridefile > Sources
	cd dist-deb/ebiff/;gzip -f Packages Sources
	lintian -i dist-deb/ebiff/*.deb dist-deb/ebiff/*.dsc || true
	rm -fr tmp
	@echo 
	@echo "----------------------> END <--------------------------"
	@echo


install: all
	mkdir -p $(PREFIX)/bin/
	mkdir -p $(PREFIX)/share/ebiff/
	cp ebiff $(PREFIX)/bin/
	find plugins -name \*.so -exec cp \{\} \
		$(PREFIX)/share/ebiff/ 2>/dev/null \;

doc: doc/manual.html

doc/manual.html : grammar.lua doc/doc.lua tableex.lua doc/doc.xslt
	lua -l doc/doc.lua -l grammar.lua -l tableex.lua \
		-e "print(generate_doc(\"relation\",\"\"))" |\
		tee doc/doc.xml | xsltproc doc/doc.xslt - > doc/manual.html



%.o:%.cc
	g++ $(CPPFLAGS) -c $< 

.parser.d:parser.cc parser.c tableex.c grammar.c typecheck.c
	g++ -MM $(CPPFLAGS) $< > $@

.%.d:%.cc 
	g++ -MM $(CPPFLAGS) $< > $@

parser.c:parser.lua grammar.lua tableex.lua
	lua -l parser.lua -l grammar.lua -l tableex.lua \
		-e "print(generate_parser(\"relation\"))" > parser.c

%.c:%.lua contrib/bin2c
	luac -o $<c $<
	contrib/bin2c $<c > $@
	rm $<c

.PHONY: plugins doc

include $(SRC:%.cc=.%.d)


