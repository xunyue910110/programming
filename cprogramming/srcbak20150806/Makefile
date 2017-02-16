.SUFFIX: .c .o #associate .c and .o
CC=gcc #compiler
SRCS=setoutput.c add.c #source file list
OBJS=$(SRCS:.c=.o) #OBJS=setoutput.o add.o
EXEC=setoutput

start:$(OBJS)
#@echo ${HOME}
	@echo "Linking setoutput"
	$(CC) -o $(EXEC) $(OBJS) #compiling
	
.c.o: #
	$(CC) -Wall -g -o $@ -c $<
clean:
	rm -f *.o $(EXEC)
