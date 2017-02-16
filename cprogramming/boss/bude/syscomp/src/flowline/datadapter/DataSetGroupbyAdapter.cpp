#include "DataSetGroupbyAdapter.h"

#include "flowline/adapter/DataProcessorAdapter.h"
#include "base/ClassRegister.h"
#include "base/StringUtil.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
#include "flowline/Element.h"
#include "flowline/flbase.h"

#include <string>
#include <vector>
#include <map>
namespace flowline
{
namespace dataprocessor
{
void DataSetGroupbyAdapter::init(FlowLineDao& dao)
{      
    const Metadata* dstmetadata = this->getMetadata(""); 
		if(dstmetadata == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}                         
	PDataSet dest = dstmetadata->createDataSet();          
    this->getFieldName(dest,this->m_dstfld);
    this->getFieldDataType(dest,this->m_dsttype);
};  
        
//得到字段名数组
void DataSetGroupbyAdapter::getFieldName(PDataSet data,vector<string>& fields)
{
    fields.clear();
    int fcount = data->fieldCount();
    for(int i=0; i<fcount; i++) 
    {
        fields.push_back(data->getField(i).getFieldName());
    }
        
}

//TYPE
void DataSetGroupbyAdapter::getFieldDataType(PDataSet data, vector< DataType >& datatype)
{ 
    datatype.clear();  
		int fcount = data->fieldCount();
    for(  int i = 0;i < fcount ;i++)
    {
        datatype.push_back(data->getField(i).getDataType());
    }   
   
}



//得到group by 字段
 static int stat_int = -1;

void DataSetGroupbyAdapter::getgroupbyScript(Element* element, 
                         vector< pair< int ,TransType> >& groupbyidx , PDataSet src)
{
		//cout << "DataSetGroupbyAdapter::getMappingScript:mappingidx:"<<groupbyidx.size()<<endl;
		ASSERT(element!=0);
		ASSERT(src!=0);
		groupbyidx.clear();			
	    string collect[]={"sum","ave","max","min","count"};
		int dstsize = this->m_dstfld.size();
		//cout<<"dstsize value is-98 .."<<dstsize<<endl;
		string attr;
		TransType fieldtype = NONE ;
	 
		for(int i = 0 ;i < dstsize; i++ )//-1是为了添加一行为了求和
		{
    	attr = element->getExtendedAttribute(this->m_dstfld[i]);  
      //  cout << "getExtendedAttribute:"<< this->m_dstfld[i] << "=" << attr <<endl;     
		if(!attr.empty())
		{
			int jj = 0;
			int ct =0;
			int pos=-1;
	        for(; ct<5; ct++)
	 	   	{
	       		pos=attr.find(collect[ct]);
 		       
 		     	if(pos==0)break;
	     	
	     	}    
			
				 if(ct==4)
				 {
				 stat_int = i;
				// cout<<"aaaa"<<i<<endl;
				 }
				if(pos>=0)
				{
					attr.erase(0,attr.find('('));
	      			attr.erase(attr.find(')'),attr.find(')'));
 	    			attr.erase(0,attr.find('.')+1);
  	    			int srcidx = src->getFieldIndex(attr.c_str());
	      			if(srcidx>=0)
	      			{   
	      				jj = srcidx;
 		     			srcidx = (ct+1)*100 + srcidx;	
 		     		//	cout<<"100 row value is ...    "<<srcidx<<endl;     			
      					if(this->m_dsttype[i] == dataset::FTINT||this->m_dsttype[i] == dataset::FTDOUBLE||this->m_dsttype[i] == dataset::FTINT64)
      					{
	      				   fieldtype = checkTransType(src->getField(jj).getDataType(),
               	                            this->m_dsttype[i]);
               	           groupbyidx.push_back(pair< int ,TransType> (srcidx,fieldtype));
	      			    } 
	      			}
			 	}
		 		else 
		 		{
        			//配置里没有找到，就从源头上找
        			attr.erase(0,attr.find('.')+1);
	     			int ii = src->getFieldIndex(attr.c_str());
	     			//int srcidx = src->getFieldIndex(m_dstfld[i].c_str());  
	     			if(ii>=0){
	     				     
	     				fieldtype = checkTransType(src->getField(ii).getDataType(),
                	                      this->m_dsttype[i]);
	     				groupbyidx.push_back(pair< int ,TransType> (ii,fieldtype));                  
		   			}
		   			else
   	     				 THROW_P1(DefinitionException, "执行转换目标字段找不到源字段或脚本:%s", this->m_dstfld[i].c_str());
		   		}
	   	  }
   	      else
   	      THROW_P1(DefinitionException, "执行转换目标字段找不到源字段或脚本:%s", this->m_dstfld[i].c_str());
   	  }
 }
 
