PACKAGE := moblin-cursor-theme
VERSION := $(shell git describe --abbrev=0)

all:

dist:
	cd pngs && ./make.sh
	mkdir -p $(PACKAGE)-$(VERSION)
	mv xcursors $(PACKAGE)-$(VERSION)	
	cp COPYING AUTHORS $(PACKAGE)-$(VERSION)
	tar cjvf $(PACKAGE)-$(VERSION).tar.bz2 $(PACKAGE)-$(VERSION)
	rm -rf $(PACKAGE)-$(VERSION)

.PHONY: all dist

