SOURCE=mfind.c list.c
OBJECTS=mfind.o list.o parser.o
FLAGS=-std=c11 -Wall -pedantic -Werror -pthread

all: $(OBJECTS)
	gcc $(OBJECTS) -pthread -o mfind

mfind.o: mfind.c
	gcc $(FLAGS) -c mfind.c

list.o: list.c list.h
	gcc $(FLAGS) -c list.c

parser.o: parser.c parser.h list.h
	gcc $(FLAGS) -c parser.c

debug: FLAGS+=-DDEBUG -g
debug: all

test: all
	./mfind -td -p2 . .. fail mfind

time: FLAGS+=-DTIME
time: all

memtest: all
	valgrind ./mfind -td -p2 . .. mfind

clean:
	rm -f mfind *.o
