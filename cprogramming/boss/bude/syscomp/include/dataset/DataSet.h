/*********************************************************************************
 * 描述： DataSet.h
 * 说明： 内存表DataSet (支持本地内存、共享内存、文件映射方式)
         主要功能:
          1)增，删，改，
          2)排序(单个字段，多个字段).
          3)建单个索引
          4)建复合索引
          5)指定索引查找
       
 * 环境： 基于标准C++,STL
 * 作者： ZHIJUN SONG (E-mail:songzj@lianchuang.com; MSN:ninyjun@hotmail.com)
 * 日期： created:2004-03-1
          history:
          	    2005-03-23        ZHIJUN SONG  增加了共享内存方式
          	    2005-03-24        ZHIJUN SONG  修改了class Field字段名称为char数组,且不能超过30，
          	    										使之可以放入共享内存头中。
          	    2005-03-24        ZHIJUN SONG  修正了UNIX上内存对齐算式
          	    2005-03-24        ZHIJUN SONG  增加了对共享内存边界超出的检查
          	    2005-03-24        ZHIJUN SONG  修改了程序退出不删除共享内存，下次启动时可以继续attach.
          	    2005-03-24        ZHIJUN SONG  重构了一些代码。
          	    2005-03-24        ZHIJUN SONG  BaseAlloc增加clone接口，构造DataSet时候传入分配器的引用，
          	                                   使用多态调用clone出需要的分配器。
          	    2005-03-24        ZHIJUN SONG  增加创建DataSet时默认Alloc(本地内存分配器，默认每次扩展4k)
          	
          	    2005-03-24        ZHIJUN SONG  再次attach共享内存的时候，增加校验码校验。
          	    2005-03-24        ZHIJUN SONG  增加了FTLONG数据类型         
          	    2005-03-25        ZHIJUN SONG  增加退出程序是否删除共享内存的控制。                          
          	    2005-04-09        ZHIJUN SONG  修改数据类型FTLONG为FTINT64        
          	    2005-04-12        ZHIJUN SONG  Data增加同类型的赋值操作.       
          	    2005-04-12        ZHIJUN SONG  Row 增加copy操作.    
          	    
          	    2005-04-18       ZHIJUN SONG   Field增加Tag属性,可以用来区分特别的数据类型
          	    
          	    2005-04-26        ZHIJUN SONG DataSet::iterator 增加了iterator operator =(index_iterator &iit)
          	    	                           方便了index_iterator到iterator的转换. 
          	    	                           
          	    2005-04-26        ZHIJUN SONG  增加指定索引查询
       										   bool locate(index_iterator &begin,index_iterator &end,const char *name,const std::vector<Data> &vd);
 	           	2005-04-26        ZHIJUN SONG  增加了建索引的方法
       			                     		   bool buildIndex(const char *name,const std::vector<std::pair<std::string,SORT_TYPE> > &vp); //建立索引 
   		        2005-04-27        ZHIJUN SONG iterator 增加了 Row operator ->() 方法
   		        
   		        2005-04-28        ZHIJUN SONG  Data 增加了Data & operator += (const Data &dta)
   		        										  Data & operator -= (const Data &dta)	
   		        										  Data & operator +  (const Data &dta)
          	    2005-05-10        ZHIJUN SONG  push_back得时候增加了保护
          	    
          	    2005-05-10        ZHIJUN SONG  修改指定索引名字locate的bug.
          	    2005-12-13        ZHIJUN SONG  Exception 增加了虚析构函数,修正了在gcc上的bug.
          	    
          	    
 * 备注： Version 0.96Beta 
 * 说明：             
                                分配器层次结构
        
                                 BaseAlloc
                                     |
                                     |
        -------------------------------------------------------
        |                            |                        |
       Alloc                      ShmAlloc                  FileAlloc
  (本地内存分配器)       (共享内存分配器基于System V)    (文件映射内存分配器)
                             
                              
                                   
     1) 使用本地内存的时候用Alloc或者默认(默认每次扩展内存为4k)，注入构造DataSet
      ！！！注意:使用本地内存的时候，内存不够时按内存分配器的构造参数增量扩展。
     example:
     	  Alloc all(4096); //按4k增量增加内存
		  DataSet ds(all);// or DataSet ds 默认方式
		  ds.addField(Field("field1",FTSTRING,20));
		  ds.addField(Field("field2",FTINT,16));
		  ds.addField(Field("field3",FTSTRING,10));
		  ds.addField(Field("field4",FTDOUBLE,12)); 
      
     2) 使用共享内存的时候用ShmAlloc，注入构造DataSet,
       ！！！注意：使用共享内存的方式，内存一次性分配,确保您分配的内存够使用
     example:
     	ShmAlloc  shmAlloc("/app/ebossdev/crm/src/acctmanm/src/tmp/testmm",4096*100);
  		if (!shmAlloc.allocate())
  		{
   			cout <<"share memory allocate fail!" <<endl;
  			 return -1;
  		}
	
		DataSet ds(shmAlloc);
	    ds.addField(Field("field1",FTSTRING,20));
	    ds.addField(Field("field2",FTINT,16));
	    ds.addField(Field("field3",FTSTRING,10));
	    ds.addField(Field("field4",FTDOUBLE,12));
   
     3) 使用文件映射内存分配方式FileAlloc (目前暂未实现) !!! 
      example:

 **********************************************************************************/
