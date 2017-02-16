.SUFFIXES:.c .o

CC=gcc
SRCS=SelectionSort.c  InsertionSort.c main.c ../myheader.c

CFLAGS=-I ../
OBJS=$(SRCS:.c=.o)
EXEC=SelectionSort 

start: $(OBJS)
		$(CC) -o $(EXEC) $(OBJS)

SelectionSort: $(OBJS) 
	$(CC) -o  $@ $(OBJS)

InsertionSort: $(OBJS)
	$(CC) -o $@ $(OBJS)

.o:.c
	$(CC) $(CFLAGS) -o  $@ -c $<
clean:
	rm -f $(OBJS)
