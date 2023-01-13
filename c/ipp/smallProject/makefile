CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2

all: labyrinth

labyrinth: main.o error.o conversions.o vector.o input.o string.o labyrinth.o queue.o bfs.o twobitset.o
	${CC} -o labyrinth $^

main.o: main.c libs.h labyrinth.h error.h conversions.h defines.h vector.h input.h
	${CC} ${CFLAGS} -c $<

input.o: input.c input.h libs.h error.h conversions.h defines.h string.h vector.h
	${CC} ${CFLAGS} -c $<

error.o: error.c error.h libs.h defines.h
	${CC} ${CFLAGS} -c $<

conversions.o: conversions.c conversions.h libs.h vector.h string.h
	${CC} ${CFLAGS} -c $<

vector.o: vector.c vector.h libs.h error.h
	${CC} ${CFLAGS} -c $<

twobitset.o: twobitset.c twobitset.h error.h libs.h
	${CC} ${CFLAGS} -c $<

labyrinth.o: labyrinth.c labyrinth.h libs.h input.h defines.h error.h conversions.h vector.h twobitset.h
	${CC} ${CFLAGS} -c $<

string.o: string.c string.h error.h libs.h
	${CC} ${CFLAGS} -c $<

queue.o: queue.c queue.h error.h libs.h
	${CC} ${CFLAGS} -c $<

bfs.o: bfs.c bfs.h queue.h libs.h twobitset.h defines.h labyrinth.h
	${CC} ${CFLAGS} -c $<

clean:
	rm *.o main