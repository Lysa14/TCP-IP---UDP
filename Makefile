CC = gcc
CFLAGS = -g -Wall
BIN_CL = client.out
BIN_SV = server.out
SRCS_CL = src/client/client.c
SRCS_SV = src/server/server.c

all: client server

client: $(SRCS_CL)
	$(CC) $(CFLAGS) -o $(BIN_CL) $(SRCS_CL)

server: $(SRCS_SV)
	$(CC) $(CFLAGS) -o $(BIN_SV) $(SRCS_SV)

st:
	./server.out TCP

ct:
	./client.out TCP

su:
	./server.out UDP

cu:	
	./client.out UDP


clean:
	rm -f main
	rm -f vgcore.*
	rm -f *.toc
	rm -f *.gz
	rm -f *.out
	rm -f *.aux