CC=cc
CFLAGS=-std=c99 -Wall
SHELLLIBS=-lm -ledit

all: shell

run: shell
	./shell

SHELLSOURCES=shell.c lang.c lispy.c mpc/mpc.c
shell: $(SHELLSOURCES)
	$(CC) $(CFLAGS) $(SHELLSOURCES) $(SHELLLIBS) -o $@

ASTDBGSOURCES=astdbg.c lang.c mpc/mpc.c
astdbg: $(ASTDBGSOURCES)
	$(CC) $(CFLAGS) $(ASTDBGSOURCES) $(SHELLLIBS) -o $@

clean:
	rm shell
