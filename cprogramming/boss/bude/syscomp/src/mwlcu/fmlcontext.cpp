#include "mwlcu/fmlcontext.h" 

using namespace std;
using namespace ebpm4c;

FMLContext::~FMLContext(){
   //if(baseExcep) delete baseExcep;
}
      
void FMLContext::setException(Exception& exp){
  // if(baseExcep) delete baseExcep;
   PException pe(exp.clone());
   baseExcep = pe;
}    

Exception* FMLContext::getException(){    
    return baseExcep.get();   
}  

//是否要为回退
bool FMLContext::getUndoFlag(){
   return this->undoflag;
} 

string FMLContext::getFlowId(){
   return this->flowid;
}

void  FMLContext::setUndoFlag(bool flag){
   this->undoflag = flag;   
}

void FMLContext::setFlowId(const string& flowId){
	this->flowid = flowId;
}  

void FMLContext::addCFmlBuf(const string& bufname,const PCFmlBuf& cfmlbuf){
  	PCFmlBuf pfbuf(cfmlbuf);
  	this->buffers.insert(pair<string, PCFmlBuf>(bufname,pfbuf));
  	//cout << "fmlContext:addCFmlBuf:" << bufname<<endl;
  	// pfbuf->Print();	  	
}
	
///getCFmlBuf//两个buffer可以相加 增加*，   b1+b2  / b1*b2
PCFmlBuf FMLContext::getCFmlBuf(const string& bufname){
		PCFmlBuf pcfmlbuf;		
				
		//int pos = bufname.find("+");
		int pos = bufname.find_first_of("+*");
		if( pos < 0){
	   		map < string, PCFmlBuf >::const_iterator it = buffers.find(bufname);
	   		if (it == buffers.end()){  
				//cout << "创建Buffer:" << bufname <<endl;  
	   			pcfmlbuf = PCFmlBuf(new CFmlBuf());				
	   			this->buffers.insert(pair<string, PCFmlBuf>(bufname,pcfmlbuf));				
	   		}
		} else {
			PCFmlBuf pbuftemp(new CFmlBuf());	
			string strbuf = string(bufname);
			string buf = strbuf.substr(0,pos);
			strbuf = strbuf.substr(pos + 1);
			//递归一下取得前一个Buf
			pcfmlbuf = getCFmlBuf(buf);
			//pbuftemp->Append(*pcfmlbuf.get());			
			pbuftemp->Cover(*pcfmlbuf.get());
			//递归一下取得后一堆Buf
			pcfmlbuf = getCFmlBuf(strbuf);
			char type = bufname.at(pos);
			 
			if(type =='+'){
				pbuftemp->Cover(*pcfmlbuf.get()); 
			} else{			
			    //调用了*号
			   	pbuftemp->Append(*pcfmlbuf.get()); 
			}       
			return pbuftemp;
	   }
		//cout << "fmlContext:getCFmlBuf:" << bufname<<endl;
		//buffers[bufname]->Print();  
  	   	   
		return buffers[bufname];
}

string FMLContext::getFMLFldValue(const string& varname,bool needstr){
	return getFMLFldValue(varname);
}     

string FMLContext::getFMLFldValue(const string& varname){
	string value = "";
	PCFmlBuf cfmlbuf;
	try {	  	
		 //cout <<"[LCU]getCFmlBuf-ORG:|" << varname << "|" <<endl;
		 string tmpvar(varname);	
		 string bufname;
         const char * fldname;  
         string fldnamestr;  	
	     int p = tmpvar.find('.');
         int q = tmpvar.find_last_of('.');
         
         if(p > 0 && q > p ){//发现有两个点
         	int f = tmpvar.find(".count");
         	if( f > 0){
	         	bufname = tmpvar.substr(0 , p);
	         	//cout << "求Buffer的长度:" << bufname << "." <<endl;
	         	fldnamestr = tmpvar.substr(p + 1,q-p-1);
	          	fldname = fldnamestr.c_str(); 
	          	cfmlbuf = getCFmlBuf(bufname); 
	          	if(cfmlbuf->IsFldExist(fldname)){ 
	               q = cfmlbuf->GetCount(fldname);
	               char rt[10];
	               sprintf(rt,"%d",q);
	               value = string(rt);	               			
	            }   else {
	               value = "0";
	            }
	            //cout << fldname << ".count="<< value <<endl;	
	            return value;
            }
		 }		
		    	
         p = varname.find('[');
         q = varname.find_last_of(']');		
                  
         int index = 0;
         if( p > 0 ){
         	
         	string xiabiao = varname.substr(p+1,q-p-1);  
         	   
         	int z = xiabiao.find_first_not_of(" 0123456789");//找到有不为数字和空格的下标
         	if(z >= 0){//找到了
         		index = atoi(getFMLFldValue(xiabiao).c_str());
         	}else{
         		index = atoi(xiabiao.c_str());
         	}
         	tmpvar = varname.substr(0,p); 
         } else {
         	tmpvar = string(varname);
         }
                    
         p = tmpvar.find('.');
         if( p > 0 ){
          	bufname = tmpvar.substr(0 , p);
            fldnamestr = tmpvar.substr(p + 1);
          	fldname = fldnamestr.c_str(); 
          	
          	cfmlbuf = getCFmlBuf(bufname);
	       
            if(cfmlbuf->IsFldExist(fldname)){ 
               value = cfmlbuf->GetString(fldname,index);					
            }
            if(cfmlbuf->GetFldType(fldname) == 5 ){
            	   value = "'" + value + "'";               	   
            } else {
                 if(value.empty()) return "0";//不是字符串但为空，则返回0;
            }
            //cout << "[LCU]getCFmlBuf-Value:" << bufname << "/"<< fldname << "/" << index<<"/" << value << "/" <<endl;  
		 }
	} catch (exception & e){
		 string  errinfo = "从FMLBUFFER里取变量值失败,变量名:";
		 errinfo += varname;
		 errinfo += e.what();	
    	 THROW(LCUExecutionException, errinfo);  	
	}
	
	return value;
} 
	
//此函数暂时没有用到
string FMLContext::getBufferedValue(const string& bufname,const string& fldname,int index){
   string value = "-1";
   PCFmlBuf cfmlbuf;
   try{
   		cfmlbuf = getCFmlBuf(bufname);
        if(cfmlbuf){	        		                
   		value = cfmlbuf->GetString(fldname.c_str(),index);
   		if(cfmlbuf->GetFldType(fldname.c_str()) == 5){
   			value = "'" + value + "'";
   		}
       }
    } catch (exception& e){    	
    	throw;
    }
    return value;
} 	 

/*
  PCFmlBuf FMLContext::getCFmlBuf(const string& bufname){}
   两个buffer相加的操作，增加*，   b1+b2  / b1*b2
      + 表示 b1.Cover(b2)。  返回的是一个临时对象：
                 b1里有b2里没有的字段保留；b2里有的字段如果b1里有就把b1里的字段清掉，用b2里字段，如果b1里没有就直接输出b2里的字段。                
      * 表示 b1.Append(b2)。 返回的是一个临时对象，
                 b1里有b2里没有的字段保留；b2里有的字段 如果b1里有就追加在b1的字段记录后面，如果b1里没有就直接输出b2里的字段。
    
     
 注意：
       是右逻辑计算。靠后面的先执行。不允许有括号。
       例如：  b1*b2+b3*b4                
               先计算 b3*b4 然后 b2+  *的返回值 ，最后是 b1* +的返回值。
               相当于用了括号：  b1*(b2+(b3*b4)) 
         

      我用的是递归，所以后面的先执行。





*/