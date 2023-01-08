OUTPUTS = client server 
CC = gcc

all: $(OUTPUTS)

clean:
        rm -f *.o *~

.PHONY: all clean

server: server.o ant.o client_server_definitions.o settings.o server_definition.o
        $(CC) -o server server.o ant.o client_server_definitions.o settings.o server_definition.o -pthread

*.o: *.c *.h
        gcc $< -c

client: client.o client_definition.o client_server_definitions.o settings.o
        $(CC) -o client client.o client_definition.o client_server_definitions.o settings.o -pthread

.o: *.c
        gcc $< -c

client.o: client_definition.h client_server_definitions.h structuresEnums.h settings.h