 //得到转换类型
DataSetGroupbyAdapter::TransType DataSetGroupbyAdapter::checkTransType(dataset::DataType srctype, dataset::DataType dsttype)
{       
   if(srctype == dsttype) 
   {      
   	 switch(srctype ){
   			case  dataset::FTINT:
   					 return ISINT;
  		    case dataset::FTDOUBLE:
  		    		 return ISDOUBLE;
   			case dataset::FTINT64:
   				     return ISINT64;
   			default :
   					break;
   		}
   }
   
   if(srctype == dsttype)return NONE;
   switch(srctype){
        case dataset::FTSTRING:
            return (dsttype == dataset::FTINT)? STR2INT: (dsttype == dataset::FTDOUBLE)? STR2DOUBLE :STR2INT64 ;                   
        case dataset::FTDOUBLE :
            return (dsttype == dataset::FTSTRING)? DOUBLE2STR : DOUBLE2INT;
        case dataset::FTINT :
             return (dsttype == dataset::FTSTRING)?INT2STR:(dsttype == dataset::FTDOUBLE)? INT2DOUBLE : INT2INT64 ;
        case dataset::FTINT64 : 
            return (dsttype == dataset::FTSTRING)? INT642STR  : NONE;
        default : 
        	break ;               
   }  
	return NONE;
} 

