SRC=vice
BIN=bin
OBJ=obj
INC=vice

CC=gcc
CFLAGS=-Wall -ggdb -O0 -I$(INC)

$(BIN)/vice: $(OBJ)/attack.o \
$(OBJ)/bitboards.o \
$(OBJ)/board.o \
$(OBJ)/data.o \
$(OBJ)/evaluate.o \
$(OBJ)/hashkeys.o \
$(OBJ)/init.o \
$(OBJ)/io.o \
$(OBJ)/makemove.o \
$(OBJ)/misc.o \
$(OBJ)/movegen.o \
$(OBJ)/perft.o \
$(OBJ)/pvtable.o \
$(OBJ)/search.o \
$(OBJ)/uci.o \
$(OBJ)/validate.o \
$(OBJ)/xboard.o \
$(SRC)/vice.c
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ)/attack.o: $(SRC)/attack.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/bitboards.o: $(SRC)/bitboards.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/board.o: $(SRC)/board.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/data.o: $(SRC)/data.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/evaluate.o: $(SRC)/evaluate.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/hashkeys.o: $(SRC)/hashkeys.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/init.o: $(SRC)/init.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/io.o: $(SRC)/io.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/makemove.o: $(SRC)/makemove.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/misc.o: $(SRC)/misc.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/movegen.o: $(SRC)/movegen.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/perft.o: $(SRC)/perft.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/pvtable.o: $(SRC)/pvtable.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/search.o: $(SRC)/search.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/uci.o: $(SRC)/uci.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/validate.o: $(SRC)/validate.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ)/xboard.o: $(SRC)/xboard.c
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: clean

clean:
	rm -f $(OBJ)/*
	rm -f $(BIN)/*
