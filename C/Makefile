EXE:=tb8
DEBUG=-DDEBUG=0

.PHONY: clean run

all: clean $(EXE) run

$(EXE): main.c
	$(CC) -Wall -Werror $(DEBUG) -o $(EXE) main.c

run:
	stty raw; ./$(EXE) ; stty cooked

clean:
	@rm -rf $(EXE) *~ main.dSYM/
