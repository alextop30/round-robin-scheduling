CC = g++
CFLAGS = -Wall -g -std=c++11

all: rr

process.o : process.cpp process.h
	${CC} ${CFLAGS} -c process.cpp

main.o : rr_schedule.cpp process.h
	${CC} ${CFLAGS} -c rr_schedule.cpp

rr : main.o process.o
	${CC} ${CFLAGS} process.o rr_schedule.o -o roundrobin
	
run:
	make all
	echo && ./roundrobin

clean:
	rm -rf *.o
	rm roundrobin