#ifndef __DATESET_H
#define __DATESET_H

#include <vector>
#include <iterator>

#include <string>
#include <iostream>
#include <stdlib.h>
#include <exception>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/errno.h>

//内存对齐字节
const int ALIGN_SIZE = sizeof(void*); 

//对齐
#define DOALIGN(x)   (((x-1)|(ALIGN_SIZE-1))+1)


#include "Index.h"


extern int errno;

namespace dataset {

enum DataType {FTINT=0,FTSTRING,FTDOUBLE,FTINT64};
//////////////////////

class Exception : public std::exception
{
public:
       Exception(const std::string & msg)
            	:message(msg)
       {
       };
       Exception(const char * msg):message(msg)
       {
       };

       virtual const char* what () const throw()
       {
       		return message.c_str();
       }

	   virtual ~Exception() throw()
	   {
	  
	   }
	
private:
   std::string message;
};



class DataSet;

///////////////////////Class Field////////////////////
class Field
{
	enum {FLDNAME_LEN=30};
public:

	Field(const char *Name,DataType dt,int Length,int Tag=0)
		:DataType_(dt),
        Length_(Length),
        Offset_(0),
        Tag_(Tag)
	{
		strncpy(FieldName,Name,FLDNAME_LEN);
		setLength(Length);
	}

	Field()
		:DataType_(FTINT),
		Length_(sizeof(int)),Offset_(0),Tag_(0)
    {
    	memset(FieldName,0x00,FLDNAME_LEN +1);

    }
	void setFieldName(const char * Name)
	{
		strncpy(this->FieldName,Name,FLDNAME_LEN);
	}

	const char * getFieldName() const
	{
		return FieldName;
	}

	void setDataType(DataType dt)
	{
		DataType_ = dt;
		if (DataType_ == FTINT)
			Length_ = sizeof(int);
		else if (DataType_ == FTDOUBLE)
			Length_ = sizeof(double);
		else if (DataType_ == FTINT64)
			Length_ = sizeof(int64_t);
		
	}

	DataType getDataType() const
	{
		return 	DataType_;
	}

	void setLength(int Len)
	{
		if (DataType_ == FTINT)
			Length_ = sizeof(int);
		else if (DataType_ == FTDOUBLE)
			Length_ = sizeof(double);
		else if (DataType_ == FTINT64)
			Length_ = sizeof(int64_t);
		else
		    Length_ = DOALIGN(Len+1); //加1的目的是为了肯定以'\0'结尾		    
	}

	int getLength() const
	{
		return Length_;
	}

	int getOffset() const
	{
		return Offset_;
	}


	void linkField(const Field &fld)
	{
		this->Offset_ = fld.Offset_ + fld.getLength();
	}

	
	char 	 FieldName[FLDNAME_LEN + 1];	
	
	
	void setTag(int i)
	{
		Tag_ = i;
	}
	
