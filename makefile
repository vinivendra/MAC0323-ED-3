execName = ep3
execOptions =  -g -o $(execName) -lm -lc
objOptions = -g -pedantic -Wall -lm -lc -c
objects = main.o ARNE.o Item.o

.PHONY:default
default: $(objects)
	gcc $(objects) $(execOptions)

main.o: main.c
	gcc -g -pedantic -Wall -lm -lc -c main.c

ARNE.o: ARNE.c Item.c
	gcc -g -pedantic -Wall -lm -lc -c ARNE.c

Item.o: Item.c
	gcc -g -pedantic -Wall -lm -lc -c Item.c

clean:
	rm -f *.o $(execName)
