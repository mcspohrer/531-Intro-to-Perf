all: t t2

t: dotProduct_serial.c
	gcc -o seq dotProduct_serial.c -pg -lpthread

t2: 
	gcc -o par dotpar.c -pg -lpthread
