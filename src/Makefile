#Makefile
UNAME := $(shell uname -s)

#OCFLAGS=-O3 -DUSE_EMITTER -DUSE_MQTT=1
OCFLAGS=-O3 -DUSE_EMITTER -DUSE_REDIS=1
#OCFLAGS=-O3 -DUSE_EMITTER -DUSE_ZMQ=1
#DCFLAGS=-Wall -Wextra -Werror -g -DDEBUG
DCFLAGS=-Wall -g -DDEBUG
PCFLAGS=-g -pg
LDFLAGS=-lpcap -lldns -lhiredis 
#LDFLAGS=-lpcap -lldns -lmosquitto
#LDFLAGS=-lpcap -lldns -lzmq

OBJECTS=$(patsubst %.c,%.o,$(wildcard *.c))
DOBJECTS=$(patsubst %.o,%_dbg.o,$(OBJECTS))
POBJECTS=$(patsubst %.o,%_pfl.o,$(OBJECTS))
BINARY=stash53

ifeq ($(UNAME), FreeBSD)
    OCFLAGS=-I/usr/local/include -O3
    LDFLAGS=-L/usr/local/lib -lpcap -lldns
endif

all: $(BINARY)

%_dbg.o: %.c
	gcc $(DCFLAGS) -c $< -o $@

%_pfl.o: %.c
	gcc $(PCFLAGS) -c $< -o $@

%.o: %.c Makefile
	gcc $(OCFLAGS) -c $< -o $@

stash53: $(OBJECTS)
	gcc -o stash53 $(OBJECTS) $(LDFLAGS)

static: $(OBJECTS)
	gcc -static -o stash53 dns.o stash53.o $(LDFLAGS) -lc -lnsl -lcrypto -lz

debug: $(DOBJECTS)
	gcc -o stash53 $(DOBJECTS) $(LDFLAGS)

profile: $(POBJECTS)
	gcc -o stash53 $(POBJECTS) $(LDFLAGS)

clean:
	-rm -v stash53 *.o 2>/dev/null || true

