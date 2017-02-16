################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  MyApplication.cpp                  SendApp.cpp                    SendParameterFromFile.cpp \
SendLog.cpp                         RawCdrProcessor.cpp            RawFileProcessor.cpp \
GeneralFile.cpp                     OutputFile.cpp                 CdrFlag.cpp \
StandardFile.cpp                    StandardField.cpp              FieldInfo.cpp \
RecordFormat.cpp                    DataFormat.cpp                 DateTime.cpp \
TokenCdrExtractor.cpp               TokenFieldExtractRule.cpp      TokenRawCdrProcessor.cpp \
TokenFormat.cpp                     FormatConversion.cpp           PublicFunction.cpp \
FieldConvertors.cpp                 FieldValidators.cpp            CdrExtractor.cpp \
FieldExtractRule.cpp                FieldValidateRule.cpp          FieldConvertRule.cpp  \
FieldConvertRule_1.cpp              FieldConvertRule_2.cpp         FieldConvertRule_3.cpp \
FieldConvertRule_4.cpp              FieldConvertRule_5.cpp         FieldConvertRule_6.cpp \
FieldConvertRule_7.cpp              FieldConvertRule_8.cpp         FieldConvertRule_9.cpp \
FieldConvertRule_10.cpp             FieldConvertRule_11.cpp \
FieldConvertRule_13.cpp             FieldConvertRule_14.cpp        FieldConvertRule_15.cpp \
FieldConvertRule_16.cpp             FieldConvertRule_17.cpp \
FieldValidateRule_1.cpp             FieldValidateRule_2.cpp        FieldValidateRule_3.cpp \
FieldValidateRule_4.cpp             FieldValidateRule_5.cpp \
FieldValidateRule_7.cpp             FieldValidateRule_8.cpp        FieldValidateRule_9.cpp \
CompositeFieldExtractRule.cpp       Block.cpp											 FieldConvertRule_30.cpp


SUBMIT_HDRS      = 

#生成目标
PROGRAM          = send
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

THE_LIB          = -lapp -lbase -lnsl -llog4cpp -lbsd

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) #-DFP_DEBUG #-D_DEBUG_
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
