

.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=dm01_sigstatus dm02_sigaction dm03_sigqueuc
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)