	int getTag() const
	{
		return Tag_;
	}
	
private:
	int       Offset_;
	
	DataType  DataType_;

	int       Length_;

	//域标志
	int			Tag_;

	friend class DataSet;
};


/////////////////class Data////////////////////////
class Row;
class Data
{
		friend class Row;

private:
        Data(Field * fld,void *Address)
                :Address_(Address),Field_(fld)
        {

        }
		
public:
		
		/*Data(const Data &dt)
        {
			Field_ = dt.Field_;
        	Address_ = dt.Address_;
        }
		
		Data()
		:Field_(0),Address_(0)
        {

        }
		*/
		
		DataType getDataType()
		{
			return Field_->getDataType();
		}
		
		
        operator double () const throw (Exception)
        {
                if (Field_->getDataType() == FTDOUBLE)
                      return  *(double*)Address_;
                else if (Field_->getDataType() == FTINT)
                {
                      double ret =   *(int*)Address_;
                      return ret;
                }
                else
                   throw Exception("not double or long data type.");
        }

        operator int () const throw (Exception)
        {
                if (Field_->getDataType() == FTINT)
                      return  *(int*)Address_;
                else if (Field_->getDataType() == FTDOUBLE)
                {
                      return (int)(*(double*)Address_);
                }
                else
                    throw Exception("not double or long data type.");

        }
        
        operator int64_t () const throw (Exception)
        {        
                if (Field_->getDataType() == FTINT64)
                {
                      int64_t p;
                     	memcpy(&p,Address_,Field_->getLength()); 
                      return p;
                      //return *(int64_t*)Address_;
                }
                else if (Field_->getDataType() == FTINT)
                      return  *(int*)Address_;
                else if (Field_->getDataType() == FTDOUBLE)
                {
                      return (long)(*(double*)Address_);
                }
                else
                    throw Exception("not double or long data type.");

        }
        


        const char * c_str() const throw (Exception)
        {
                if (Field_->getDataType() == FTSTRING)
                {
                        return  (char*)Address_;
                 }
                 else
                    throw Exception("not string data type.");

        }

        const void * data() const
        {
             return  (void*)Address_;

        }

        std::string  toString() const throw (Exception)
        {
                if (Field_->getDataType() == FTSTRING)
                        return  std::string((char*)Address_);
                else if (Field_->getDataType() == FTINT)
                {
                	char tmp[20] = "\0";
                        std::sprintf(tmp, "%d", *((int*)Address_));
                        return  std::string((char*)tmp);

                }
                else if (Field_->getDataType() == FTDOUBLE)
                {
                     	char tmp[40] = "\0";
                        std::sprintf(tmp, "%.4f", *((double*)Address_));
                        return  std::string((char*)tmp);
                }
                else if (Field_->getDataType() == FTINT64)
                {
                     	char tmp[40] = "\0";
                     	int64_t p;
                     	memcpy(&p,Address_,Field_->getLength()); 
                      std::sprintf(tmp, "%ld", p);  
                        //std::sprintf(tmp, "%ld", *((int64_t*)Address_));
                        return  std::string((char*)tmp);
                }
				else
				   	throw Exception("not string data type.");
        }


        Data & operator = (int l)
        {
                if (Field_->getDataType() == FTINT)
                        *(int*)Address_ = l;
                else if (Field_->getDataType() == FTINT64)
                	    *(int64_t*)Address_ = l;
                	
                return *this;
        }
        
        
        Data & operator + (const Data &dta)
        {
   			return (*this += dta);	    	
        }
        
        Data & operator += (const Data &dta)
        {
   			if (this->Field_->getDataType() == dta.Field_->getDataType())
   			{
   				if (Field_->getDataType() == FTINT)
                        *(int*)Address_ += (int)dta;
                else if (Field_->getDataType() == FTDOUBLE)
                		*(double*)Address_ += (double)dta;
                else if (Field_->getDataType() == FTINT64)
               			*(int64_t*)Address_ += (int64_t)dta;
                else if (Field_->getDataType() == FTSTRING)
                {
                	    std::string tmp=(char*)Address_;
                	    tmp += dta.c_str();
	                    strncpy((char*)Address_,tmp.c_str(),Field_->getLength()-1);
                }
   			}
   			return *this;	    	
        }
   		
