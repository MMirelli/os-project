targets: msgStart schedulerProcessi msgEnd

include ../sMakefile

schedulerProcessi: $(BIN)schedulerProcessi.o $(BIN)list.o
	cc $(BIN)schedulerProcessi.o $(BIN)list.o -o $(BIN)schedulerProcessi

$(BIN)schedulerProcessi.o: $(SRC)schedulerProcessi.c $(INC)schedulerProcessi.h $(SINC)list.h
	cc -c $(SRC)schedulerProcessi.c -o $(BIN)schedulerProcessi.o

$(BIN)list.o: $(SSRC)list.c $(SINC)list.h
	cc -c $(SSRC)list.c -o $(BIN)list.o
