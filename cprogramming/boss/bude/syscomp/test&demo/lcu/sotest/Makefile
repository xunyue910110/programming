
# This makefile creates a shared library.  It needs the open server header
# files usually installed in $mddev directory.
# This make file can be used for generating the template ESPs.
# It references the following macros:
#

# Include path where ospublic.h etc reside. You may have them in the
# standard places like /usr/lib etc.

# Place where the shared library will be generated.
# Directory where the source code is kept.
CC		   = CC
BSVC		 = buildserver
CC_HOME	  = /opt/SUNWspro/prod
TUXDIR	   = /data1/bea/tuxedo8.1
MW_HOME	  = /app/budedev/syscomp
LANG_HOME = /app/budedev/lang
INCLUDEPATH	 = -I. \
				-I$(ORACLE_HOME)/rdbms/demo \
				-I$(ORACLE_HOME)/rdbms/public \
				-I$(CC_HOME)/include \
				-I$(TUXDIR)/include \
				-I$(MW_HOME)/include/mwci \
				-I$(LANG_HOME)/include \
				-I$(LANG_HOME)/include/base \
				-I$(MW_HOME)/include/udbi \
				-I$(MW_HOME)/include \
				-I$(MW_HOME)/include/dynlib

LIBPATH		 = -L. -L../lib/ \
				-L/usr/lib \
				-L/usr/local/lib \
				-L$(TUXDIR)/lib \
				-L$(ORACLE_HOME)/lib \
				-L$(ORACLE_HOME)/rdbms/lib \
				-L$(MW_HOME)/lib \
				-L$(ORACLE_HOME)/lib

ORALIB		  = -locci -lclntsh -ludbi -llog4cpp


TUXLIB		  = -lmwci -ltux -lbuft   -lfml -lfml32 -lengine  

LIBS		= $(LIBPATH) $(ORALIB) $(TUXLIB) -ldl 

DEBUGFLAG   = -DDEBUG -g

CFLAGS  = $(INCLUDEPATH) $(DEBUGFLAG) -D__SUNOS__ $(LIBS)

SOURCES =  mwsvcsotest.cpp   sotest.cpp

OBJS  = $(SOURCES:.cpp=.o)

.SUFFIXES: $(SUFFIXES) .cpp

.cpp.o:
	$(CC) -o $@ -c $< $(CFLAGS)

all:$(OBJS) mwsvcsotest

mwsvcsotest : $(OBJS)
	$(BSVC) \
	-r Oracle_XA \
	-s MWSVCSOTEST \
	-o mwsvcsotest \
	-f sotest.o \
	-f mwsvcsotest.o \
	-l"-lmwci -ldynlib -ludbi"

install: all
	#tmshutdown -s mwsvcsotest
	cp mwsvcsotest /app/devtux/bin
	#tmboot -s mwsvcsotest

clean:
	rm -f *.o mwsvcsotest