   		Data & operator -= (const Data &dta)
        {
   			if (this->Field_->getDataType() == dta.Field_->getDataType())
   			{
   				if (Field_->getDataType() == FTINT)
                        *(int*)Address_ -= (int)dta;
                else if (Field_->getDataType() == FTDOUBLE)
                		*(double*)Address_ -= (double)dta;
                else if (Field_->getDataType() == FTINT64)
               			*(int64_t*)Address_ -= (int64_t)dta;
   			}
   			return *this;	    	
        }
   
        
        Data & operator = (const Data & dta)
        {
        		if (this == &dta) 
        			return *this;
        		
        		DataType dt = Field_->getDataType(); 
               
                if (dt == dta.Field_->getDataType()) //如果数据类型相同
                {         	
                	if ( dt == FTSTRING)
                	{
                		*this = dta.c_str();
                	}
                	else if ( dt == FTINT64)
                	{
                		*this = (int64_t)dta;
                	}
                	else if ( dt == FTINT)
                	{
                		*this = (int)dta;
                	}
                	else if ( dt == FTDOUBLE)
                	{
                		*this = (double)dta;
                	}
                }     
                return *this;
        }
        
        
        
        Data & operator = (int64_t l)
        {
                if (Field_->getDataType() == FTINT64){
                       // *(int64_t*)Address_ = l;
                        memcpy(Address_,&l,Field_->getLength());
                        
                }        
                else if (Field_->getDataType() == FTINT)
                        *(int*)Address_ = l;
                return *this;
        }
        

        Data & operator = (double d)
        {
                if (Field_->getDataType() == FTDOUBLE)
                        *(double*)Address_ = d;

                return *this;
        }
        Data & operator = (const char *s)
        {
                if (Field_->getDataType() == FTSTRING)
                {
                    strncpy((char*)Address_,s,Field_->getLength()-1);
                }
                return *this;
        }

        friend bool operator == (const Data &left,const Data &right)
	    {
    		if (left.Field_->getDataType() != right.Field_->getDataType()) return false;

    		if (left.Field_->getDataType()   == FTSTRING)
    			return strcmp((char*)left.Address_,(char*)right.Address_) == 0;
    		else if (left.Field_->getDataType()  == FTINT)
    			return *(int*)left.Address_  == *(int*)right.Address_;
    		else if (left.Field_->getDataType()  == FTINT64)
    			return *(int64_t*)left.Address_  == *(int64_t*)right.Address_;
    		else if (left.Field_->getDataType()  == FTDOUBLE)
    			return *(double*)left.Address_  == *(double*)right.Address_;
    		else
    			return false;
	    }

        friend bool operator < (const Data &Left,const Data &Right)
    	{
    		if (!(Left.Field_->getDataType() == Right.Field_->getDataType()))
    			return false;
    		if (Left.Field_->getDataType() == FTSTRING)
    			return strcmp((char*)Left.Address_,(char*)Right.Address_) < 0;
    		else if (Left.Field_->getDataType() == FTINT)
    			return *(int*)Left.Address_  < *(int*)Right.Address_;
    		else if (Left.Field_->getDataType() == FTINT64)
    			return *(int64_t*)Left.Address_  < *(int64_t*)Right.Address_;
    		else if (Left.Field_->getDataType() == FTDOUBLE)
    			return *(double*)Left.Address_  < *(double*)Right.Address_;
    		else
    			return false;
    	}

        friend std::ostream & operator << (std::ostream & os, const Data & right)
        {
           switch(right.Field_->getDataType() )
           {
            case FTSTRING:
                return os << (char*)right.c_str();
            case FTINT:
                return os << (int)right;
            case FTDOUBLE:
                return os << (double)right;
            case FTINT64:
            	return os << (int64_t)right;
           }
           return os;
        }
private:

