test: test.c vandmlib.o
	gcc -o test test.c vandmlib.o -lm

vandmlib.o: vandmlib.h vandmlib.c
	gcc -c vandmlib.c -o vandmlib.o -lm