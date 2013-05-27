execName = ep3
execOptions =  -g -o $(execName) -lm -lc
objOptions = -g -pedantic -Wall -lm -lc
objects = main.o

.PHONY:default
default: $(objects)
	gcc $(objects) $(execOptions)

main.o: main.c
	gcc -c main.c $(objOptions)

clean:
	rm -f *.o $(execName)