        Field *  Field_;
        void *   Address_;
};


////////////////////////////////class Row///////////////////////
extern "C" int sortHelper( const void * left, const void * right);

class Row
{
public:
	Row(DataSet * dataSet,void *address)
		:DataSet_(dataSet),
		Address_(address)
	{

	}

	Row(const Row & row)
	{
		DataSet_  = row.DataSet_;

		Address_  = row.Address_;
	}

   void * getAddress()
   {
   	return Address_;
   }

	DataSet * getDataSet()
	{
		return DataSet_;
	}

	Data operator [](int index) const;

	Data fieldByName(const char * fldname) const;

	bool operator == (const Row &right);

	bool operator !=(const Row &right)
   {
   	return  (!this->operator ==(right));
   }

	Row & copy (const Row & right);

	bool operator < (const Row & right ) const ;



	friend int sortHelper( const void * left, const void * right);

private:

	void    *     Address_;

	DataSet *     DataSet_;

};


///////////////////  class MemoryObj ///////////////////
class MemoryObj
{
public:
    void * Address;

    unsigned int    Size;

    MemoryObj( void * Address = 0, int Sz = 0)
    	:Address(Address), Size(Sz)
    {

    };

};

////////////////////////////分配器基类//////////////
class BaseAlloc
{
public:
	BaseAlloc()
    {
    }
	virtual ~BaseAlloc()
    {
    }

    virtual void free(void * Address) = 0;

    virtual MemoryObj extend(const MemoryObj & mo) = 0;
    
    virtual MemoryObj extend(const MemoryObj & mo,size_t add) = 0;

    virtual MemoryObj reserve(unsigned int RowSize,unsigned int n,const MemoryObj & mo) = 0;
    
    //克龙
    virtual BaseAlloc * clone() = 0;

    virtual void setBegin(unsigned int beg)
    {
    	
    }
    virtual bool setEnd(unsigned int end)
    {
    	
    	return true;
    }
    virtual void setBorder(unsigned int border)
    {
    	
    }

    virtual void addField(const Field &fld)
    {
    }
    
    virtual void getFields(std::vector<Field> &vec)
    {
    }
    virtual int getBegin()
    {
       return 0;
    }
    virtual int getEnd()
    {
    	return 0;
    }
    virtual int getBorder()
    {
    	return 0;
    }

    virtual void * getAddress()
    {
    	return NULL;
    }
    
    virtual bool isAttached()
    {
    	return false;
    }
    virtual bool isAllocated()
    {
    	return false;
    }
protected:
  

};



/////////////////////// 本地内存分配器///////////////////////
class Alloc :public BaseAlloc
{

public:
	Alloc(unsigned int chunk):BaseAlloc()
    	,chunk_(chunk)
    {
    }
	
	Alloc():BaseAlloc()
    	,chunk_(4096)
    {
    }

    virtual void free(void * Address)
    {
       	::free(Address);
    }

    MemoryObj extend(const MemoryObj & mo)
    {
        void * Address = ::realloc(mo.Address, mo.Size + chunk_);
        memset((char*)Address + mo.Size ,0x00,chunk_);
        return MemoryObj(Address, mo.Size  + chunk_);

    }
    
    MemoryObj extend(const MemoryObj & mo,size_t add)
    {

        void * Address = ::realloc(mo.Address, mo.Size + add);

        //初始化内存.
        memset((char*)Address + mo.Size ,0x00,add);

        return MemoryObj(Address, mo.Size  + add);
    }


    MemoryObj reserve(unsigned int RowSize,unsigned int n,const MemoryObj & mo)
    {
        void *Address =0;
        if( mo.Address != 0 )
        {
            Address = ::realloc(mo.Address, RowSize * n);

            Address = memset((char*)mo.Address + mo.Size,0x00,RowSize * n);
        }
        else
        {
            Address = ::malloc(RowSize * n);
            Address =  memset(Address,0x00,RowSize * n);
        }

        return MemoryObj(Address, RowSize * n);
    }
    
