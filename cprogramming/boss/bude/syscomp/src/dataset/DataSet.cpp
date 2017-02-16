#include "DataSet.h"
#include <stdarg.h>
#include <fstream>

using namespace std;
namespace dataset {

Data Row::operator [](int Index) const
{
	//assert(Index <= DataSet_->fieldCount() - 1);

	return Data(&DataSet_->Fields_[Index],
                        (char*)Address_ +
                                DataSet_->Fields_[Index].getOffset());
}

Data Row::fieldByName(const char * fldname) const
{

	int iIndex = DataSet_->getFieldIndex(fldname);
	if ( iIndex >= 0)
	{
         return Data(&DataSet_->Fields_[iIndex],
                        (char*)Address_ +
                                DataSet_->Fields_[iIndex].getOffset());
	}
}




bool Row::operator == (const Row &right)
{
      
    assert(this->DataSet_);
 
    assert(this->DataSet_->SortOpt_.FieldCount);
    
    

    for(unsigned int i = 0; i < this->DataSet_->SortOpt_.FieldCount; ++i)
    {
        int index = this->DataSet_->SortOpt_.FieldIndex[i];

        if(!((*this)[index] == right[index])) return false;
    }
    return true;
}

Row & Row::copy(const Row & right)
{
	if (this == &right) return *this;
	
	if (DataSet_->RowSize_ == right.DataSet_->RowSize_)
	{
		memcpy(Address_, right.Address_,DataSet_->RowSize_);
	}
	
	return *this;
}


bool Row:: operator < (const Row & right ) const
{

    DataSet::SortOption * opt =  & this->DataSet_->SortOpt_;

    for(unsigned int i = 0; i < opt->FieldCount; i++)
    {
        if( opt->Ascend[i] )
	    {
            int index = opt->FieldIndex[i];

	    if( (*this)[index] < right[index] )
                return true;
            else if( (*this)[index] == right[index] )
                continue;
            else
                return false;
        }
        else
	    {
            int index = opt->FieldIndex[i];
            if( (*this)[index] < right[index] )
                return false;
            else if((*this)[index] == right[index] )
                continue;
            else
                return true;
        }
    }
    return false;
}




DataSet * sortDataSet;

int sortHelper( const void * left, const void * right)
{
    Row a(sortDataSet, (void*)left);
    Row b(sortDataSet, (void*)right);
    if( a < b )
        return -1;
    else if( a== b )
        return 0;
    else
        return 1;
}

DataSet::DataSet(BaseAlloc & alc)
         :RowSize_(0),   //通过增加字段赋值
          Begin_(this, 0),
          End_(this, 0),
          Border_(this, 0),
          Address_(0),
          MemorySize_(0)
{
    
    alloc = alc.clone();
    
    Address_ = alloc->getAddress();
    Begin_.Index_ = alloc->getBegin();
    End_.Index_ = alloc->getEnd();
    Border_.Index_ = alloc->getBorder();
	Fields_.clear();
	
	//从共享内存种取得字段
	alloc->getFields(Fields_);
	
	if (Fields_.size() > 0)
		RowSize_ = Fields_[Fields_.size() - 1].Offset_ + Fields_[Fields_.size() - 1].getLength();

}

DataSet::DataSet()
         :RowSize_(0),   //通过增加字段赋值
          Begin_(this, 0),
          End_(this, 0),
          Border_(this, 0),
          Address_(0),
          MemorySize_(0)
{
    
    alloc = new Alloc();
    
	Fields_.clear();
	
}



void DataSet::clear()
{
       
        if (alloc->isAttached() || alloc->isAllocated()) return;
        
        if (Address_ != NULL)
        {
                alloc->free(Address_);
                Address_ = 0;
        }
        
        Begin_.Index_   = 0;
        End_.Index_     = 0;
        Border_.Index_  = 0;
        //删除索引
        dropAllIndex();
}


DataSet::~DataSet()
{
    
         Fields_.clear();
         
         dropAllIndex();
         
         alloc->free(Address_);

         delete alloc;
}

void DataSet::clearField()
{
        if (alloc->isAttached() || alloc->isAllocated()) return;
        
        this->clear();
        Fields_.clear();
}


bool DataSet::createDataSetAs(const DataSet &dst,bool copy)
{
	 
	 clearField();
	 	 
	 for (unsigned int i = 0; i < dst.Fields_.size(); ++i)
	 {
		if(!addField(dst.Fields_[i]))
			return false;
	 }
	 
	 if (copy) //拷贝实际数据!!!未实现
	 {
	 	;
	 }
	 return true;
}
	


bool DataSet::addField(Field fld)
{
	if (alloc->isAttached()) return false; 
	
    for (unsigned int i = 0; i < Fields_.size(); ++i)
    {
        if (strcmp(Fields_[i].FieldName,fld.FieldName) == 0) //不能有重复的字段名称 
           return false;
    }
   
    Fields_.push_back(fld);
    
    resetField();
	
	alloc->addField(Fields_[Fields_.size() - 1]); //共享内存时候需要
    Address_ = alloc->getAddress(); //如果是本地内存则返回空值
	
	/* 
	
    unsigned int iRecordCount =  size();

     //如果这时候表中有数据就需要扩展内存空间
    unsigned int iOldRowSize  = RowSize_ - fld.Length_;

   if (Address_ != 0)  //！！！注意在这种情况下扩展表字段严重影响性能，如果数据量大于> 1000不推荐使用
     {
           if (extend(fld.Length_ * iRecordCount))
           {
               for (unsigned int i = 0; i < iRecordCount && iRecordCount > 1 ; ++i)
               {
                    void * src  = (char*)Address_ +  i  *  RowSize_  + iOldRowSize;

                    void * dest = (char*)Address_ +  (i + 1) * (RowSize_);

                    memmove(dest, src, (iRecordCount - i -1) * iOldRowSize );
                    memset(src,0x00,fld.Length_);
               }
           }
     }*/
     return true;
}

void DataSet::resetField()
{
        if (Fields_.size() > 0)
        {
                Fields_[0].Offset_ = 0;
                for (unsigned int i = 1; i < Fields_.size(); ++i)
                {
                        Fields_[i].linkField(Fields_[i - 1]);
                }

                RowSize_ = Fields_[Fields_.size() - 1].Offset_ + Fields_[Fields_.size() - 1].getLength();
               // alloc->setRowSize(RowSize_);
        }

}

bool DataSet::extend()
{
        MemoryObj mo = alloc->extend(MemoryObj(Address_,MemorySize_));

        if (mo.Address == 0) return false;
        else
        {
                this->Address_     = mo.Address;
                this->MemorySize_ = mo.Size;

                Border_ = DataSet::iterator(this,MemorySize_ / RowSize_);
	       
                if (Border_ == End_)
                {
                	return false;
				}
				alloc->setBorder(Border_.Index_);
		             

                return true;
        }
      
}

bool DataSet::extend(size_t add)
{
        MemoryObj mo = alloc->extend(MemoryObj(Address_,MemorySize_),add);

        if (mo.Address == 0) return false;
        else
        {
                this->Address_     = mo.Address;
                this->MemorySize_ = mo.Size;

                Border_ = DataSet::iterator(this,MemorySize_ / RowSize_); 	
               
                if (Border_ == End_) 
                {
                	return false;
                }
               
                alloc->setBorder(Border_.Index_);
               
                return true;
        }
}


DataSet::iterator DataSet::pushBack()
{
	if (Fields_.size() == 0)
		 throw Exception("DataSet is no Fields.");	
	while( End_ == Border_ )
	{
	        if( !this->extend() )
	        {
	            return End_;
	        }
	}
	
	End_++;
	
	alloc->setEnd(End_.Index_);


	return (End_ - 1);
}

/*
bool DataSet::refreshIndexs(DataSet::iterator &it)
{
	for (unsigned int i = 0; i < Indexs_.size(); ++i)
	{
		SortOpt_ = SortOpts_[i];
	    
		index_iterator iit = Indexs_[i].upperBound(*it);
		
		if (Indexs_[i].size() > 0&& iit != Indexs_[i].begin())
		{
		    int j = 1;
			while ( *(iit - j) == *it) //如果有相等的记录判断是否是同一条记录
			{
				if ( (*(iit-j)).getAddress() == (*it).getAddress())
				{
					return false; //索引中已经有该记录了.
				}
				else
				{
					if ((iit - j) == Indexs_[i].begin()) //已经是第一条记录了.
					{
						break;
					}
					else //再上一条记录
					{
						j++;
					}
				}
							
			 }	//end while	
		}
		
    	Indexs_[i].insert(iit,*it);
    	
	 }
	
	return true;
}
   */   



bool DataSet::reserve(int n)
{
	
    if((RowSize_ < 1) ||
      (begin() + n < Border_ )) return true;
    
    MemoryObj obj = alloc->reserve(RowSize_, n, MemoryObj(Address_,MemorySize_));

    if( obj.Address == 0 )
        return false;
    else
    {
        Address_         = obj.Address;

        MemorySize_     = obj.Size;

        Border_        = DataSet::iterator(this, MemorySize_ / RowSize_);
        alloc->setBorder(Border_.Index_);
        return true;
    }
}

//建立索引
bool DataSet::buildIndex(const char *name,const char *FieldName,SORT_TYPE type)
{
	 for (unsigned int i = 0; i < SortOpts_.size(); ++i)
	 {
	 	if (SortOpts_[i].IndexName == name) //有相同的名称
	 		return false;
	 }
	 
     int Idx = this->getFieldIndex(FieldName);
     
     if (Idx >= 0)
     {
        SortOpt_.clear();
        SortOpt_.IndexName = name;
        bool Asc = (type == ASC ? true:false);

        SortOpt_.append(Idx,Asc);      //排序选项 
       
        SortOpts_.push_back(SortOpt_); //排序选项列表 
        
        Index <Row > idx;
        idx.reserve(size());
        for (iterator it = this->begin(); it != this->end(); ++it)
        {
                Row r =  (*it);
                idx.push_back(r);
        }

        idx.sort();

        Indexs_.push_back(idx);
        
        return true;
                   
     }
     return false;   
}


bool DataSet::buildIndex(const char *name,const vector<pair<string,SORT_TYPE> > &vp)
{
	for (unsigned int j = 0; j < SortOpts_.size(); ++j)
	{
	 	if (SortOpts_[j].IndexName == name) //有相同的名称
	 		return false;
	}
   	
   	
   	SortOpt_.clear();
    SortOpt_.IndexName = name;
    
    for(unsigned int i = 0; i < vp.size(); ++i)
	{
            int Idx = this->getFieldIndex(vp[i].first.c_str());
            bool asc = vp[i].second;
            if (Idx >= 0)
            {
               SortOpt_.append(Idx,asc);
            }
            else
            {
                return false;
            }   
    }

    
     SortOpts_.push_back(SortOpt_); //排序选项列表 
        
     Index <Row > idx;
     for (iterator it = this->begin(); it != this->end(); ++it)
     {
                Row r =  (*it);
                idx.push_back(r);
     }

     idx.sort();

     Indexs_.push_back(idx);

     return true;

}

bool DataSet::buildIndex(const char *name,unsigned int FieldCount,...) //建立联合索引 
{
    if( FieldCount <= 0 )  return false;
    
    for (unsigned int i = 0; i < SortOpts_.size(); ++i)
	{
	 	if (SortOpts_[i].IndexName == name) //有相同的名称
	 		return false;
	}
   
    va_list ap;
    va_start(ap, FieldCount);

    SortOpt_.clear();
    SortOpt_.IndexName = name;
    
    for(unsigned int i = 0; i < FieldCount; i++)
	{
            char * FieldName =  va_arg(ap, char*);
            int Idx = this->getFieldIndex(FieldName);
            int flag = va_arg(ap, int);
            bool asc = (flag == ASC ? true:false);
            if (Idx >= 0)
            {
               SortOpt_.append(Idx,asc);
            }
            else
            {
                va_end(ap);
                return false;

            }   
    }

    va_end(ap);
    
    
     SortOpts_.push_back(SortOpt_); //排序选项列表 
        
     Index <Row > idx;
     for (iterator it = this->begin(); it != this->end(); ++it)
     {
                Row r =  (*it);
                idx.push_back(r);
     }

     idx.sort();

     Indexs_.push_back(idx);

    
     return true;

} 

//销毁索引
bool DataSet::dropIndex(const char *name)
{
    unsigned int i = 0;
    for (; i < SortOpts_.size();++i)
    {
    	if (SortOpts_[i].IndexName == name)
    		break;	 
    }
    if (i < SortOpts_.size())
    {
    	vector<SortOption> ::iterator itOpt = SortOpts_.begin();//删除索引选项	 
    	itOpt += i;
    	SortOpts_.erase(itOpt);
	
	 	vector<Index<Row> > ::iterator it = Indexs_.begin(); //删除索引数据
    	it += i;
    	Indexs_[i].clear();
    	Indexs_.erase(it);
	}
	else
	{
		return false;
	}
	
    return true;
}


void DataSet::dropAllIndex()
{
	unsigned int i = 0;
    while (SortOpts_.size() > 0)
    {
        dropIndex(SortOpts_[i].IndexName.c_str());
        i++;
    }
}



bool DataSet::sort(unsigned int FieldCount,...)
{

    if( FieldCount <= 0 )  return false;

    SortOpt_.clear();

    va_list ap;
    va_start(ap, FieldCount);

    for(unsigned int i = 0; i < FieldCount; i++)
    {
            char * fldName =  va_arg(ap, char*);
            int Idx = this->getFieldIndex(fldName);
            if (Idx >= 0)
            {   
            	
                int flag = va_arg(ap, int);
                bool asc = (flag == ASC ? true :false);
                SortOpt_.append(Idx,asc);
               
            }
            else
            {
                va_end(ap);
                return false;
            }
    }
    va_end(ap);

    extern DataSet * sortDataSet;

    sortDataSet = this;

    qsort(Address_, size(), RowSize_,sortHelper);
}


/*
bool DataSet::filter(vector<Row> &rows ,unsigned int Indexno,...)
{
       va_list ap;
       va_start(ap, Indexno);
  
       char * address = new char[this->RowSize_ +1];
       
       memset(address,0x00,this->RowSize_ +1);
       
       if (Indexno >= Indexs_.size())  return false; //指定的索引号不存在 

       //根据排序的字段数处理接受参数 

       for (unsigned int i = 0; i < this->SortOpts_[Indexno].FieldCount; ++i)
       {
          unsigned int iIndex = this->SortOpts_[Indexno].FieldIndex[i]; //取得索引字段
          
          DataType type = Fields_[iIndex].getDataType(); //取得改字段的类型 
    
          unsigned int Offset = Fields_[iIndex].getOffset();
          if (type == FTINT)
          {
                 *((int*)(address + Offset)) = va_arg(ap,int) ;
          }
          else if (type == FTDOUBLE)
          {
                 *((double*)(address + Offset)) = va_arg(ap,double) ;
          }
          else if (type == FTSTRING)
          {
                 strcpy(address + Offset,va_arg(ap,char*));
          }
     }

     //构造排序的字段 
     Row row(this,(void*)address);

     //设置排序选项
     SortOpt_ = SortOpts_[Indexno];

     cout << "row[1] =" << row[1] <<endl;

     Index<Row>::iterator itbeg;
     Index<Row>::iterator itend;
     
     Indexs_[Indexno].locate(row,itbeg,itend);


     delete address;
     rows.clear();
     //没有找到记录 
     if (itbeg == Indexs_[Indexno].end())
        return false;
     else
     {
         Index<Row>::iterator it;
         for ( it = itbeg; it !=  itend; ++it)
         {
             rows.push_back(*it);
         }
     } 

     return true;

}
*/

/*bool DataSet::locate(DataSet::iterator &beg,
                        DataSet::iterator &end,
                         unsigned int FldCount,
                        ...)
{
       va_list ap;
       va_start(ap, FldCount);

       char * address = new char[this->RowSize_ +1];
       memset(address,0x00,this->RowSize_ +1);

       for (unsigned int i = 0; i < this->SortOpt_.FieldCount; ++i)
       {
                  unsigned int iIndex = SortOpt_.FieldIndex[i];

                  DataType type = Fields_[iIndex].getDataType();

                  if (type == FTINT)
                  {
                         *((int*)(address+Fields_[iIndex].getOffset())) = va_arg(ap,int) ;
                  }
                  else if (type == FTDOUBLE)
                  {
                         *((double*)(address+Fields_[iIndex].getOffset())) = va_arg(ap,double) ;
                  }
                  else if (type == FTSTRING)
                  {
                       //  cout <<"offset=" << Fields_[iIndex].getOffset() <<endl;
                         string  s = va_arg(ap,char*);
                         //cout <<"char *=" << s <<endl;
                         
                         strcpy(address+Fields_[iIndex].getOffset(),s.c_str());
                  }
     }

     Row row(this,(void*)address);

     //初始化
     beg   = End_;
     end   = End_;


    iterator it    =   lower_bound(Begin_,End_,row);
    if ((*it) != row || it == End_ )
    {
        delete address;
        return false;
    }
    else  //有对应的记录
    {
      beg   = it;
      end   = upper_bound(Begin_,End_,row);
    }
    


    delete address;

    return true;

}
*/
























/*
DataSet::iterator DataSet::find(int iFieldCount,...)
{
       va_list ap;
       va_start(ap, iFieldCount);
       char * address = new char[this->RowSize_ +1];
       memset(address,0x00,this->RowSize_ +1);

     for (int i = 0; i < this->SortOpt_.FieldCount; ++i)
     {
                  int iIndex = SortOpt_.FieldIndexs[i];
                  DATATYPE type = Fields_[iIndex].getDataType();

                  if (type == FTINT)
                  {
                         *(int*)(address+Fields_[iIndex].getOffset()) = va_arg(ap,int) ;
                  }
                  else if (type == FTDOUBLE)
                  {
                         *(double*)(address+Fields_[iIndex].getOffset()) = va_arg(ap,double) ;
                  }
                  else if (type == FTSTRING)
                  {     
                         strcpy(address+Fields_[iIndex].getOffset(),va_arg(ap,char*));
                  }
     }
                               
  Row row(this,(void*)address);

  iterator it =   std::lower_bound(Begin_,End_,row);

  if ((*it) == row)
  {
        delete[] address; //delete address 也可 为统一起见一律修改为  delete[] address
        return it;
  }
  else
  {
        delete[] address;   //delete address 也可 为统一起见一律修改为  delete[] address

        return End_;
  }


    int len = End_ - Begin_;
    int half;
    
    iterator first = Begin_;
    
    iterator middle;

    while (len > 0)
    {
       half = len / 2;
       middle = first + half;
       int iFlag = 0;  
       
       va_list ap;
       va_start(ap, iFieldCount);
       
       Data data;
	   for (int i = 0; i < SortOpt_.FieldCount; ++i)
	   {
	   		int iIndex = SortOpt_.FieldIndexs[i];
	   		
	   		DATATYPE type = Fields_[iIndex].getDataType();
	   		data.setDataType(type);
	   		if (type == FTINT)
	   			data.setAddress(&va_arg(ap,int));
	   		else if (type == FTDOUBLE)
	   			data.setAddress(&va_arg(ap,double));
	   		else if (type == FTSTRING)
	   			data.setAddress(va_arg(ap,char*));
	   			
	   		if ((*middle)[iIndex] < data)
	   		{
				iFlag = -1;
				break;   			
	   		} 
	   		else 
	   		{
	   			if (i  != SortOpt_.FieldCount -1) continue;
	   			break;
	   		}
	   
	   }
	   
	   if (iFlag < 0) 
	   {
	   		first = middle +1;
			len = len -half -1;
	   }
	   else 
	       len = half;
	   va_end(ap);

    }
    return first;
    
}


void DataSet::saveToFile(const char *szFileName,char cSeparator )
{
  std::fstream f(szFileName,ios_base::trunc|ios_base::in|ios_base::out);

  for (iterator it = this->begin();it != this->end(); ++it)
  {
        int i = 0;
        for (i = 0; i < this->field_count() -1; ++i)
        {
                f << (*it)[i] <<cSeparator;
        }
        f << (*it)[i];
        f << std::endl;
  }

  f.close();
}

void DataSet::loadFromFile(const char *szFileName,char cSeparator)
{
        std::fstream f(szFileName);
        this->clear();
        char *szBuf = new char [this->RowSize_ +1];

        memset(szBuf,0x00,this->RowSize_ + 1);
        std::string s;
        while(!f.eof())
        {
                f.getline(szBuf,this->RowSize_);
                iterator it = this->push_back();
                char *p = szBuf;
                while (*p != '\0')
                {


                        p++;
                }

                f.getl

        } 
}
*/


//指定索引号查询
bool DataSet::locate(index_iterator &begin,index_iterator &end,const char *name,const vector<Data> &vd)
{
	   unsigned int j = 0;
   	   for (; j < SortOpts_.size();++j)
   	   {
   	   		if (SortOpts_[j].IndexName == name) break;
   	   }
   	   if (j >= SortOpts_.size()) return false; //没有索引名称
   	   
   	   unsigned int iIndexno = j;
       
       SortOpt_ = SortOpts_[iIndexno];
		
	   if (this->SortOpt_.FieldCount != vd.size()) return false;
	   	

       char * address = new char[this->RowSize_ +1];
       memset(address,0x00,this->RowSize_ +1);
       
       for (unsigned int i = 0; i < this->SortOpt_.FieldCount; ++i)
       {
                  unsigned int iIndex = SortOpt_.FieldIndex[i];

                  DataType type = Fields_[iIndex].getDataType();

                  if (type == FTINT)
                  {
                         *((int*)(address+Fields_[iIndex].getOffset())) = (int)(vd[i]);
                  }
                  else if (type == FTDOUBLE)
                  {
                         *((double*)(address+Fields_[iIndex].getOffset())) = (double)(vd[i]) ;
                  }
                  else if (type == FTINT64)
                  {
                         *((int64_t*)(address+Fields_[iIndex].getOffset())) = (int64_t)(vd[i]) ;
                  }
                  else if (type == FTSTRING)
                  {
                         string  s = vd[i].c_str();
                         strncpy(address+Fields_[iIndex].getOffset(),s.c_str(),Fields_[iIndex].getLength() - 1);
                  }
     }

     Row row(this,(void*)address);
     
    
     bool ret = Indexs_[iIndexno].locate(row,begin,end);
    
	 delete [] address;
    
     return ret;    
		
}




bool DataSet::locate(index_iterator &begin,index_iterator &end,const char *name,...)
{
	
       unsigned int j = 0;
   	   for (; j < SortOpts_.size();++j)
   	   {
   	   		if (SortOpts_[j].IndexName == name) break;
   	   }
   	   if (j >= SortOpts_.size()) return false; //没有索引名称
   	   
   	   unsigned int iIndexno = j;
    
       va_list ap;
       va_start(ap, name);
       	
       
       SortOpt_ = SortOpts_[iIndexno];

       char * address = new char[this->RowSize_ +1];
       memset(address,0x00,this->RowSize_ +1);

       for (unsigned int i = 0; i < this->SortOpt_.FieldCount; ++i)
       {
                  unsigned int iIndex = SortOpt_.FieldIndex[i];

                  DataType type = Fields_[iIndex].getDataType();

                  if (type == FTINT)
                  {
                         *((int*)(address+Fields_[iIndex].getOffset())) = va_arg(ap,int) ;
                  }
                  else if (type == FTDOUBLE)
                  {
                         *((double*)(address+Fields_[iIndex].getOffset())) = va_arg(ap,double) ;
                  }
                  else if (type == FTINT64)
                  {
                         *((int64_t*)(address+Fields_[iIndex].getOffset())) = va_arg(ap,int64_t) ;
                  }
                  else if (type == FTSTRING)
                  {
                         string  s = va_arg(ap,char*);
                         //strcpy(address+Fields_[iIndex].getOffset(),s.c_str());
                         strncpy(address+Fields_[iIndex].getOffset(),s.c_str(),Fields_[iIndex].getLength() - 1);
                  }
     }

     Row row(this,(void*)address);

     bool ret = Indexs_[iIndexno].locate(row,begin,end);
    
	 delete [] address;
    
     return ret;
}


void DataSet::erase(DataSet::iterator pos)
{
    if( pos == End_ )  return;
    //将从pos + 1开始的内存都往前移动一个记录

    //删除索引中的记录
/*
    for (int i = 0; i < IndexList_.size();++i)
    {
           Row r =  *pos;
           vector<Row>::iterator it =  Indexs_[i].find(r);

           Indexs_[i].erase(it);
    }
*/


    void * src  = (char*)Address_ +  (pos - Begin_ + 1) * RowSize_;

    void * dest = (char*)Address_ +  (pos - Begin_ ) * RowSize_;
    
    size_t sz = (End_ - pos - 1) * RowSize_;
    
    memmove(dest, src, sz);
    
    End_--;
    alloc->setEnd(End_.Index_);

}



}





