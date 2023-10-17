PROGNAME=twtwt
COMMITS_COUNT=$(shell git rev-list --count HEAD)
VERSION?=0.0.$(COMMITS_COUNT)

PKG_CONFIG?=pkg-config
LIBS:=libcurl inih

CFLAGS_CUSTOM=-Wall -Wextra -Werror -Wno-unused-parameter
CFLAGS+=$(CFLAGS_CUSTOM)

LDFLAGS+=$(shell $(PKG_CONFIG) --libs $(LIBS))
INCLUDE+=$(shell $(PKG_CONFIG) --cflags $(LIBS)) -Iinclude
OUTDIR:=.build

PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin
MANDIR?=$(PREFIX)/share/man

SOURCES:=$(sort $(wildcard src/*.c))
HEADERS:=$(sort $(wildcard include/*.h))
OBJECTS:=$(SOURCES:src/%.c=$(OUTDIR)/%.o)

TEST_LDFLAGS:=$(LDFLAGS) $(shell $(PKG_CONFIG) --libs check)
TEST_INCLUDE:=$(INCLUDE) $(shell $(PKG_CONFIG) --cflags check)

TEST_SOURCES:=$(sort $(wildcard tests/check_*.c))
TEST_OBJECTS:=$(TEST_SOURCES:tests/%.c=$(OUTDIR)/%.o)

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG=1 -g -O0
endif

all: $(PROGNAME) compile_flags.txt

compile_flags.txt: Makefile  # used for clangd's LSP server
	true > $@
	echo $(CFLAGS_CUSTOM) >> $@
	echo $(INCLUDE) >> $@

$(OUTDIR):
	mkdir -p "$@"

$(OBJECTS) $(TEST_OBJECTS) $(OUTDIR)/check.o: | $(OUTDIR)

$(OUTDIR)/%.o: src/%.c $(HEADERS)
	$(CC) -std=c99 -c $(CFLAGS) -DVERSION='"$(VERSION)"' $(INCLUDE) $< -o $@

$(PROGNAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OUTDIR)/check_%.o: $(TEST_SOURCES) $(OUTDIR)/%.o $(HEADERS)
	$(CC) -std=c99 -c $(CFLAGS) $(TEST_INCLUDE) $< -o $@

$(OUTDIR)/check.o: tests/check.c
	$(CC) -std=c99 -c $(CFLAGS) $(TEST_INCLUDE) $< -o $@

$(OUTDIR)/check: $(OUTDIR)/check.o $(TEST_OBJECTS)
	$(CC) $(TEST_LDFLAGS) \
		$(OUTDIR)/check.o \
		$(patsubst $(OUTDIR)/check_%.o,$(OUTDIR)/%.o,$(TEST_OBJECTS)) \
		$(TEST_OBJECTS) -o $@

test: $(OUTDIR)/check
	$<

install: all
	mkdir -p $(DESTDIR)/$(BINDIR) $(DESTDIR)/$(MANDIR)
	install -m755 $(PROGNAME) $(DESTDIR)/$(BINDIR)/$(PROGNAME)

uninstall:
	rm -f $(DESTDIR)/$(BINDIR)/$(PROGNAME)

indent: $(HEADERS) $(SOURCES) $(TEST_SOURCES) tests/check.c
	indent $^ && rm **/*~

clean:
	rm -rf $(OUTDIR) $(PROGNAME) **/*~

.DEFAULT_GOAL=all
.PHONY: indent clean install uninstall test
