.SUFFIXES:.c .o

CC=gcc
SRCS=main.c myslist.c ../myheader.c

CFLAGS=-I ../
OBJS=$(SRCS:.c=.o)
EXEC=slisttest

start: $(OBJS)
		$(CC) -o $(EXEC) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -o  $@ -c $< -g
clean:
	rm -f $(OBJS)