     virtual BaseAlloc * clone()
     {
     		Alloc *p = new Alloc();
     		
     		(*p) = *this;
     		
     		return p;
     }
    
private:
     unsigned int chunk_;

};
///////////////////  共享内存分配器 /////////////////////
class ShmAlloc :public BaseAlloc
{

public:
	ShmAlloc(const char * key,unsigned int size,bool destroyed = false):BaseAlloc()
    	,key_(key),allocated_(false),attached_(false),
        head_(0),originalAddress_(0),size_(size),address_(0),destroyed_(destroyed)
    {
    	
    }
    
    ShmAlloc():BaseAlloc()
    	,key_(""),allocated_(false),attached_(false),
        head_(0),originalAddress_(0),size_(0),address_(0)
    {
    	
    }

    virtual ~ShmAlloc()
    {
    	
    }

    virtual void free(void * Address)
    {
    	  if (originalAddress_ != 0)
    	  {
    	  	 shmdt((char*)originalAddress_);
          }
    }

    virtual MemoryObj extend(const MemoryObj & mo)
    {
           if (init() != 0) return NULL;
           
           return MemoryObj(address_, head_->totalSize);
    }
    
    virtual MemoryObj extend(const MemoryObj & mo,size_t add)
    {
        	return extend(mo);
    }


    virtual MemoryObj reserve(unsigned int RowSize,unsigned int n,const MemoryObj & mo)
    {
            return extend(mo);
    }

    //设置共享内存头信息
    virtual void setBegin(unsigned int beg)
    {
    	if (allocated_|| attached_)
        {
        	head_->begin = beg;
        }
    };
    virtual bool setEnd(unsigned int end)
    {
    	if (allocated_ || attached_)
        {
         
    		head_->leftSize -= (end - head_->end) * head_->rowSize;
    		head_->end = end;
        	//std::cout <<"*************leftsize = " << head_->leftSize <<std::endl;
        	return true;
        }
        return false;
    };

    virtual void setBorder(unsigned int border)
    {
       	if (allocated_|| attached_)
        {
        	head_->border = border;
        }
    }
    


	virtual void addField(const Field &fld)
    {
    	if (allocated_ && !attached_) //已经attached不能增加字段
        {
            void *tmpPtr =  address_;
         
        	new(tmpPtr)Field();

        	*((Field *)tmpPtr) = fld;      

        	//设置数据地址
        	address_= (char*)address_ + sizeof(Field);
        	
        	head_->fieldCount++;

            head_->leftSize -= sizeof(Field); //共享内存实际空闲
            
            head_->totalSize = head_->leftSize;
           // std::cout <<"totalSize=" <<  head_->totalSize <<std::endl;
               
            head_->rowSize += fld.getLength();
        }
    }

	virtual void getFields(std::vector<Field>& vec)
    {
    	if (head_ !=0)
    	{
    		Field *tmpPtr = (Field*)((char*)address_ - sizeof(Field) * head_->fieldCount);
    		for (unsigned int i = 0; i < head_->fieldCount;++i)
    		{
    			Field fld = *(tmpPtr++);
    			
    			vec.push_back(fld);
    			
    		}
    	
    	}
    	
    }
	
	virtual bool isAttached()
    {
    	return attached_;
    }

	virtual bool isAllocated()
    {
    	return allocated_;
    }


    bool allocate()  //分配共享内存
    {
      return (init() == 0);
    }

    bool attach()
    {
         if (allocated_) return false;
    	
    	 key_t kt = ftok(key_.c_str(),'0');
         if ((key_t)-1 == kt ) return false;


         int shmId = shmget(kt, 0, (IPC_CREAT));
      	 if (-1 == shmId) return false;

         originalAddress_ = shmat(shmId, 0, 0);
        
          
         if ( *(int*)originalAddress_ == -1) return false;
       	 
       	 address_ = originalAddress_;
         
         void * tmpPrt=originalAddress_;

         head_ = (ShmHead*)tmpPrt; //构造共享内存头
         
         //校验码
         if (!(head_->key == kt && head_->verifyCode == 123456789)) return false;
         
         if (head_->fieldCount < 1 || head_->end < 1) return false;
              
         //取得数据开始地址
         address_ = (char*)address_ + sizeof(ShmHead) + sizeof(Field) * head_->fieldCount;
         attached_ = true;
         return true;
    }
    
