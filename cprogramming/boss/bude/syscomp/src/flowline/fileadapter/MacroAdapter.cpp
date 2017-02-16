#include "MacroAdapter.h"
#include "flowline/adapter/Adapter.h"
#include "flowline/Context.h"
#include "fileadapter/Util.h"
#include "fileadapter/CDateTime.h"
#include "flowline/flbase.h"
#include "flowline/FlowLineException.h"
#include "file/File.h"
#include "base/Exception.h"
#include "flowline/Cycle.h"
#include <string>
#include <time.h>
#include <vector>

using namespace flowline;
namespace flowline
{

MacroAdapter::~MacroAdapter()
{
}

const string SEQUENCE = "seq";
const string DATETIME = "date";
const string CYCLEID  = "cycleId";

//根据宏名得到返回值
string MacroAdapter::getAttribute(const string& name) const {
       
        vector <string> args;
        parserMethod(name,args);       
        
        if(args.size()== 0){
            THROW(DefinitionException, "无效宏变量");
        }
        if(args[0]==SEQUENCE){      
        
            char buffer[32];
            char formatter[8] = "%0";
            
            if(args.size() > 1) {                                
                sprintf(formatter+2, "%dd", atoi(args[1].c_str()));
            } else {
                sprintf(formatter+2, "%dd", 1);
            } 
            if(args.size() > 2)
                sprintf(buffer, formatter, m_ctx.getFlowLineDao().nextId(targetname) -1 + atoi(args[2].c_str()) );
            else
                sprintf(buffer, formatter, m_ctx.getFlowLineDao().nextId(targetname)); 
                   
            return buffer;   
        }else if(args[0]==DATETIME  ){            
     
            if( args.size() > 1 ){
               return  CDateTime::now(args[1].c_str());   
               
            }else{
                return  CDateTime::now();    
               
            } 
        }
        else if(args[0]==CYCLEID){
        	return m_ctx.getAttribute(CYCLEID); 
        }
        string v= m_ctx.getAttribute(args[0]);
        if(!v.empty()) return v;
        if(args[0]==CYCLEID){                       //
            return m_ctx.getAttribute(CYCLEID);     //
        }                                           //
         else {                                     //   
        	const char* s = getenv(args[0].c_str());
            if(s){
              return s;
            }
        }
            THROW_P1(DefinitionException ,"无效宏变量%s", args[0].c_str());
       // } 
        
       
}

string MacroAdapter::getAttributeFilter(const string& name) const {
        vector <string> args;
        parserMethod(name,args);       
      cout<<"name is "<<name.c_str()<<endl;
        if(args.size()== 0){
            THROW(DefinitionException, "无效宏变量");
        }
        if(args[0]==SEQUENCE){      
            string buffer = "";
            if(args.size() == 1){
               return "*";
            }
            if(args.size() >= 2) {
                int i= atoi(args[1].c_str());
                while (i-- > 0){
                  buffer += "?";
                }
            }
            return buffer;
        }else if(args[0]==CYCLEID){                        
          if(args.size()<=1){
          	return m_ctx.getAttribute(CYCLEID);
          }
          if(args.size()>=2){
	          string temp = m_ctx.getAttribute(CYCLEID).c_str();
	         // cout<<"string temp vaule is :  "<<temp<<endl;
	          string aa;
	          aa = temp.substr(0,4);
	          int YY = atoi(aa.c_str());
	          temp.erase(0,4);
	          aa = temp.substr(0,2);
	          int MM = atoi(aa.c_str());
	          temp.erase(0,2);
	          aa = temp.substr(0,2);
	          int DD = atoi(aa.c_str());
                  int HH;
	          if(temp.size()>=4){
		  	temp.erase(0,2);
	          	aa = temp.substr(0,2);
	          	HH = atoi(aa.c_str());
		  }
	          temp.clear();  
	         
	         
	          int jj;
              temp = args[1].c_str();
              cout<<"temp ,args[1].c_str()"<<temp.c_str()<<endl;
              //----begin---YYYYMMDD---------------------------------
              if(temp.find("YYYYMMDD")>=0 && temp.find("YYYYMMDD")<=20){
          		string rr;
                        if(args.size()==2){
                                char fir1[11];
                                sprintf(fir1, "%04d%02d%02d" ,YY ,MM ,DD );
                                return fir1;
                        }
          		for(int i =2 ;i<args.size(); i++ ){
          			rr = args[i].c_str();
          			if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	}else{
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	} 
                    }	       			
          			
	          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
	                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
	                    		rr.erase(rr.find("]"));
	                    		rr.erase(rr.find("M"),rr.find("-")+1);
	                    		jj = atoi(rr.c_str());
	                    		MM = MM - jj;
	                    	}else{
	                    		rr.erase(rr.find("M"),rr.find("-")+1);
	                    		jj = atoi(rr.c_str());
	                    		MM = MM - jj;
	                    	} 
	                    }	       		
	                    else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
	                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
	                    		rr.erase(rr.find("]"));
	                    		rr.erase(rr.find("D"),rr.find("-")+1);
	                    		jj = atoi(rr.c_str());
	                    		DD = DD - jj;
	                    	}else{
	                    		rr.erase(rr.find("D"),rr.find("-")+1);
	                    		jj = atoi(rr.c_str());
	                    		DD = DD - jj;
	                    	} 
	                    }	       		
	       				
	       				
	  					else if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
	                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
	                    		rr.erase(rr.find("]"));
	                    		rr.erase(rr.find("Y"),rr.find("+")+1);
	                    		jj = atoi(rr.c_str());
	                    		YY = YY + jj;
	                    	}else{
	                    		rr.erase(rr.find("Y"),rr.find("+")+1);
	                    		jj = atoi(rr.c_str());
	                    		YY = YY + jj;
	                    	} 
	                    }	       			
	          			
	          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
	                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
	                    		rr.erase(rr.find("]"));
	                    		rr.erase(rr.find("M"),rr.find("+")+1);
	                    		jj = atoi(rr.c_str());
	                    		MM = MM + jj;
	                    	}else{
	                    		rr.erase(rr.find("M"),rr.find("+")+1);
	                    		jj = atoi(rr.c_str());
	                    		MM = MM + jj;
	                    	} 
	                    }	       		
	          			
	          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
	                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
	                    		rr.erase(rr.find("]"));
	                    		rr.erase(rr.find("D"),rr.find("+")+1);
	                    		jj = atoi(rr.c_str());
	                    		DD = DD + jj;
	                    	}else{
	                    		rr.erase(rr.find("D"),rr.find("+")+1);
	                    		jj = atoi(rr.c_str());
	                    		DD = DD + jj;
	                    	} 
	                    }	       		
	       				
	          		}
	       			
	       			char fir[11];
	       			sprintf(fir, "%04d%02d%02d" ,YY ,MM ,DD ); 
	       			return fir; 
	       		}      
	            //----end----------------------------------
              if(temp.find("MMDD")>=0 && temp.find("MMDD")<=20){
          	  	string rr;
          		if(args.size()>=2){
          		for(int i =2 ;i<args.size(); i++ ){
          			rr = args[i].c_str();
          		    if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	}else{
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	} 
                    }	       		
                    else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	} 
                    }	       		
          			
          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("M"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM + jj;
                    	}else{
                    		rr.erase(rr.find("M"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM + jj;
                    	} 
                    }	       		
          			
          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	} 
                    }	       		
       				
          		}
          	  }
       			char fir[5];
       			sprintf(fir, "%02d%02d" ,MM ,DD ); 
       			cout<<"fir value MMDD "<<fir<<endl;
       			return fir; 
       		}      
            //----end----------------------------------
          if(temp.find("DD")>=0 && temp.find("DD")<=20){
          	string rr;
          	if(args.size()>=2){
          		for(int i =2 ;i<args.size(); i++ ){
          			rr = args[i].c_str();
                    if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	} 
                    }	       		
          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	} 
                    }	       		
       				
          		}
          	  }
       			char fir[3];
       			sprintf(fir, "%02d",DD ); 
       			return fir; 
       		}      
       		
           //------------------------------------------------------end 	
           //---------------HH---------------------
           if(temp.find("HH")>=0 && temp.find("HH")<=20){
          	string rr;
          	if(args.size()>=2){
          		for(int i =2 ;i<args.size(); i++ ){
          			rr = args[i].c_str();
                    if((rr.find("H")>=0&&rr.find("H")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	}else{
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	} 
                    }	       		
          			else if((rr.find("H")>=0&&rr.find("H")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("H"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH + jj;
                    	}else{
                    		rr.erase(rr.find("H"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH + jj;
                    	} 
                    }	       		
       				
          		}
          	  }
       			char fir[3];
       			sprintf(fir, "%02d",HH ); 
       			return fir; 
       		}      
           //----------------HH--end------------------	
            
	                
	        
	          
         }
            return "*";
        }else if(args[0]==DATETIME ){ 
         time_t t;
         struct tm *loctm;
         loctm=localtime(&t);
         std::cout<<"time value is   "<<time(&t)<<"/"<<localtime(&t)<<std::endl;//can not delete!!
         string temp;
         temp=args[1].c_str();
         int jj;
         string strg ;
          int YY = loctm->tm_year+1900 ;
          int MM = loctm->tm_mon + 1;
          int DD = loctm->tm_mday;
          int HH = loctm->tm_hour;
          int MI = loctm->tm_min;
          int SS = loctm->tm_sec;
          int WE = loctm->tm_wday;
          cout<<"134 row  is   "<<YY<<MM<<DD<<HH<<MI<<SS<<"/"<<WE<<"/"<<loctm->tm_wday<<endl;
          
          if(args.size()==1)return "*";
          
          //--begin---YYYYMMDDHH24MISS------
          if(temp.find("YYYYMMDDHH24MISS")>=0 && temp.find("YYYYMMDDHH24MISS")<=20){
          	string rr;
          	if(args.size()>=2){
          		for(int i =2 ;i<args.size(); i++ ){
          			rr = args[i].c_str();
          			if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	}else{
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	} 
                    }	       			
          			
          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	}else{
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	} 
                    }	       		
          			
          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	} 
                    }	       		
       				
       				else if((rr.find("H")>=0&&rr.find("H")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	}else{
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	} 
                    }	       		
  					
  					else if((rr.find("MI")>=0&&rr.find("MI")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("MI"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI - jj;
                    	}else{
                    		rr.erase(rr.find("MI"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI - jj;
                    	} 
                    }	       			
  					else if((rr.find("SS")>=0&&rr.find("SS")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("SS"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		SS = SS - jj;
                    	}else{
                    		rr.erase(rr.find("SS"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		SS = SS - jj;
                    	} 
                    }	       			
  					
  					else if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("Y"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY + jj;
                    	}else{
                    		rr.erase(rr.find("Y"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY + jj;
                    	} 
                    }	       			
          			
          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("M"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM + jj;
                    	}else{
                    		rr.erase(rr.find("M"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM + jj;
                    	} 
                    }	       		
          			
          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	} 
                    }	       		
       				
       				else if((rr.find("H")>=0&&rr.find("H")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("H"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH + jj;
                    	}else{
                    		rr.erase(rr.find("H"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH + jj;
                    	} 
                    }	       			
          			else if((rr.find("MI")>=0&&rr.find("MI")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("MI"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI + jj;
                    	}else{
                    		rr.erase(rr.find("MI"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI + jj;
                    	} 
                    }	       			
          			else if((rr.find("SS")>=0&&rr.find("SS")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("SS"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		SS = SS + jj;
                    	}else{
                    		rr.erase(rr.find("SS"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		SS = SS + jj;
                    	} 
                    }	       			
          		}
          	  }
       			char fir[15];
       			sprintf(fir, "%04d%02d%02d%02d%02d%02d" ,YY ,MM ,DD,HH,MI,SS ); 
       			return fir; 
       		}      
            //----end------------------------------------
            
            //----begin----YYYYMMDDHH24MI------
            else if(temp.find("YYYYMMDDHH24MI")>=0 && temp.find("YYYYMMDDHH24MI")<=20){
          	string rr;
          	if(args.size()>=2){
          		for(int i =2 ;i<args.size(); i++ ){
          			rr = args[i].c_str();
          			if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	}else{
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	} 
                    }	       			
          			
          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	}else{
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	} 
                    }	       		
          			
          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	} 
                    }	       		
       				
       				else if((rr.find("H")>=0&&rr.find("H")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	}else{
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	} 
                    }	       		
  					
  					else if((rr.find("MI")>=0&&rr.find("MI")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("MI"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI - jj;
                    	}else{
                    		rr.erase(rr.find("MI"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI - jj;
                    	} 
                    }	       			
  					
  					else if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("Y"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY + jj;
                    	}else{
                    		rr.erase(rr.find("Y"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY + jj;
                    	} 
                    }	       			
          			
          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("M"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM + jj;
                    	}else{
                    		rr.erase(rr.find("M"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM + jj;
                    	} 
                    }	       		
          			
          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD + jj;
                    	} 
                    }	       		
       				
       				else if((rr.find("H")>=0&&rr.find("H")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("H"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH + jj;
                    	}else{
                    		rr.erase(rr.find("H"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH + jj;
                    	} 
                    }	       			
          			else if((rr.find("MI")>=0&&rr.find("MI")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("MI"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI + jj;
                    	}else{
                    		rr.erase(rr.find("MI"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		MI = MI + jj;
                    	} 
                    }	       			
          				
          		}
          	  }
       			char fir[13];
       			sprintf(fir, "%04d%02d%02d%02d%02d" ,YY ,MM ,DD,HH,MI ); 
       			return fir; 
       		}      
            //----end--------------------------------------
            
            //----begin--YYYYMMDDHH24------------------------------
            else if(temp.find("YYYYMMDDHH24")>=0 && temp.find("YYYYMMDDHH24")<=20){
          	string rr;
          	if(args.size()>=2){
          		for(int i =2 ;i<args.size(); i++ ){
          			rr = args[i].c_str();
          			if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	}else{
                    		rr.erase(rr.find("Y"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY - jj;
                    	} 
                    }	       			
          			
          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	}else{
                    		rr.erase(rr.find("M"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		MM = MM - jj;
                    	} 
                    }	       		
          			
          			else if((rr.find("D")>=0&&rr.find("D")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	}else{
                    		rr.erase(rr.find("D"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		DD = DD - jj;
                    	} 
                    }	       		
       				
       				else if((rr.find("H")>=0&&rr.find("H")<=20)&&(rr.find("-")>=0&&rr.find("-")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	}else{
                    		rr.erase(rr.find("H"),rr.find("-")+1);
                    		jj = atoi(rr.c_str());
                    		HH = HH - jj;
                    	} 
                    }	       		
  					
  					else if((rr.find("Y")>=0&&rr.find("Y")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.find("]"));
                    		rr.erase(rr.find("Y"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY + jj;
                    	}else{
                    		rr.erase(rr.find("Y"),rr.find("+")+1);
                    		jj = atoi(rr.c_str());
                    		YY = YY + jj;
                    	} 
                    }	       			
          			
          			else if((rr.find("M")>=0&&rr.find("M")<=20)&&(rr.find("+")>=0&&rr.find("+")<=20)){
                    	if(rr.find(rr.find("]"))>=0&&rr.find(rr.find("]"))<=20){
                    		rr.erase(rr.f