# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -fsanitize=address 
TARGET = puzzle
OBJ = main.o pqueue.o hashtable.o node.o

# Executed on "make"
all: $(TARGET)

# Link the objects files to create executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compiles main.c into object file, it depends on main.c and pqueue.h
main.o: main.c pqueue.h board.h hashtable.h node.h
	$(CC) $(CFLAGS) -c main.c

# Complies pqueue.c into object file, it depends on pqueue.c and pqueue.h
pqueue.o: pqueue.c pqueue.h
	$(CC) $(CFLAGS) -c pqueue.c

hashtable.o: hashtable.c hashtable.h board.h
	$(CC) $(CFLAGS) -c hashtable.c

node.o: node.c board.h
	$(CC) $(CFLAGS) -c node.c

clean:
	rm -f $(TARGET) $(OBJ)