    virtual void * getAddress()
    {
    	
    	return address_;
    }
    
    virtual int getBegin()
    {
       return head_->begin;
    }
    virtual int getEnd()
    {
    	return head_->end;
    }
    virtual int getBorder()
    {
    	return head_->border;
    }
    
    virtual BaseAlloc * clone()
    {
    	ShmAlloc *p = new ShmAlloc();
     		
     	(*p) = *this;
     		
     	 return p;
    }
    
private:
	class ShmHead
    {
    public:
    	ShmHead()
        	:begin(0),end(0),border(0),leftSize(0),fieldCount(0),
        		rowSize(0),totalSize(0),key(0),verifyCode(123456789)
        {
        };
        ~ShmHead()
        {
        };
		unsigned int begin; //开始
        unsigned int end;   //结束
        unsigned int border; //边界
        unsigned int leftSize; //当前剩余数据内存
        unsigned int totalSize;//总数据共享内存大小
        unsigned int fieldCount;//字段个数
        unsigned int rowSize;//每行的长度
        key_t	 key;
        unsigned int verifyCode;//校验码
    };


	int init()   //分配共享内存
    {

      	if (allocated_ || attached_) return 0;
        key_t kt = ftok(key_.c_str(),'0');
        if ((key_t)-1 == kt ) return -1;


        int shmId = shmget(kt, size_ + sizeof(ShmHead), (IPC_CREAT | 07777));
      	if (-1 == shmId)
      	{
      		if (errno == EEXIST) //试图attach
      		{
      			if (attach())
      			{
      				return 0;
      			}
      		}
      		return -1;
      	} 

        originalAddress_ = shmat(shmId, 0, 0);
       
        if ( *(int*)originalAddress_ == -1) return -1;
       
        address_ = originalAddress_;
        
        void * tmpPtr = originalAddress_;
        new(tmpPtr) ShmHead();

        head_ = (ShmHead*)tmpPtr; //构造共享内存头
        
        if (destroyed_)
        	shmctl(shmId, IPC_RMID, 0);
       
		
        head_->leftSize = size_; //大小
        head_->totalSize = head_->leftSize;
        head_->key = kt;
    
        address_ = (char*)address_ + sizeof(ShmHead);
        address_ = memset(address_,0x00,head_->leftSize);
        allocated_ = true;
        return 0;
    }
    
    std::string key_; //共享内存的路径

    bool        allocated_;  //是否已经分配

    bool        attached_;  //是否已经连接到共享内存
    
    int         size_; 		//共享内存原始分配的大小

    ShmHead		*head_;		//共享内存头
 
    void        * originalAddress_;//原始指针
    
    void        * address_;//数据段地址
    
    bool		destroyed_;//退出程序是否销毁共享内存
};


/////////////////// 基于文件映射的分配器 /////////////////////
/*class FileAlloc :public BaseAlloc
{
public:
	
private:	
	
};
*/


///////////////////////////DataSet///////////////////////////////
enum SORT_TYPE{ASC=1,DESC};
class DataSet
{
public:
		DataSet(BaseAlloc &alc);
		
		DataSet(); 
	   

		~DataSet();


		typedef Index<Row>::iterator index_iterator;

        //类似于标准库中的迭代器 
        class iterator : public std::iterator<std::random_access_iterator_tag, Row>
        {
        	public:
        		iterator(DataSet *dst = 0,int iIndex = 0)
        			:DataSet_(dst),Index_(iIndex)
        		{
        
        		}
        
        		iterator operator =(index_iterator &iit)
        		{
        			DataSet *pds = (*iit).getDataSet();
        			return iterator(pds,((char*)((*iit).getAddress()) - pds->getAddress())/pds->getRowSize());
        		}
        		
        		
        		iterator &operator ++()
        		{
        			++Index_;
        			return *this;
        		}
        
        		iterator &operator ++ (int)
        		{
        			Index_++;
        			return *this;
        		}
        		iterator operator + (int n)
        		{
        			return iterator(DataSet_,Index_ + n);
        		}
        
