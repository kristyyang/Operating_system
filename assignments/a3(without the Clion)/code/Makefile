CFLAGS = -g -Wall -Wno-unused-function

all: timelife cache-ref cache-test

cache-ref: cache-ref.o cache-test.o
cache-test: cache-test.o cache.o
cache-test.o: cache-test.c cache.h
cache.o: cache.c cache.h


timelife.o: timelife.c life.h
life.o: life.c life.h
timelife: timelife.o life.o

life-%.o: life.c life.h
	gcc $(CFLAGS) -DLIFE_VERSION=$* -c life.c -o life-$*.o
timelife-%: timelife.o life-%.o
	gcc $(CFLAGS) $(LDFLAGS) -o timelife-$* timelife.o life-$*.o

clean:
	-rm -f cache-test cache-test.o cache.o cache-ref
	-rm -f timelife timelife.o life.o timelife-* life-*.o