 /*
void DataSetGroupbyAdapter::printVector( vector<dataset::Data>& datas){
	vector<dataset::Data>::iterator id = datas.begin();
	cout << "-------printVector---------"<< endl;
	for (;id!=datas.end();id++)
		{
		  cout << *id<<endl;
		}
		cout << "-------printVector---------"<< endl;
}
*/
//处理文件
void DataSetGroupbyAdapter::process(Context & ctx, pair<string, PDataSet>& srcdata, 
                                   map<string, PDataSet>& result)
{
   
    int destfldcount = this->m_dstfld.size();    
	const Metadata* dstmetadata = this->getMetadata(); 
	if(dstmetadata == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}
	PDataSet dstds = dstmetadata->createDataSet(); 
	//------------
	const Metadata* dstmetadata1 = this->getMetadata(); 
	if(dstmetadata1 == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}
	PDataSet dstds1 = dstmetadata1->createDataSet(); 
	//------------
	
	dstds->addField(dataset::Field("fieldcount",FTINT,20));
	//dataset::Field fld("fieldcount",FTINT,20);
  	//fld.setTag(-1);
  	//dstds->addField(fld);
	string srcname = srcdata.first;
	PDataSet srcds = srcdata.second; 

    //cout<<"[srcds]"<<*srcds.get()<<endl;
	
	DataSet::iterator it = srcds->begin(); //源文件的头指针
	this->getgroupbyScript(this->getTarget(),this->m_groupbyidx, srcds); 
    vector< pair< string ,dataset::SORT_TYPE> > groupbyname;
    groupbyname.clear();//太重要了！！
    vector <int> averow;//存放AVE字段的下标例如：1，2，3。。。
    groupsrc.clear();
    for(int ii=0; ii < destfldcount; ii++)
    {
     	 int jsidx = m_groupbyidx[ii].first;
     	 if(jsidx>=0&&jsidx<100){     	 
     	 		groupbyname.push_back(pair<string,dataset::SORT_TYPE> (this->m_dstfld[ii], dataset::ASC));
       			//cout<< "this->m_dstfld[ii] value row group by row   "<<this->m_dstfld[ii]<<"/"<<jsidx<<endl;
       			groupsrc.push_back(jsidx);
       		}                            
    }
                 
		DataSet::iterator itdst;//目标的叠代指针
		
		TransType fieldtype = NONE;

   vector<dataset::Data> datas;    
   int  tmpint = 0;
   double tmpdouble = 0.0;
   const char * tmpchar;
   TransType transtype = NONE;
   vector <int>::iterator itsrc;
   DataSet::index_iterator it1,it2;
   bool ISAVE = true; 
   int64_t  stemp64  = 0;
   int64_t  dtemp64  = 0; 
   
   for(; it!= srcds->end(); it++)
    {   	
      if(it != srcds->begin())
      {
        dstds->dropIndex("groupbyindex");
        dstds->buildIndex("groupbyindex",groupbyname); 
     //  cout<<"[dstds]-----------------------"<<*dstds.get()<<endl;
	  }
		
        if(ISAVE)
        {
			 for(int i = 0; i < destfldcount; i++)//统计求平均值的列
			{						
				int avecount= m_groupbyidx[i].first;//取得下标值，
				if(avecount/100==2)averow.push_back(i);
									
			}
			ISAVE = false;
		} 			
       datas.clear();
     
         if(it != srcds->begin()){
	        for(itsrc=this->groupsrc.begin(); itsrc!=this->groupsrc.end(); itsrc++)
	        {    
	        	
	        	datas.push_back((*it)[*itsrc]);//把groupby的字段放入datas中
	       	//	cout<<"datas.push_back value :zzzcccc"<<(*it)[*itsrc]<<endl;
	        }   
       }
       
       bool	 r = dstds->locate(it1, it2, "groupbyindex", datas);//录找与data中相同值的索引行号
       		
       datas.clear();
     //  cout<<"r value is .."<<r<<endl;
       if(r) //如果能找到执行
        {
           
            for(int i = 0; i < destfldcount ; i++)//为了添加一列
			{
					 			
				int jsidx = m_groupbyidx[i].first;//取得下标值，
			    transtype = m_groupbyidx[i].second; 
				   if(jsidx>=100)
			       {
			     	int choice=jsidx/100;      
			     	transtype = m_groupbyidx[i].second; 
			     	jsidx=jsidx%100;
			     
					if(choice == 1)      //求和
					{
						switch(transtype)
						{
							
							case ISINT:
					         (*it1)[i] =int((*it1)[i]) + int((*it)[jsidx]);
							  break;
							case NONE:
					          (*it1)[i] =int((*it1)[i]) + int((*it)[jsidx]);
							   break;	
							case  ISDOUBLE:
								(*it1)[i] =double((*it1)[i]) + double((*it)[jsidx]);	
						    	break;
							case  ISINT64:
							{
								
								stemp64  = (*it)[jsidx];
						       	dtemp64  = (*it1)[i]; 
						        dtemp64  =  dtemp64 + stemp64;	
						    	(*it1)[i] = dtemp64;	
						      }
						    	break;
							case INT2DOUBLE:
					        {   
					         	(*it1)[i] =double((*it1)[i]) + double((*it)[jsidx]);	
						    }
						    break;
							case INT2INT64:
						    {
						        stemp64  = (*it)[jsidx];
						        dtemp64  = (*it1)[i]; 
						        dtemp64  += stemp64;	
						    	(*it1)[i] = dtemp64;	
						    }
						    	break;
							default:
								break;
			     		 }
			     	//(*it1 )[destfldcount] =	int((*it1 )[destfldcount]) + 1;
			     
			     	}

			         if(choice == 2)//求平均值AVE
			     	{
			            	switch(transtype)
							{
							    case NONE:
					          	 (*it1)[i] =int((*it1)[i]) + int((*it)[jsidx]);
							      break;	
								case ISINT:
						         (*it1)[i] =int((*it1)[i]) + int((*it)[jsidx]);
								  break;	
								case  ISDOUBLE:
									(*it1)[i] =double((*it1)[i]) + double((*it)[jsidx]);	
							    	break;
				     		    case  ISINT64:
									stemp64  = (*it)[jsidx];
						        	dtemp64  = (*it1)[i]; 
						        	dtemp64  += stemp64;	
						    		(*it1)[i] = dtemp64;	
							    	break;
						    	case INT2DOUBLE:
					       	    {   
					         	   (*it1)[i] =double((*it1)[i]) + double((*it)[jsidx]);	
						   		}
						    		break;
								case INT2INT64:
						    	{
						        	stemp64  = (*it)[jsidx];
						        	dtemp64  = (*it1)[i]; 
						        	dtemp64  += stemp64;	
						    		(*it1)[i] = dtemp64;	
						    	 }
						    		break;
							default:
								break;	
			     		    }
			     	
			      //		(*it1 )[destfldcount] =	int((*it1 )[destfldcount]) + 1;
			      	}
			      	 if(choice == 3)//求最大值MAX
			       	{  
			        	switch(transtype)
		           	    {
	             		    case  NONE:
						   				(*it1)[i] = (int((*it1)[i])>int((*it)[jsidx])) ? int((*it1)[i]) : int((*it)[jsidx]);
						        break;
	             		    case  ISINT:
						   				(*it1)[i] = (int((*it1)[i])>int((*it)[jsidx])) ? int((*it1)[i]) : int((*it)[jsidx]);
						        break;
						    case  ISDOUBLE:
						          (*it1)[i] = (double((*it1)[i])>double((*it)[jsidx])) ? double((*it1)[i]) : double((*it)[jsidx]);
						        break;
						   
	            		 	case  ISINT64:
								  stemp64  = (*it)[jsidx];
					        	  dtemp64  = (*it1)[i]; 
					        	  dtemp64  = (dtemp64 > stemp64) ? dtemp64 : stemp64;
					              (*it1)[i] = dtemp64;	
						    	break;
						    case INT2DOUBLE:
					       	    {   
					         	   (*it1)[i] = (double((*it1)[i])>double((*it)[jsidx])) ? double((*it1)[i]) : double((*it)[jsidx]);
						   		}
						    		break;
							case INT2INT64:
					    	{
					        	stemp64  = (*it)[jsidx];
					        	dtemp64  = (*it1)[i]; 
					        	dtemp64  = (dtemp64 > stemp64) ? dtemp64 : stemp64;
					            (*it1)[i] = dtemp64;	
					    	 }
					    		break;
							default:
								
								break;	
			     		    }	
					        //(*it1 )[destfldcount] =	int((*it1 )[destfldcount]) + 1;
					    }
			        if(choice == 4) //求最小值MIN
     				{
						switch(transtype)
						  {
	              			case  NONE:
						   				(*it1)[i] = (int((*it1)[i])>int((*it)[jsidx])) ? int((*it)[jsidx]) : int((*it1)[i]);
						        break;
	             		    case  ISINT:
						   				(*it1)[i] = (int((*it1)[i])>int((*it)[jsidx])) ?int((*it)[jsidx])  :int((*it1)[i]);
						        break;
						    case  ISDOUBLE:
						          (*it1)[i] = (double((*it1)[i])>double((*it)[jsidx])) ? (*it)[jsidx] : double((*it1)[i]);
						        break;
						   case  ISINT64:
								{
								  stemp64  = (*it)[jsidx];
					        	  dtemp64  = (*it1)[i]; 
					        	  dtemp64  = (dtemp64 < 3) ? dtemp64 : stemp64;
					              (*it1)[i] = dtemp64;	
						    	}
						    	break;
						    case INT2DOUBLE:
					       	    {   
					         	   (*it1)[i] = (double((*it1)[i])<double((*it)[jsidx])) ? double((*it1)[i]) : double((*it)[jsidx]);
						   		}
						    		break;
							case INT2INT64:
					    	{
					        	stemp64  = (*it)[jsidx];
					        	dtemp64  = (*it1)[i]; 
					        	dtemp64  = (dtemp64 < stemp64) ? dtemp64 : stemp64;
					            (*it1)[i] = dtemp64;	
					    	 }
					    		break;
							default:
								
								break;	
						}
			            	//(*it1 )[destfldcount] =	int((*it1 )[destfldcount]) + 1;
			     	}
			  //      if(choice==5) //求count 统计个数
			 
				}
            }    
            (*it1 )[destfldcount] =	int((*it1 )[destfldcount]) + 1;
      }  
      else //如果不能找到就添加一行
      {
         	itdst = dstds->pushBack();//添加一行
            for(int i=0; i < destfldcount; i++)//为了添加一列 -1
            {       
            	   
			      	int addidx = m_groupbyidx[i].first;
			      	transtype = m_groupbyidx[i].second; 
			      	if(addidx>=100)addidx=addidx%100;
			      	transtype = m_groupbyidx[i].second; 
					switch(transtype)
					{
              			
              			case  NONE:
					        (*itdst)[i] = (*it)[addidx];	
					        break;
              			case  ISINT:
					        (*itdst)[i] = (*it)[addidx];	
					        break;
				        case  ISINT64:	
				            (*itdst)[i] = (*it)[addidx];
				    
				        	break;
				        case  ISDOUBLE:
				        	(*itdst)[i] = (*it)[addidx];	
				        	break;
              			
					    case  STR2INT:
					        {
					        	tmpchar = (*it)[addidx].c_str();	
					        	(*itdst)[i] = atoi(tmpchar);
					        }
						    break;
					    case STR2DOUBLE:
						    {
						        tmpchar = (*it)[addidx].c_str();	
						        (*itdst)[i] = atof(tmpchar);
						    }
						    break;
					    case INT2STR:
						    {   tmpint = (*it)[addidx];	
						        char tchar[10];
						        sprintf(tchar,"%d",tmpint);
						        (*itdst)[i] = tchar;
						    }
						    break;
					    case DOUBLE2STR:
					         {   
					         	tmpdouble = (*it)[addidx];	
						        char tchar[20];
						        sprintf(tchar,"%f",tmpdouble);
						        (*itdst)[i] = tchar;
						    }
						     break;
					    case INT2DOUBLE:
					        {   
					         	tmpint = (*it)[addidx];							      
						        (*itdst)[i] = (double)tmpint;
						    }
						    break;
					    case DOUBLE2INT:
					        {   
					         	tmpdouble = (*it)[addidx];							      
						        (*itdst)[i] = (int)tmpdouble;
						    }
						     break;
			
						 case  STR2INT64:
					        {
					        	tmpchar = (*it)[addidx].c_str();	
					        	(*itdst)[i] = StringUtil::toValue<int64_t>(tmpchar);
					        }
						    break;
					    case INT2INT64:
						    {
						       
						        stemp64 = (*it)[addidx];
						    	(*itdst)[i] = stemp64;
				
						    }
						    break;
					    case INT642STR:
					    	 
						    {  
						        ostringstream oos;
						    	oos << (*it)[addidx];							        
						        (*itdst)[i] = oos.str().c_str();
						    }
						    break;
					   default:
					         (*itdst)[i] = (*it)[addidx];	
					         
							
						    break;
            	}
			      	
          }
       			     (*itdst)[destfldcount] = 1;
       		
       }
        dstds->dropIndex("groupbyindex");
   }    
    dstds->dropIndex("groupbyindex");
    
    vector <int> ::iterator itave=averow.begin();
    DataSet::iterator itend = dstds->begin(); //result文件的头指针
    //
   // DataSet::iterator itend1 = dstds->begin();
     if(1) {
		int  destfldcount = this->m_dstfld.size();
		for(; itend!= dstds->end(); itend++){
    		for(int i = 0; i < destfldcount; i++){
				for(itave=averow.begin();itave!=averow.end();itave++){    	
			    	if(*itave==i){
			    			if(dstds->getField(i).getDataType()==dataset::FTINT64){
			    				dtemp64 = (*itend)[destfldcount];
			    				stemp64	= (*itend)[i];
			    				stemp64	= stemp64/dtemp64;
			    			    (*itend)[i] = stemp64;
			    			}
			    			else if(dstds->getField(i).getDataType()==dataset::FTINT){
			    				double  temp = (double((*itend)[i]))/(double((*itend)[destfldcount]));
			    				(*itend)[i] = int(temp);
			    			}
			    			else{
			    			     double temp = (double((*itend)[i]))/(double((*itend)[destfldcount]));
							     (*itend)[i] = temp ;
							}			    
			    	}
    	  		}	
       		}
    	}
	}
     DataSet::iterator itend1 = dstds1->begin();
     itend = dstds->begin();
     for(; itend!= dstds->end(); itend++){
     	itend1 = dstds1->pushBack();
     	for(int i = 0; i < destfldcount; i++){    
        	 if(stat_int == i ){
        	 	stemp64 = (*itend)[destfldcount];
        	 	(*itend1)[i] = stemp64;
         	 }
         	 else
        	 (*itend1)[i] = (*itend)[i];
        }
     }
     result.insert(pair<string , PDataSet> (string(),dstds1));
     this->ASS.push_back(dstds);
     groupbyname.clear();
     LOG_TRACE_P1(ctx.getLogger(),"DataSetGroupbyAdapter:result:%d",dstds1->size());
     //cout<<"dstds1:" <<*dstds1.get()<<endl; 
    // cout<<"dstds .  "<<*dstds.get() <<endl;
     //FL_TRACE_P1("DataSetGroupbyAdapter::finish,SIZE:%d",res.size());
    
	
}      
    
	
   
   
   
void DataSetGroupbyAdapter::finish(Context& ctx, bool isAllFinished, map<string, PDataSet>& result )
{
   //DataSet::iterator itdst;//目标的叠代指针
   // itdst = dstds->pushBack();//添加一行
    //this->ASS.push_back(dstds);
  /*  
    DataSet::iterator fit ;//= srcds->begin();
    DataSet::iterator fit1; 
    int destfldcount = this->m_dstfld.size(); 
    vector< pair< string ,dataset::SORT_TYPE> > groupbyname;
    groupsrc.clear();
    
    for(int ii=0; ii < destfldcount; ii++)
    {
     	 int jsidx = m_groupbyidx[ii].first;
     	 if(jsidx>=0&&jsidx<100){     	 
     	 		groupbyname.push_back(pair<string,dataset::SORT_TYPE> (this->m_dstfld[ii], dataset::ASC));
       		//	cout<< "this->m_dstfld[ii] value row group by row   "<<this->m_dstfld[ii]<<"/"<<jsidx<<endl;
       			groupsrc.push_back(jsidx);
       		}                            
    }
    
    if(isAllFinished){
    	const Metadata* dstmetadata1 = this->getMetadata(); 
		if(dstmetadata1 == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}
        
	    PDataSet fdstds = dstmetadata1->createDataSet();
	    fdstds->addField(dataset::Field("finshcount",FTINT,20));
	    
	    
	    const Metadata* dstmetadata2 = this->getMetadata(); 
		if(dstmetadata2 == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}
        
	    PDataSet fdstds1 = dstmetadata2->createDataSet();
	    fdstds1->addField(dataset::Field("finshcount2",FTINT,20));
	    
	    
	    
	    const Metadata* dstmetadata = this->getMetadata(); 
		if(dstmetadata == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}
        
	    PDataSet frefer = dstmetadata->createDataSet();
	//-----------------------------------
	    vector<PDataSet>::iterator ass=ASS.begin();	
	    PDataSet fstr;
	    DataSet::iterator fdst;
	    DataSet::iterator fdst1;
	    TransType transtype = NONE;
	    int64_t  stemp64  = 0;
   		int64_t  dtemp64  = 0; 
	//----------------------------------    
	    for(;ass!=ASS.end();ass++){
	    	fstr = *ass;
	    	fit  = fstr->begin();
	    	for(;fit!=fstr->end();fit++){
	    	   fdst = fdstds->pushBack();
	    	   for(int i =0 ;i <destfldcount+1 ;i++)
	    	   (*fdst)[i] = (*fit)[i];
	    //	   cout<<"value is    "<< (*fdst)[i]<<endl;}
	    	   
	    	}
	    	
	   // cout<<" vector<PDataSet>::iterator ass=ASS.begin();	"<<*(*ass).get()<<endl;
    	}
      // cout<<"finished targetstore value is             "<<*fdstds.get()<<endl;// 复制后但未处理的数据
       vector <int> averow;	
       vector<dataset::Data> datas;
       vector <int>::iterator itsrc;
       DataSet::index_iterator it1,it2;
       bool ISAVE = true;
	   for(fit1=fdstds->begin(); fit1!=fdstds->end(); fit1++){
	   		if(fit1 != fdstds->begin()){
		        fdstds->dropIndex("groupbyindex2");
		        fdstds->buildIndex("groupbyindex2",groupbyname); 
		     //  cout<<"[dstds]-----------------------"<<*dstds.get()<<endl;
			  }
				
		    if(ISAVE){
				for(int i = 0; i < destfldcount; i++){//统计求平均值的列						
					int avecount= m_groupbyidx[i].first;//取得下标值，
					if(avecount/100==2)averow.push_back(i);						
				}
					ISAVE = false;
			} 			
		    datas.clear();
		    if( fit1 != fdstds->begin()){
	        	for(itsrc=this->groupsrc.begin(); itsrc!=this->groupsrc.end(); itsrc++)
	        	{    
	        	
	        		datas.push_back((*fit1)[*itsrc]);//把groupby的字段放入datas中
	       			//cout<<"datas.push_back value :zzzcccc"<<(*fit1)[*itsrc]<<endl;
	        	}   
       		}
		    bool  r = fdstds->locate(it1, it2, "groupbyindex2", datas);//录找与data中相同值的索引行号
	   		datas.clear();
     	  //  cout<<"r value is .."<<r<<endl;
       		
       		if(r){//如果能找到执行
  				 for(int i = 0; i < destfldcount ; i++){//为了添加一列			
					int jsidx = m_groupbyidx[i].first;//取得下标值，
			    	transtype = m_groupbyidx[i].second; 
			     	if(jsidx>=100){
			     		int choice=jsidx/100;      
			     		transtype = m_groupbyidx[i].second; 
			     		jsidx=jsidx%100;
			      		if(choice == 1){
			      			switch(transtype)
							{
							
								case ISINT:
						         (*fdst1)[i] =int((*fdst1)[i]) + int((*fit1)[jsidx]);
								  break;
								case NONE:
						          (*fdst1)[i] =int((*fdst1)[i]) + int((*fit1)[jsidx]);
								   break;	
								case  ISDOUBLE:
									(*fdst1)[i] =double((*fdst1)[i]) + double((*fit1)[jsidx]);	
							    	break;
								case  ISINT64:
								{
									
									stemp64  = (*fit1)[jsidx];
							       	dtemp64  = (*fdst1)[i]; 
							        dtemp64  =  dtemp64 + stemp64;	
							    	(*fdst1)[i] = dtemp64;	
							      }
							    	break;
								case INT2DOUBLE:
						        {   
						         	(*fdst1)[i] =double((*fdst1)[i]) + double((*fit1)[jsidx]);	
							    }
							    break;
								case INT2INT64:
							    {
							        stemp64  = (*fit1)[jsidx];
							        dtemp64  = (*fdst1)[i]; 
							        dtemp64  += stemp64;	
							    	(*fdst1)[i] = dtemp64;	
							    }
							    	break;
								default:
									break;
			     		 	}
			      		}
			      		if(choice == 2){//ave
			      			switch(transtype)
							{
							    case NONE:
					          	 (*fdst1)[i] =int((*fdst1)[i]) + int((*fit1)[jsidx]);
							      break;	
								case ISINT:
						         (*fdst1)[i] =int((*fdst1)[i]) + int((*fit1)[jsidx]);
								  break;	
								case  ISDOUBLE:
									(*fdst1)[i] =double((*fdst1)[i]) + double((*fit1)[jsidx]);	
							    	break;
				     		    case  ISINT64:
									stemp64  = (*fit1)[jsidx];
						        	dtemp64  = (*fdst1)[i]; 
						        	dtemp64  += stemp64;	
						    		(*fdst1)[i] = dtemp64;	
							    	break;
						    	case INT2DOUBLE:
					       	    {   
					         	   (*fdst1)[i] =double((*fdst1)[i]) + double((*fit1)[jsidx]);	
						   		}
						    		break;
								case INT2INT64:
						    	{
						        	stemp64  = (*fit1)[jsidx];
						        	dtemp64  = (*fdst1)[i]; 
						        	dtemp64  += stemp64;	
						    		(*fdst1)[i] = dtemp64;	
						    	 }
						    		break;
							default:
								break;	
			     		    }
			      		}
			      		if(choice == 3){//MAX
			      			switch(transtype)
		           	    	{
	             		    case  NONE:
						   				(*fdst1)[i] = (int((*fdst1)[i])>int((*fit1)[jsidx])) ? int((*fdst1)[i]) : int((*fit1)[jsidx]);
						        break;
	             		    case  ISINT:
						   				(*fdst1)[i] = (int((*fdst1)[i])>int((*fit1)[jsidx])) ? int((*fdst1)[i]) : int((*fit1)[jsidx]);
						        break;
						    case  ISDOUBLE:
						          (*fdst1)[i] = (double((*fdst1)[i])>double((*fit1)[jsidx])) ? double((*fdst1)[i]) : double((*fit1)[jsidx]);
						        break;
						   
	            		 	case  ISINT64:
								  stemp64  = (*fit1)[jsidx];
					        	  dtemp64  = (*fdst1)[i]; 
					        	  dtemp64  = (dtemp64 > stemp64) ? dtemp64 : stemp64;
					              (*fdst1)[i] = dtemp64;	
						    	break;
						    case INT2DOUBLE:
					       	    {   
					         	   (*fdst1)[i] = (double((*fdst1)[i])>double((*fit1)[jsidx])) ? double((*fdst1)[i]) : double((*fit1)[jsidx]);
						   		}
						    		break;
							case INT2INT64:
					    	{
					        	stemp64  = (*fit1)[jsidx];
					        	dtemp64  = (*fdst1)[i]; 
					        	dtemp64  = (dtemp64 > stemp64) ? dtemp64 : stemp64;
					            (*fdst1)[i] = dtemp64;	
					    	 }
					    		break;
							default:
								
								break;	
			     		    }	
			      		}
			      		if(choice == 4){
			      			switch(transtype)
						  	{
	              			case  NONE:
						   				(*fdst1)[i] = (int((*fdst1)[i])>int((*fit1)[jsidx])) ? int((*fit1)[jsidx]) : int((*fdst1)[i]);
						        break;
	             		    case  ISINT:
						   				(*fdst1)[i] = (int((*fdst1)[i])>int((*fit1)[jsidx])) ?int((*fit1)[jsidx])  :int((*fdst1)[i]);
						        break;
						    case  ISDOUBLE:
						          (*fdst1)[i] = (double((*fdst1)[i])>double((*fit1)[jsidx])) ? (*fit1)[jsidx] : double((*fdst1)[i]);
						        break;
						   case  ISINT64:
								{
								  stemp64  = (*fit1)[jsidx];
					        	  dtemp64  = (*fdst1)[i]; 
					        	  dtemp64  = (dtemp64 < 3) ? dtemp64 : stemp64;
					              (*fdst1)[i] = dtemp64;	
						    	}
						    	break;
						    case INT2DOUBLE:
					       	    {   
					         	   (*fdst1)[i] = (double((*fdst1)[i])<double((*fit1)[jsidx])) ? double((*fdst1)[i]) : double((*fit1)[jsidx]);
						   		}
						    		break;
							case INT2INT64:
					    	{
					        	stemp64  = (*fit1)[jsidx];
					        	dtemp64  = (*fdst1)[i]; 
					        	dtemp64  = (dtemp64 < stemp64) ? dtemp64 : stemp64;
					            (*fdst1)[i] = dtemp64;	
					    	 }
					    		break;
							default:
								
								break;	
							}	
			      		}
			      		
			      	}	
			     }
       			(*fdst1 )[destfldcount] =	int((*fdst1 )[destfldcount]) + int((*fit1)[destfldcount]);
       		} 
	   		else{
	   			fdst1 = fdstds1->pushBack();//添加一行
            	for(int i=0; i < destfldcount+1; i++){ //为了添加一列 -1
            		(*fdst1)[i] = (*fit1)[i];
            	}
            	
	   		}
	  		fdstds->dropIndex("groupbyindex2");
	  		
	   }
	   
	   //------------------------------
	vector <int> ::iterator itave=averow.begin();
    DataSet::iterator itend = fdstds1->begin(); //result文件的头指针
   
     if(1) {
		//int  destfldcount = this->m_dstfld.size();
		for(; itend!= fdstds1->end(); itend++){
    		for(int i = 0; i < destfldcount; i++){
				for(itave=averow.begin();itave!=averow.end();itave++){    	
			    	if(*itave==i){
			    			if(fdstds1->getField(i).getDataType()==dataset::FTINT64){
			    				dtemp64 = (*itend)[destfldcount];
			    				stemp64	= (*itend)[i];
			    				stemp64	= stemp64/dtemp64;
			    			    (*itend)[i] = stemp64;
			    			}
			    			else if(fdstds1->getField(i).getDataType()==dataset::FTINT){
			    				double  temp = (double((*itend)[i]))/(double((*itend)[destfldcount]));
			    				(*itend)[i] = int(temp);
			    			}
			    			else{
			    			     double temp = (double((*itend)[i]))/(double((*itend)[destfldcount]));
							     (*itend)[i] = temp ;
							}			    
			    	}
    	  		}	
       		}
    	}
	}
	   //-----------------------------
	   
	   
	   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	 DataSet::iterator itend1 = frefer->begin();
     itend = fdstds1->begin();
     for(; itend!= fdstds1->end(); itend++){
     	itend1 = frefer->pushBack();
     	for(int i = 0; i < destfldcount; i++){    
        	 if(stat_int == i ){
        	 	stemp64 = (*itend)[destfldcount];
        	 	(*itend1)[i] = stemp64;
         	 }
         	 else
        	 (*itend1)[i] = (*itend)[i];
        }
     }
	   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	   
	   
	   
	    cout<<"finished targetstore value is             "<<*fdstds1.get()<<endl;
        cout<<"frefer value is       "<<*frefer.get()<<endl;
    	result.insert(pair<string , PDataSet> (string(),frefer));
    } 
   
*/
   FL_TRACE_P1("DataSetGroupbyAdapter::finish,SIZE:%d",result.size());	
}


REGISTER_CLASS("DataSetGroupAdapter", DataSetGroupbyAdapter, Adapter);

}

}
//在finish中实现了批量处理功能


