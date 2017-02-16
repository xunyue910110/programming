/**
 *
 * <p>Title:    CVar类，该类对各种数据的兼容性能有待完善</p>
 * <p>Description: 通用变量类，可以存储多种数据格式</p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: Linkage</p>
 * @author wangxr
 * @version 1.0
 */

/////////////////////////////////////////////////////////////////////////
// CVar类，该类对各种数据的兼容性能有待完善
// 作用：通用变量类，可以存储多种数据格式
// 注意：   当var被当成某些类型进行赋值的时候，自动转换为该类型
//          当进行比较的时候，类型不同返回false
//          当调用c_str()的时候，如果本来不是字符型，会自动转换为STR类型
//          当对var进行运算时，类型不会转换 比如str要加int时候，则表示连接字符串
/////////////////////////////////////////////////////////////////////////

#ifndef CVAR_HEADER_INCLUDED
#define CVAR_HEADER_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "system.h"
using std::string;
using std::ostream;
#define DEFAULT_DATA_SIZE   4

//数据类型定义
enum VAR_TYPE
{
    VAR_TYPE_INT,
    VAR_TYPE_STR,
    VAR_TYPE_DOUBLE,
    VAR_TYPE_RAW
};

class CVar
{

private:

    VAR_TYPE mType;                                 //数据类型

    int mSize;                                      //数据长度

    char * mData;                                   //数据内容存放指针

public:

    /**
     * 函数名称：VAR_TYPE type()
     * 函数功能：返回该变量的数据类型
     * 使用注意：无
     * @param
     * @return
     * @throws
     */
    VAR_TYPE type() const {
        return mType;
    };

    /**
     * 函数名称：int size() const
     * 函数功能：返回该变量的数据长度
     * 使用注意：无
     * @param
     * @return
     * @throws
     */
    int size() const{
        return mSize;
    };

    /**
     * 函数名称：void clear()
     * 函数功能：清空该变量的里的数据，把它所占用的内存都清零
     * 使用注意：无
     * @param
     * @return
     * @throws
     */
    void clear(){
        memset(mData, 0x00, mSize);
    };

    /**
     * 函数名称：void resize(int iSize)
     * 函数功能：重新调整数据的长度，如果长度变小，原有的数据会被截取
     * 使用注意：无
     * @param   int iSize       新的数据长度
     * @return
     * @throws
     */
    void resize(int iSize){
        char * bak_ptr = mData;
        mData = new char[iSize];

        if( bak_ptr != NULL ){
            memcpy(mData, bak_ptr, (mSize > iSize) ? iSize : mSize);
            delete[] bak_ptr;
        }
        mSize = iSize;
    };

    /**
     * 函数名称：CVar & operator = (const CVar & right)
     * 函数功能：赋值函数重载
     * 使用注意：无
     * @param   right           右操作数
     * @return  返回本身
     * @throws
     */
    CVar & operator = (const CVar & right){
        if( mSize < right.mSize ) resize(right.mSize);
        memcpy(mData, right.mData, right.mSize);
        mType = right.mType;
        return * this;
    };

    /**
     * 函数名称：CVar & operator = (int right)
     * 函数功能：用整型值赋值
     * 使用注意：无
     * @param   right           右操作数
     * @return  返回本身
     * @throws
     */
    CVar & operator = (int right){
        if( mSize < sizeof(int) ) resize(sizeof(int));
        memcpy(mData, &right, sizeof(int));
        mType = VAR_TYPE_INT;
        return * this;
    };

    /**
     * 函数名称：CVar & operator = (double right)
     * 函数功能：用double型数据赋值
     * 使用注意：无
     * @param   right           右操作数
     * @return  返回本身
     * @throws
     */
    CVar & operator = (double right){
        if( mSize < sizeof(double) ) resize(sizeof(double));
        memcpy(mData, &right, sizeof(double));
        mType = VAR_TYPE_DOUBLE;
        return * this;
    };

    /**
     * 函数名称：CVar & operator = (const char * right)
     * 函数功能：用字符串型数据赋值
     * 使用注意：无
     * @param   right           右操作数
     * @return  返回本身
     * @throws
     */
    CVar & operator = (const char * right){
        static int len;
        len = strlen(right)+1;
        if( mSize < len ) resize(len);
        strcpy(mData, right);
        mType = VAR_TYPE_STR;
        return * this;
    };

