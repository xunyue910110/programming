################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  GeneralFile.cpp      MyApplication.cpp              RecordFormat.cpp \
					RetLogRecord.cpp               PpRetLogCdr.cpp                FormatConversion.cpp \
					FieldConvertors.cpp            OutputFile.cpp                 StandardField.cpp \
					FieldExtractRule.cpp           RetLog.cpp                     CdrExtractor.cpp \
					Baf.cpp                        FieldInfo.cpp                  RetParameterFromFile.cpp \
					Block.cpp                      CompositeFieldExtractRule.cpp  RawCdrProcessor.cpp \
					FieldValidateRule.cpp          FieldValidateRule_1.cpp        FieldValidateRule_2.cpp \
					FieldValidateRule_3.cpp        FieldValidateRule_7.cpp        FieldValidateRule_8.cpp \
					FieldConvertRule.cpp           FieldConvertRule_1.cpp         FieldConvertRule_2.cpp \
					FieldConvertRule_6.cpp         FieldConvertRule_13.cpp \
					CdrFlag.cpp                    RetApp.cpp                     PublicFunction.cpp \
				  DataFormat.cpp                 RawFileProcessor.cpp           DateTime.cpp \
					FieldValidators.cpp            StandardFile.cpp               TokenFormat.cpp \
					TokenCdrExtractor.cpp          TokenFieldExtractRule.cpp      TokenRawCdrProcessor.cpp \
					FixedCdrExtractor.cpp          FixedFieldExtractRule.cpp      FixedRawCdrProcessor.cpp \

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = ret
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib

THE_LIB          = -lapp -lbase -lnsl -llog4cpp

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
