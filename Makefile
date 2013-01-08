OBJS = main.o ips.o
CFLAGS =  -Wall -pedantic -ansi -O2
BIN = ips

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN)

clean:
	rm -f $(OBJS) $(BIN)