        		iterator &operator += (int n)
        		{
        			Index_ += n;
                 		 return * this;
        		}
        
        		iterator &operator --()
        		{
        			--Index_;
        
        			return *this;
        		}
        
        		iterator &operator --(int)
        		{
        			Index_--;
        			assert((Index_ >= 0));
        			return *this;
        		}
        
        		iterator operator -(int n)
        		{
        			assert(((Index_ - n) >= 0));
        			return iterator(DataSet_,Index_ - n);
        		}
        
        		iterator &operator -= (int n)
        		{
        			Index_ -= n;
        			assert((Index_ >= 0));
        			return * this;
        		}
        
        		int operator - (const iterator &it)
        		{
        			assert(((Index_ - it.Index_) >=0));
        			return Index_ - it.Index_;
        		}
        	
        		Row operator * ()
        		{
                  	return Row(DataSet_,
                  		(char*)DataSet_->Address_ + Index_ * DataSet_->RowSize_);
                }

        
        		bool operator == (const iterator & it)
        		{
                  	return (DataSet_ == it.DataSet_) && (Index_ == it.Index_);
        		}
        
        		bool operator < (const iterator & it)
        		{
                  	return Index_ < it.Index_;
             	}
        
        		bool operator != (const iterator & it)
        		{
                  	return (DataSet_ != it.DataSet_) || (Index_ != it.Index_);
        		}
        
        		bool operator > (const iterator & it)
        		{
                  	return Index_ > it.Index_;
        		}
        
        		bool operator <= (const iterator &it)
        		{
              		return Index_ <= it.Index_;
        		}

        private:
        		DataSet    * DataSet_;
        
        		int          Index_;
        
        		friend class DataSet;
        };



        iterator  pushBack();

        void popBack()
        {
               if(!empty()) End_--;
        }

        void erase(iterator pos);



        int fieldCount()
        {
                return Fields_.size();
        }

        Field & getField(unsigned int Index)
        {
                if (Index <= Fields_.size() -1) return Fields_[Index];
        }

        bool addField(Field fld);

		
		bool createDataSetAs(const DataSet &dst,bool copy = false);
		

        int getFieldIndex(const char * fldName) const 
        {
                int i;
                for (i = 0; i < Fields_.size(); ++i)
                {
                        if (strcmp(Fields_[i].FieldName,fldName) == 0) break;
                }

                return (i == Fields_.size() ?  -1 : i);
        }


		int rowCount()
		{
			return End_ - Begin_;
		}

		int capacity()
		{
       		return Border_ - Begin_;
		}

		int size()
		{
			return End_ - Begin_;
		}

		iterator  begin()
		{
            return  Begin_;
		}

		void clear();

		void clearField();

		iterator  end()
		{
            return End_;
		}

		bool empty()
		{
            return Begin_ == End_;
		}

		char * getAddress()
		{
            return (char*)Address_;
		}


		bool reserve(int n);

       
       //刷新索引当索引存在的时候,在新增加完数据后可以调用此方法来维护索引.
       //此方法效率较低.
      //  bool refreshIndexs(DataSet::iterator &it); 
        	
       
		bool buildIndex(const char *name,const char *FieldName,SORT_TYPE type); // 建索引
   		bool buildIndex(const char *name,unsigned int FieldCount,...); //建立索引 
   		bool buildIndex(const char *name,const std::vector<std::pair<std::string,SORT_TYPE> > &vp); //建立索引 
   		

       //指定索引号查询
        bool locate(index_iterator &begin,index_iterator &end,const char *name,...);

		//指定索引号查询
        bool locate(index_iterator &begin,index_iterator &end,const char *name,const std::vector<Data> &vd);


        //删除索引 
        bool dropIndex(const char *name);
        void dropAllIndex();

        //过滤
		//bool filter(std::vector<Row> &rows,unsigned int Indexno,...);


        index_iterator	getIndexBegin(unsigned index = 0)
        {
        	return Indexs_[index].begin();
        }	
        
        index_i   
		bool buildIndex(const char *name,const char *FieldName,SORT_TYPE type); // 寤虹储寮