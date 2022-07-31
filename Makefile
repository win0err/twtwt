OPTI_FLAG=-O2
CFLAGS+=-Wall -Wextra -Werror -Wno-unused-parameter $(OPTI_FLAG)

LDFLAGS+=-lcurl -linih
INCLUDE+=-Iinclude
OUTDIR=.build

PROGNAME=twtwt

.DEFAULT_GOAL:=$(PROGNAME)

SOURCES=$(shell find src -type f -name '*.c')
HEADERS=$(shell find include -type f -name '*.h')
OBJECTS=$(SOURCES:src/%.c=$(OUTDIR)/%.o)

DEBUGFLAGS=-DDEBUG=1 -g
ifeq ($(shell uname), Linux)
DEBUGFLAGS+=-fsanitize=leak 
endif

$(OUTDIR)/%.o: src/%.c $(HEADERS)
	@mkdir -p $(OUTDIR)
	$(CC) -std=c99 -pedantic -c -o $@ $(CFLAGS) $(INCLUDE) $<

$(PROGNAME): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS+=$(DEBUGFLAGS)
debug: OPTI_FLAG=-O0
debug: $(PROGNAME)

indent: $(HEADERS) $(SOURCES)
	indent $^ && rm **/*~

clean:
	rm -rf $(OUTDIR) $(PROGNAME) **/*~

.PHONY: indent clean debug
