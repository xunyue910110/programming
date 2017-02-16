all:build

build:
	cd src; make -f makefile$(APP_BIT)

submit:
	cd src; make -f makefile$(APP_BIT) submit; 
	tar cvf dta_release.tar dta_release
	cp -f dta_release/lib/libdta.a ${BUDE_HOME}/syscomp/lib/
	if [ ! -d $(BUDE_HOME)/syscomp/include/dta ]; \
	then mkdir $(BUDE_HOME)/syscomp/include/dta; fi
	cp -f dta_release/include/dta.h $(BUDE_HOME)/syscomp/include/dta/dta.h


clean:
	cd src; make clean
	if [ -d samples ]; \
	then cd samples; make clean; fi
	rm -rf dta_release 
	rm -f dta_release.tar		
