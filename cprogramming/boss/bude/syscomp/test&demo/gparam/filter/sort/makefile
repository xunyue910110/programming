include ../make/mk_def

MY_INCLUDE_PATH =-I../sort -I../dbparam   -I$(BOSSBILLING_HOME)/include/gparam  $(LD_INCLUDE_PATH) $(DB_INCLUDE)

SRCS=   FilterCdr.cpp 	FilterRule_1.cpp FilterRule_2.cpp 	 \
		ReadStandardCdr.cpp SortApplication.cpp SortCdr.cpp  \
		SortLog.cpp SortRule_1.cpp SortRule_2.cpp SortRule_3.cpp SortRule_4.cpp \
		SortRule_5.cpp  SortRule_6.cpp SortRule_Gprs.cpp WriteStandardCdr.cpp\
		

OBJS    = ${SRCS:.cpp=.o}

all:	$(OBJS) link

$(OBJS):
	$(ACC)  $(DUBUG) $(PARAMETER) $(DB_PARAMETER) $(MY_INCLUDE_PATH)  $(@:.o=.cpp)
  #$(ISSUE)
	@echo " "

link:
	@echo "Creating libsort.a ...";$(LD) libsort.a $(OBJS) $(SYS_O)
	cp libsort.a ../lib

clean :
	@rm -rf *.o *.a Sun* ir* cxx_repository*




#######################################################################################
FilterCdr.o:        FilterCdr.h
FilterRule_1.o:     FilterRule_1.h FilterCdr.h
FilterRule_2.o:     FilterRule_2.h FilterCdr.h
ReadStandardCdr.o:  ReadStandardCdr.h
SortApplication.o:  SortApplication.h SortCdr.h InputField.h FilterCdr.h \
	                ReadStandardCdr.h WriteStandardCdr.h FilterFp.h FilterRule_1.h \
	                FilterRule_2.h SortRule_1.h SortRule_2.h SortRule_3.h SortRule_4.h\
	                SortRule_5.h SortRule_6.h SortRule_Gprs.h
SortCdr.o:          SortCdr.h InputField.h
SortLog.o:          SortLog.h
SortRule_1.o:       SortRule_1.h SortCdr.h InputField.h
SortRule_2.o:       SortRule_2.h SortCdr.h InputField.h
SortRule_3.o:       SortRule_3.h SortCdr.h InputField.h
SortRule_4.o:       SortRule_4.h SortCdr.h InputField.h
SortRule_5.o:       SortRule_5.h SortCdr.h InputField.h
SortRule_6.o:       SortRule_6.h SortCdr.h InputField.h
SortRule_Gprs.o:    SortRule_Gprs.h SortCdr.h InputField.h
WriteStandardCdr.o: WriteStandardCdr.h FilterFp.h
