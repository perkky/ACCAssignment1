C = gcc
CFLAGS = -g -Wall -std=gnu99 -pthread
TARGET = server
OBJ = server.o FileIO.o ACCSockets.o serverApi.o history.o IpList.o

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean
clean:
	rm $(OBJ) $(TARGET)
