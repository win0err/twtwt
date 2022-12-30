PROGNAME=twtwt
VERSION?=0.0.3

CFLAGS+=-Wall -Wextra -Werror -Wno-unused-parameter -DVERSION='"$(VERSION)"'
LDFLAGS+=-lcurl -linih # $(shell pkg-config --libs libcurl inih)
INCLUDE+=-Iinclude
OUTDIR=.build

PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin
MANDIR?=$(PREFIX)/share/man

SOURCES=$(sort $(wildcard src/*.c))
HEADERS=$(sort $(wildcard include/*.h))
OBJECTS=$(SOURCES:src/%.c=$(OUTDIR)/%.o)

ifeq ($(DEBUG),1)
	override CFLAGS+=-DDEBUG=1 -g -O0
endif

all: $(PROGNAME)

$(OUTDIR)/%.o: src/%.c $(HEADERS)
	@mkdir -p $(OUTDIR)
	$(CC) -std=c99 -c $(CFLAGS) $(INCLUDE) $< -o $@

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
