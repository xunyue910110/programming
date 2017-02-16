.SUFFIXES:.c .o

CC=gcc
SRCS=main.c  mystack.c  ../myheader.c ../array/myarray.c

CFLAGS=-I ../
OBJS=$(SRCS:.c=.o)
EXEC=stacktest

start: $(OBJS)
		$(CC) -o $(EXEC) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -o  $@ -c $< -g
clean:
	rm -f $(OBJS)
