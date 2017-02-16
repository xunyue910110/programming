################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  AsnCdrExtractor.cpp              \
                    AsnFieldExtractRule.cpp          \
                    AsnFormat.cpp                    \
                    AsnRawCdrProcessor.cpp           \
                    Baf.cpp                          \
                    Block.cpp                        \
                    CdrExtractor.cpp                 \
                    CdrFlag.cpp                      \
                    CompositeFieldExtractRule.cpp    \
                    DataFormat.cpp                   \
                    DateTime.cpp                     \
                    FieldConvertRule.cpp             \
                    FieldConvertRule_1.cpp           \
                    FieldConvertRule_10.cpp          \
                    FieldConvertRule_11.cpp          \
                    FieldConvertRule_12.cpp          \
                    FieldConvertRule_13.cpp          \
                    FieldConvertRule_14.cpp          \
                    FieldConvertRule_15.cpp          \
                    FieldConvertRule_16.cpp          \
                    FieldConvertRule_17.cpp          \
                    FieldConvertRule_18.cpp          \
                    FieldConvertRule_19.cpp          \
                    FieldConvertRule_2.cpp           \
                    FieldConvertRule_3.cpp           \
                    FieldConvertRule_4.cpp           \
                    FieldConvertRule_5.cpp           \
                    FieldConvertRule_6.cpp           \
                    FieldConvertRule_7.cpp           \
                    FieldConvertRule_8.cpp           \
                    FieldConvertRule_9.cpp           \
                    FieldConvertors.cpp              \
                    FieldExtractRule.cpp             \
                    FieldInfo.cpp                    \
                    FieldInfos.cpp                   \
                    FieldValidateRule.cpp            \
                    FieldValidateRule_1.cpp          \
                    FieldValidateRule_2.cpp          \
                    FieldValidateRule_3.cpp          \
                    FieldValidateRule_4.cpp          \
                    FieldValidateRule_5.cpp          \
                    FieldValidateRule_7.cpp          \
                    FieldValidateRule_8.cpp          \
                    FieldValidateRule_9.cpp          \
                    FieldValidateRule_10.cpp         \
                    FieldValidators.cpp              \
                    FixedCdrExtractor.cpp            \
                    FixedFieldExtractRule.cpp        \
                    FixedRawCdrProcessor.cpp         \
                    GAsnCdrExtractor.cpp             \
                    GasnFieldExtractRule.cpp         \
                    GeneralFile.cpp                  \
                    OutputFile.cpp                   \
                    MyApplication.cpp                \
                    PpLog.cpp                        \
                    PpParameterFromFile.cpp          \
                    Pp_Exception.cpp                 \
                    PreProApi.cpp                    \
                    PreProcApplication.cpp           \
                    RawCdrProcessor.cpp              \
                    RawFileProcessor.cpp             \
                    RecordFormat.cpp                 \
                    RepeatableFieldInfo.cpp          \
                    SasnCdrExtractor.cpp             \
                    SasnFieldExtractRule.cpp         \
                    SasnFormat.cpp                   \
                    SasnRawCdrProcessor.cpp          \
                    StandardField.cpp                \
                    StandardFile.cpp                 \
                    TokenCdrExtractor.cpp            \
                    TokenFieldExtractRule.cpp        \
                    TokenFormat.cpp                  \
                    TokenRawCdrProcessor.cpp         

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = pp
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

THE_LIB          = -lapp -lbase -lnsl -llog4cpp -lclntsh -lbsd

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) #-D_DEBUG_ ####-P   ###-qmaxmem=-1 -g -w
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) ###-qmaxmem=-1 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
