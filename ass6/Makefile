all: t t2

t: slow.c
	gcc -o s slow.c -pg -lpthread

t2: fast.c
	gcc -o f fast.c -pg -lpthread -O3
