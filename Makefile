CC=gcc
CFLAGS=-I./SyncGraph -I./ThreadPool -Wall -pthread
DEPS = SyncGraph/graph.h ThreadPool/threadpool.h
OBJ = main.o SyncGraph/graph.o ThreadPool/threadpool.o BFS.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

part_III.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) part_III.out
