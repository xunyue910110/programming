rm -rf csyscomp/lib                
rm -rf syscomp/include            
rm -rf syscomp/bin                
rm -rf lang/lib                   
rm -rf lang/include               
rm -rf stdmake                    
rm -rf bizcomp/include            
rm -rf bizcomp/lib                
rm -rf frame/mwsvc/lib            
rm -rf frame/mwsvc/bin            
rm -rf frame/mwsvc/include

cp -R ../anta$APP_BIT\b/syscomp/lib		syscomp/lib               
cp -R ../anta$APP_BIT\b/syscomp/include		syscomp/include             
cp -R ../anta$APP_BIT\b/syscomp/bin		syscomp/bin                 
cp -R ../anta$APP_BIT\b/lang/lib		lang/lib                    
cp -R ../anta$APP_BIT\b/lang/include		lang/include                
cp -R ../anta$APP_BIT\b/stdmake			stdmake                     
cp -R ../anta$APP_BIT\b/bizcomp/include		bizcomp/include             
cp -R ../anta$APP_BIT\b/bizcomp/lib		bizcomp/lib                 
cp -R ../anta$APP_BIT\b/frame/mwsvc/lib		frame/mwsvc/lib             
cp -R ../anta$APP_BIT\b/frame/mwsvc/bin		frame/mwsvc/bin             
cp -R ../anta$APP_BIT\b/frame/mwsvc/include	frame/mwsvc/include         
cp    ../anta$APP_BIT\b/linkbude.sh		./
