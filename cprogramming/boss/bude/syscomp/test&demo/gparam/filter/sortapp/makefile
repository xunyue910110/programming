include ../make/mk_def

MYLIBS = -lapp -lsort -lmmv  -lparam -ldbparam -lfilterdbparam -lbase  -lTypes  \
         -lgparam  -loradbi $(SYSLIBS) $(DB_LIB) -lpthread #-lutil 

MY_INCLUDE_PATH =-I../sort -I../dbparam  $(LD_INCLUDE_PATH) -I$(BOSSBILLING_HOME)/include/gparam $(DB_INCLUDE) 
MY_LIBRARY_PATH = $(LD_LIBRARY_PATH) $(DB_LIBPATH) 

SRCS	      = MyApplication.cpp
OBJS	      = ${SRCS:.cpp=.o}

PROGRAM       = filter

all:	sort_issue $(OBJS) $(PROGRAM) 

build:	clean sort_issue $(OBJS) $(PROGRAM)  
	
strip:
	strip 	$(PROGRAM)
	
sort_issue:
	@cd ../sort;make -f issue

debug:	sort $(OBJS) $(PROGRAM)	

submit:
	@cp $(PROGRAM)	$(BOSSBILLING_HOME)/bin

sort:
	@cd ../sort;make	

$(OBJS):
	$(ACC)  $(DUBUG) $(PARAMETER) $(DB_PARAMETER) $(MY_INCLUDE_PATH) $(@:.o=.cpp)
	echo "no $(ISSUE) "
$(PROGRAM):     $(OBJS)
				@echo " "
				@echo "Linking $(PROGRAM) ..."
				$(ACC) $(PARAMETER) $(OBJS) $(MY_LIBRARY_PATH) $(MYLIBS) -o $(PROGRAM)
				#added for cygwin
				@mv $(PROGRAM).exe $(PROGRAM)
				#@strip $(PROGRAM)
				@echo "done"

clean:
	@rm -rf *.o *.a Sun* ir* cxx_repository*
	@cd ../sort;rm -rf *.o *.a Sun* ir* cxx_repository*
	


