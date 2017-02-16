cd /app/budedev/syscomp/src/lcu
make 
make submit
cd /app/budedev/syscomp/src/mwlcu
make 
make submit
rm tsserver
make -f Makefile.dev install
cp tsserver /app/budedev/syscomp/bin