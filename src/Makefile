#Makefile
UNAME := $(shell uname -s)

#OCFLAGS=-O3 -Ilibtai-0.60 -DUSE_EMITTER -DUSE_MQTT=1
OCFLAGS=-O3 -Ilibtai-0.60 -DUSE_EMITTER -DUSE_REDIS=1
#OCFLAGS=-O3 -Ilibtai-0.60 -DUSE_EMITTER -DUSE_ZMQ=1
#DCFLAGS=-Wall -Wextra -Werror -g -DDEBUG
DCFLAGS=-Wall -g -DDEBUG -Ilibtai-0.60
PCFLAGS=-g -pg
LDFLAGS=-lpcap -lldns -lhiredis 
#LDFLAGS=-lpcap -lldns -lmosquitto
#LDFLAGS=-lpcap -lldns -lzmq

OBJECTS=$(patsubst %.c,%.o,$(wildcard *.c))
DOBJECTS=$(patsubst %.o,%_dbg.o,$(OBJECTS))
POBJECTS=$(patsubst %.o,%_pfl.o,$(OBJECTS))
BINARY=stash53
LIBTAI=libtai-0.60/libtai.a

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

$(LIBTAI): libtai-0.60
	(cd libtai-0.60 && make libtai.a)

stash53: $(OBJECTS) $(LIBTAI)
	gcc -o stash53 $(OBJECTS) $(LDFLAGS) $(LIBTAI)

static: $(OBJECTS)
	gcc -static -o stash53 dns.o stash53.o $(LDFLAGS) -lc -lnsl -lcrypto -lz

debug: $(DOBJECTS)
	gcc -o stash53 $(DOBJECTS) $(LDFLAGS)

profile: $(POBJECTS)
	gcc -o stash53 $(POBJECTS) $(LDFLAGS)

clean:
	-rm -v stash53 *.o 2>/dev/null || true