    /**
     * 函数名称：CVar & operator = (const string & right)
     * 函数功能：用string型数据赋值
     * 使用注意：无
     * @param   right           右操作数
     * @return  返回本身
     * @throws
     */
    CVar & operator =(const std::string & right){
    //CVar & operator =(string & right){
        static int string_len;
        string_len = right.length()+1;
        if( mSize < string_len ) resize(string_len);
        strcpy(mData, right.c_str());
        mType = VAR_TYPE_STR;
        return * this;
    };

    /**
     * 函数名称：operator int()
     * 函数功能：返回int型数据，如果原先的数据类型是字符串，则把字符串转换为数字输出
     * 使用注意：本身的数据类型不变
     * @param
     * @return  int
     * @throws
     */
    operator int(){
        switch(mType){
            case VAR_TYPE_INT:{
                return *(int *) mData;
            }
            case VAR_TYPE_STR:{
                return atoi(mData);
            }
            case VAR_TYPE_DOUBLE:{
                return int(*((double*)mData));
            }
        }
        return 0;
    };

    /**
     * 函数名称：operator double()
     * 函数功能：返回double型数据，如果原先的数据类型是字符串，则把字符串转换为数字输出
     * 使用注意：本身的数据类型不变
     * @param
     * @return  double
     * @throws
     */
    operator double(){
        switch(mType){
            case VAR_TYPE_DOUBLE:{
                return *(double *) mData;
            }
            case VAR_TYPE_INT:{
                return double(*(int *) mData);
            }
            case VAR_TYPE_STR:{
                return atof(mData);
            }
        }
        return 0;
    };

    /**
     * 函数名称：string str() const
     * 函数功能：返回字符串
     * 使用注意：本身的数据类型不变
     * @param
     * @return  string
     * @throws
     */
    string str() const {

        switch(mType){
            case VAR_TYPE_STR:{
                return string(mData);
            }
            case VAR_TYPE_INT:{
                char tmp[16];
                sprintf(tmp,"%d", *(int*)mData);
                return string(tmp);
            }
            case VAR_TYPE_DOUBLE:{
                char tmp[16];
                sprintf(tmp,"%f", *(double*)mData);
                return string(tmp);
            }
        }
        return string("");
    };

    /**
     * 函数名称：const char * c_str()
     * 函数功能：返回字符串型数据
     * 使用注意：如果本来不是字符串型数据，则自动转换为字符型
     * @param
     * @return  const char *
     * @throws
     */
    const char * c_str() {
        switch(mType){
            case VAR_TYPE_STR:{
                return mData;
            }
            case VAR_TYPE_INT:{
                char tmp[16];
                sprintf(tmp,"%d", *(int*)mData);
                int len = strlen(tmp) + 1;
                if( mSize < len ) this->resize(len);
                strcpy(mData, tmp);
                mType = VAR_TYPE_STR;
                return mData;
            }
            case VAR_TYPE_DOUBLE:{
                char tmp[16];
                sprintf(tmp,"%f", *(double*)mData);
                int len = strlen(tmp) + 1;
                if( mSize < len ) this->resize(len);
                strcpy(mData, tmp);
                mType = VAR_TYPE_DOUBLE;
                return mData;
            }
            default:
                return NULL;
        }
    };

    /**
     * 函数名称：const void * ptr() const
     * 函数功能：返回数据指针
     * 使用注意：无
     * @param
     * @return  const void *
     * @throws
     */
    const void * ptr() const {
        return (void*) mData;
    };

    /**
     * 函数名称： bool operator == (const CVar & var)
     * 函数功能：==函数重载
     * 使用注意：必须保证两者的数据类型相同，如果不同的话，直接返回false
     * @param   var         右值
     * @return  bool        是否相等
     * @throws
     */
    bool operator == (const CVar & var) const {
        if( mType != var.mType )
            return false;
        switch(mType){
            case VAR_TYPE_INT:{
                return *(int * )mData == *((int *)(var.mData));
            }
            case VAR_TYPE_STR:{
                return strcmp((char *)mData, (char *)(var.mData)) == 0;
            }
            case VAR_TYPE_DOUBLE:{
                return *(double * )mData == *((double *)(var.mData));
            }
            case VAR_TYPE_RAW:{
                return 0 == memcmp(mData, var.mData, mSize);
            }
        }
        return false;
    };

