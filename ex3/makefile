targets: msgStart server client msgEnd  # regola che garantisce la compilazione di ambo gli eseguibili di interesse

include ../sMakefile # accede alle variabili dichiarate nel makefile comune

server: $(BIN)server.o $(BIN)util.o $(BIN)list.o
	cc $(BIN)server.o $(BIN)util.o $(BIN)list.o -o $(BIN)server

client: $(BIN)client.o $(BIN)util.o
	cc $(BIN)client.o $(BIN)util.o -o $(BIN)client

$(BIN)client.o: $(SRC)client.c $(INC)client.h $(INC)util.h
	cc -c $(SRC)client.c -o $(BIN)client.o

$(BIN)server.o: $(SRC)server.c $(INC)server.h $(INC)util.h $(SINC)list.h
	cc -c $(SRC)server.c -o $(BIN)server.o

$(BIN)util.o: $(SRC)util.c $(INC)util.h
	cc -c $(SRC)util.c -o $(BIN)util.o

$(BIN)list.o: $(SSRC)list.c $(SINC)list.h
	cc -c $(SSRC)list.c -o $(BIN)list.o
