.SUFFIXES:.c .o

CC=gcc
SRCS=main.c myarray.c ../myheader.c

CFLAGS=-I ../
OBJS=$(SRCS:.c=.o)
EXEC=arraytest

start: $(OBJS)
		$(CC) -o $(EXEC) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -o  $@ -c $< -g
clean:
	rm -f $(OBJS)