    bool operator == (const CVar & var) {
        if( mType != var.mType )
            return false;
        switch(mType){
            case VAR_TYPE_INT:{
                return *(int * )mData == *((int *)(var.mData));
            }
            case VAR_TYPE_STR:{
                return strcmp((char *)mData, (char *)(var.mData)) == 0;
            }
            case VAR_TYPE_DOUBLE:{
                return *(double * )mData == *((double *)(var.mData));
            }
            case VAR_TYPE_RAW:{
                return 0 == memcmp(mData, var.mData, mSize);
            }
        }
        return false;
    };

    /**
     * 函数名称：bool operator > (const CVar & right)
     * 函数功能：>函数重载
     * 使用注意：必须保证两者的数据类型一致
     * @param   right       右值
     * @return  bool        是否大于right
     * @throws
     */
    bool operator > (const CVar & var){
        if( mType != var.mType )
            return false;
        switch(mType){
            case VAR_TYPE_INT:{
                return *(int * )mData > *((int *)(var.mData));
            }
            case VAR_TYPE_STR:{
                return strcmp((char *)mData, (char *)(var.mData)) > 0;
            }
            case VAR_TYPE_DOUBLE:{
                return *(double * )mData > *((double *)(var.mData));
            }
            case VAR_TYPE_RAW:{
                return memcmp(mData, var.mData, mSize) > 0;
            }
        }
        return false;
    };

    /**
     * 函数名称：bool operator >= (const CVar & var)
     * 函数功能： >=函数重载
     * 使用注意：必须保证两者的数据类型一致
     * @param   right       右值
     * @return  bool        是否大于等于right
     * @throws
     */
    bool operator >= (const CVar & var){
        if( mType != var.mType )
            return false;
        switch(mType){
            case VAR_TYPE_INT:{
                return *(int * )mData >= *((int *)(var.mData));
            }
            case VAR_TYPE_STR:{
                return strcmp((char *)mData, (char *)(var.mData)) >= 0;
            }
            case VAR_TYPE_DOUBLE:{
                return *(double * )mData >= *((double *)(var.mData));
            }
            case VAR_TYPE_RAW:{
                return memcmp(mData, var.mData, mSize) >= 0;
            }
        }
        return false;
    };

    /**
     * 函数名称：bool operator <= (const CVar & var)
     * 函数功能：<=函数重载
     * 使用注意：必须保证两者的数据类型一致
     * @param   right       右值
     * @return  bool        是否小于等于right
     * @throws
     */
    bool operator <= (const CVar & var){
        if( mType != var.mType )
            return false;
        switch(mType){
            case VAR_TYPE_INT:{
                return *(int * )mData <= *((int *)(var.mData));
            }
            case VAR_TYPE_STR:{
                return strcmp((char *)mData, (char *)(var.mData)) <= 0;
            }
            case VAR_TYPE_DOUBLE:{
                return *(double * )mData <= *((double *)(var.mData));
            }
            case VAR_TYPE_RAW:{
                return memcmp(mData, var.mData, mSize) <=0;
            }
        }
        return false;
    };

    /**
     * 函数名称： bool operator == (int right)
     * 函数功能：==函数重载
     * 使用注意：必须保证var本身的存储的数据类型为int型数据
     * @param   right       右值
     * @return  bool        是否相等
     * @throws
     */
    bool operator == (int right) const {
        if( mType != VAR_TYPE_INT )
            return false;
        return (*(int *)mData == right);
    };

    /**
     * 函数名称： bool operator == (const char * right)
     * 函数功能：==函数重载
     * 使用注意：必须保证var本身的存储的数据类型为char *型数据
     * @param   right       右值
     * @return  bool        是否相等
     * @throws
     */
    bool operator == (const char * right) const {
        if( mType != VAR_TYPE_STR )
            return false;
        return (strcmp((char *)mData, right) == 0);
    };

