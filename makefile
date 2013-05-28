execName = ep3
execOptions =  -g -o $(execName) -lm -lc
objOptions = -g -pedantic -Wall -lm -lc -c
objects = main.o ARNE.c Item.c

.PHONY:default
default: $(objects)
	gcc $(objects) $(execOptions)

main.o: main.c
	gcc $(objOptions) main.c

ARNE.o: ARNE.c Item.c
	gcc $(objOptions) ARNE.c

Item.o: Item.c
	gcc $(objOptions) Item.c

clean:
	rm -f *.o $(execName)
