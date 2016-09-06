CFLAGS = -g -Wall -O3
LIBS = -lsqlite3 -lpthread
P=main
T=test
OBJECTfortest = emlparser.o myscandir.o savetosql.o

OBJECTformain = emlparser.o myscandir.o savetosql.o threadpool.o

DEP = emlparser myscandir savetosql test

CC=cc

$(T):$(OBJECTfortest)
	gcc -o test $(OBJECTfortest) $(CFLAGS) $(LIBS) test.c
$(P):$(OBJECTformain)
	gcc -o main $(OBJECTformain) $(CFLAGS) $(LIBS) main.c

emlparser:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $*.c
myscandir:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $*.c
savetosql:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $*.c
threadpool:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $*.c


clean:
	rm -f $(OBJECTformain)
	rm main

T:
	rm -f $(OBJECTfortest)
	rm test.o test