    /**
     * 函数名称： bool operator == (double right)
     * 函数功能：==函数重载
     * 使用注意：必须保证var本身的存储的数据类型为double型数据
     * @param   right       右值
     * @return  bool        是否相等
     * @throws
     */
    bool operator == (double right) const {
        if( mType != VAR_TYPE_DOUBLE )
            return false;
        return (*(double *)mData == right);
    };

    /**
     * 函数名称： bool operator < (const CVar & var)
     * 函数功能：<函数重载
     * 使用注意：必须保证两者存储的数据类型相同
     * @param   var         右值
     * @return  bool        是否小于var
     * @throws
     */
    bool operator < (const CVar & var) const {
        if( mType != var.mType )
            return false;
        switch(mType){
            case VAR_TYPE_INT:{
                return *(int * )mData < *((int *)(var.mData));
            }
            case VAR_TYPE_STR:{
                return strcmp((char *)mData, (char *)(var.mData)) < 0;
            }
            case VAR_TYPE_DOUBLE:{
                return *(double * )mData < *((double *)(var.mData));
            }
            case VAR_TYPE_RAW:{
                return memcmp(mData, var.mData, mSize) < 0;
            }
        }
        return false;
    };

    /**
     * 函数名称： bool operator < (int right)
     * 函数功能：<函数重载
     * 使用注意：必须保证左值存储的数据类型为int
     * @param   right       右值
     * @return  bool        是否小于right
     * @throws
     */
    bool operator < (int right) const {
        if( mType != VAR_TYPE_INT )
            return false;
        return (*(int *)mData < right);
    };

    /**
     * 函数名称：bool operator < (double right)
     * 函数功能：<函数重载
     * 使用注意：必须保证左值存储的数据类型为double
     * @param   right       右值
     * @return  bool        是否小于right
     * @throws
     */
    bool operator < (double right) const {
        if( mType != VAR_TYPE_DOUBLE )
            return false;
        return (*(double *)mData < right);
    };

    /**
     * 函数名称：bool operator < (const char * right)
     * 函数功能：<函数重载
     * 使用注意：必须保证左值存储的数据类型为char *
     * @param   right       右值
     * @return  bool        是否小于right
     * @throws
     */
    bool operator < (const char * right) const {
        if( mType != VAR_TYPE_STR )
            return false;
        return (strcmp((char *)mData, right) < 0);
    };

    /**
     * 函数名称：bool operator > (int right)
     * 函数功能：>函数重载
     * 使用注意：必须保证左值的数据类型也是int型
     * @param   right       右值
     * @return  bool        是否大于right
     * @throws
     */
    bool operator > (int right){
        if( mType != VAR_TYPE_INT )
            return false;
        return *(int*)mData > right;
    };

    /**
     * 函数名称：bool operator > (double right)
     * 函数功能：>函数重载
     * 使用注意：必须保证左值的数据类型也是double型
     * @param   right       右值
     * @return  bool        是否大于right
     * @throws
     */
    bool operator > (double right){
        if( mType != VAR_TYPE_DOUBLE )
            return false;
        return *(double*)mData > right;
    };

    /**
     * 函数名称：bool operator > (const char * right)
     * 函数功能：>函数重载
     * 使用注意：必须保证左值的数据类型也是char *型
     * @param   right       右值
     * @return  bool        是否大于right
     * @throws
     */
    bool operator > (const char * right){
        return !( *this < right || *this == right );
    };

    /**
     * 函数名称：const CVar & operator += (int right)
     * 函数功能：+=函数重载
     * 使用注意：如果左值是int型，则按照int相加;
     *           如果左值是char *型的，则把right转换为字符串，追加上去
     *           本身数据类型不变
     * @param   right
     * @return  返回本身
     * @throws
     */
    const CVar & operator += (int right){
        switch(mType){
            case VAR_TYPE_INT:{
                *(int *)(mData) += right;
                return *this;
            }
            case VAR_TYPE_STR:{
                char tmp_buf[16];
                sprintf(tmp_buf, "%d", right);
                int len = strlen(tmp_buf);
                int need_len = strlen((char *)(mData)) + len + 1;
                if( mSize < need_len )
                    this->resize(need_len);
                strcat((char *)(mData), tmp_buf);
                return *this;
            }
        }
        return *this;
    };

