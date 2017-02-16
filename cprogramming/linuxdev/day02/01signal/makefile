

.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=dm01_signal dm02_signale dm03_signal sleep dm04_alarm
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)




