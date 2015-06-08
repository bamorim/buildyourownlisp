CC=cc
CFLAGS=-std=c99 -Wall
SHELLLIBS=-lm -ledit

all: shell

run: shell
	./shell

shell: shell.c mpc/mpc.c
	${CC} ${CFLAGS} shell.c mpc/mpc.c ${SHELLLIBS} -o shell

clean:
	rm shell
