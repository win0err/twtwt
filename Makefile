PROGNAME=twtwt
COMMITS_COUNT=$(shell git rev-list --count HEAD)
VERSION?=0.0.$(COMMITS_COUNT)

PKG_CONFIG?=pkg-config
LIBS:=libcurl inih

CFLAGS+=-Wall -Wextra -Werror -Wno-unused-parameter
LDFLAGS+=$(shell $(PKG_CONFIG) --libs $(LIBS))
INCLUDE+=$(shell $(PKG_CONFIG) --cflags $(LIBS)) -Iinclude
OUTDIR:=.build

PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin
MANDIR?=$(PREFIX)/share/man

SOURCES:=$(sort $(wildcard src/*.c))
HEADERS:=$(sort $(wildcard include/*.h))
OBJECTS:=$(SOURCES:src/%.c=$(OUTDIR)/%.o)

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG=1 -g -O0
endif

all: $(PROGNAME) compile_flags.txt

compile_flags.txt: Makefile
	true > $@
	echo $(CFLAGS) >> $@
	echo $(INCLUDE) >> $@

$(OUTDIR):
	@mkdir -p "$@"

$(OUTDIR)/%.o: src/%.c $(HEADERS) $(OUTDIR)
	$(CC) -std=c99 -c $(CFLAGS) -DVERSION='"$(VERSION)"' $(INCLUDE) $< -o $@

$(PROGNAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

install: all
	mkdir -p $(DESTDIR)/$(BINDIR) $(DESTDIR)/$(MANDIR)
	install -m755 $(PROGNAME) $(DESTDIR)/$(BINDIR)/$(PROGNAME)

uninstall:
	rm -f $(DESTDIR)/$(BINDIR)/$(PROGNAME)

indent: $(HEADERS) $(SOURCES)
	indent $^ && rm **/*~

clean:
	rm -rf $(OUTDIR) $(PROGNAME) **/*~

.DEFAULT_GOAL=all
.PHONY: indent clean install uninstall
