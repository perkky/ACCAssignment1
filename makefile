C = gcc
CFLAGS = -Wall -std=gnu99
TARGET = server
OBJ = server.o FileIO.o ACCSockets.o serverApi.o history.o

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean
clean:
	rm $(OBJ) $(TARGET)
