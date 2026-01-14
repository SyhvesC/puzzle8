CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = puzzle8.out
OBJ = main.c

$(TARGET): $(OBJ)
				$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

#main.o: main.c
#				$(CC) $(CFLAGS) -c main.c

clean:
				rm -f $(TARGET)
