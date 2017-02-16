CC = `../getenv CC`
SOCKLIB=`../getenv SOCKLIB`
#CCFLAGS = -D_POSIX_C_SOURCE=199506L
INCLUDE = -I. -I../src
LIB = -L. -L../src
OTHERLIB = $(SOCKLIB)
DTALIB = -lft -lmt -ldta

all: build

build: 	readq writeq putf getf mgetf \
	readq2 writeq2 putf2 getf2 \
#	sendq lo ggg ppp

readq: readq.c
	$(CC) $(INCLUDE) $(LIB) -o readq readq.c $(DTALIB) $(OTHERLIB)
writeq: writeq.c
	$(CC) $(INCLUDE) $(LIB) -o writeq writeq.c $(DTALIB) $(OTHERLIB)
putf: putf.c
	$(CC) $(INCLUDE) $(LIB) -o putf putf.c $(DTALIB) $(OTHERLIB)
getf: getf.c
	$(CC) $(INCLUDE) $(LIB) -o getf getf.c $(DTALIB) $(OTHERLIB)
mgetf: mgetf.c
	$(CC) $(INCLUDE) $(LIB) -o mgetf mgetf.c $(DTALIB) $(OTHERLIB)
readq2: readq.cpp
	$(CC) $(INCLUDE) $(LIB) -o readq2 readq.cpp $(DTALIB) $(OTHERLIB)
writeq2: writeq.cpp
	$(CC) $(INCLUDE) $(LIB) -o writeq2 writeq.cpp $(DTALIB) $(OTHERLIB)
putf2: putf.cpp
	$(CC) $(INCLUDE) $(LIB) -o putf2 putf.cpp $(DTALIB) $(OTHERLIB)
getf2: getf.cpp
	$(CC) $(INCLUDE) $(LIB) -o getf2 getf.cpp $(DTALIB) $(OTHERLIB)
sendq: sendq.c
	$(CC) $(INCLUDE) $(LIB) -o sendq sendq.c -ldta 
lo: looksem.c
	$(CC) $(INCLUDE) $(LIB) -o lo looksem.cpp -ldta
ppp: ppp.c
	$(CC) $(INCLUDE) $(LIB) -o ppp ppp.c -ldta
ggg: ggg.c
	$(CC) $(INCLUDE) $(LIB) -o ggg ggg.c -ldta
clean:
	rm -f *.o readq writeq putf getf mgetf readq2 \
	writeq2 putf2 getf2 #sendq lo ppp ggg

submit:
	cp readq ../release/bin
	cp writeq ../release/bin
	cp putf ../release/bin
	cp getf ../release/bin
	cp mgetf ../release/bin