    /**
     * 函数名称：const CVar & operator += (const char * right)
     * 函数功能：+=函数重载
     * 使用注意：如果左值是int型，则把right转换为整型值，按照int相加;
     *           如果左值是char *型的，把right追加到末尾
     *           本身数据类型不变
     * @param   right
     * @return  返回本身
     * @throws
     */
    const CVar & operator += (const char * right){
        switch(mType){
            case VAR_TYPE_INT:{
                *(int *)(mData) += atoi(right);
                return *this;
            }
            case VAR_TYPE_STR:{
                int len = strlen(right);
                int need_len = strlen((char *)(mData)) + len + 1;
                if( mSize < need_len )
                    this->resize(need_len);
                strcat((char *)(mData), right);
                return *this;
            }
        }
        return *this;
    };

    /**
     * 函数名称：friend ostream & operator << (ostream & os, CVar & var)
     * 函数功能：把var数据输出到流中
     * 使用注意：只有是int,double,char *的数据才会输出
     * @param   os          流对象
     * @param   var         要输出的var
     * @return  返回流对象的引用
     * @throws
     */
    friend ostream & operator << (ostream & os, const CVar & var){
        switch(var.mType){
            case VAR_TYPE_INT:{
                return ( os << *(int*)var.mData );
            }
            case VAR_TYPE_STR:{
                return ( os << var.mData );
            }
            case VAR_TYPE_DOUBLE:{
                return (os << *(double*)var.mData);
            }
        }
        return os;
    };
    
//public:

    /**
     * 函数名称：CVar()
     * 函数功能：默认构造函数
     * 使用注意：无
     * @param
     * @return
     * @throws
     */
    CVar(){
        mData = new char[DEFAULT_DATA_SIZE];
        mSize = DEFAULT_DATA_SIZE;
        mType = VAR_TYPE_INT;
        memset(mData, 0, DEFAULT_DATA_SIZE);
    };
    
    /**
     * 函数名称：CVar(const CVar & right)
     * 函数功能：拷贝构造函数
     * 使用注意：无
     * @param   right
     * @return
     * @throws
     */
    CVar(const CVar & right){
        mData = NULL;
        this->resize(right.size());
        memcpy(mData, right.ptr(), right.size());
        mType = right.mType;
    };

    /**
     * 函数名称：CVar(int right)
     * 函数功能：int型构造函数
     * 使用注意：无
     * @param   right
     * @return
     * @throws
     */
    explicit CVar(int right){
        mData = NULL;
        this->resize(sizeof(int));
        memcpy(mData, &right, sizeof(int));
        mType = VAR_TYPE_INT;
    };

    /**
     * 函数名称：CVar(double right)
     * 函数功能：double型构造函数
     * 使用注意：无
     * @param   right
     * @return
     * @throws
     */
    explicit CVar(double right){
        mData = NULL;
        this->resize(sizeof(double));
        memcpy(mData, &right, sizeof(double));
        mType = VAR_TYPE_DOUBLE;
    };

    /**
     * 函数名称：CVar(const char * right)
     * 函数功能：char*型构造函数
     * 使用注意：无
     * @param   right
     * @return
     * @throws
     */
    explicit CVar(const char * right){
        mData = NULL;
        int len = strlen(right)+1;
        this->resize(len);
        strcpy(mData, right);
        mType = VAR_TYPE_STR;
    };

    /**
     * 函数名称：void assign(void * ptr, int sz)
     * 函数功能：char*型构造函数
     * 使用注意：无
     * @param   ptr         数据指针
     * @param   sz          数据长度
     * @return
     * @throws
     */
    void assign(const void * ptr, int sz){    //二进制数据
        if( mSize < sz ) this->resize(sz);
        memcpy(mData, ptr, sz);
        mType = VAR_TYPE_RAW;
    };

    /**
     * 函数名称：virtual ~CVar()
     * 函数功能：析构函数
     * 使用注意：无
     * @param
     * @return
     * @throws
     */
    virtual ~CVar(){
        delete[] mData;
    };
};

typedef CVar var;

#endif  /* CVAR_HEADER_INCLUDED */


