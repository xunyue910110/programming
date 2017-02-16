#ifndef CCDRATTR_HEADER_INCLUDED
#define CCDRATTR_HEADER_INCLUDED

#include <vector>
#include <string>
//#include "xmlpp.h"
#include "system.h"

const int DATA_INT       = 1;                                       //int型
const int DATA_DOUBLE    = 2;                                       //double
const int DATA_STRING    = 3;                                       //char[]

class CCdrAttr
{
    struct __CFieldAttr{
        string  name;                                           //字段名
        string  caption;                                        //字段标题
        int     data_type;                                      //数据类型
        int     data_length;                                    //数据长度
        __CFieldAttr(const __CFieldAttr & right){
            name = right.name;
            caption = right.caption;
            data_type = right.data_type;
            data_length = right.data_length;
        }
        
        __CFieldAttr(){}
    };
    
    vector<__CFieldAttr>    __FieldVector;
    
public:

    bool parseFile(const char * file_name){                     //解析输入文件
    /*    __FieldVector.clear();
        XMLContext ctx;
	    XMLDocument doc(&ctx);
	    
	    //解析文档
	    if( !doc.load_file(file_name) ){
	        return false;
	    }
	    
	    if( !doc.cd("/fields") ){                               //设置工作目录
	        cout << "no path /fields" << endl;
	        return false;
	    }
	    
	    //把名字为field的子节点都取出来，放到nodelist里
	    XMLNodeList nodelist = doc.getCurrentNode()->children("field");
	    XMLNodeListIterator itr = nodelist.begin();

        //遍历nodelist，把每个节点的信息给取出来
        __CFieldAttr FieldAttr;
        for(; itr != nodelist.end(); itr++){                    //取出信息
            XMLNode * pn = *itr;
            
            FieldAttr.name = pn->get_attr("name");              //字段名
            FieldAttr.caption = pn->get_attr("caption");        //字段标题

            string dt = pn->get_attr("data_type");              //字段数据类型
			if( dt == "string" )
				FieldAttr.data_type = DATA_STRING;
			else if( dt == "int" )
                FieldAttr.data_type = DATA_INT;
            else if( dt == "double" )
                FieldAttr.data_type = DATA_DOUBLE;
            else
                return false;
            
            //取字段长度
			FieldAttr.data_length = atoi(pn->get_attr("length").c_str());             
			__FieldVector.push_back(FieldAttr);
        }
       */ 
        return true;
    }
    
    unsigned int size() const{                                  //返回字段数量
        return __FieldVector.size();
    }
    
    int type(int i){                                            //返回第i个字段的数据类型
        return __FieldVector[i].data_type;
    }
    
    int length(int i){                                          //返回第i个字段的数据长度
        return __FieldVector[i].data_length;
    }
    
    const char * name(int i){                                   //返回第i个字段的字段名
        return __FieldVector[i].name.c_str();
    }
    
    const char * caption(int i){                                //返回第i个字段的字段标题
        return __FieldVector[i].caption.c_str();
    }
    
    int type(const char * name){                                //返回名为name的字段的数据类型
        for( unsigned int i = 0; i < __FieldVector.size(); i++ ){
            if( __FieldVector[i].name == name )
                return __FieldVector[i].data_type;
        }
        return -1;
    }
    
    int length(const char * name){                              //返回名为name的字段的数据长度
        for( unsigned int i = 0; i < __FieldVector.size(); i++ ){
            if( __FieldVector[i].name == name )
                return __FieldVector[i].data_length;
        }
        return -1;
    }
    
    const char * caption(const char * name){                    //返回名为name的字段的标题
        for( unsigned int i = 0; i < __FieldVector.size(); i++ ){
            if( __FieldVector[i].name == name )
                return __FieldVector[i].caption.c_str();
        }
        return NULL;
    }
    
    int index(const char * name){                               //返回名为name的字段的索引
        for( unsigned int i = 0; i < __FieldVector.size(); i++ ){
            if( __FieldVector[i].name == name )
                return i;
        }
        return -1;
    }
};

#endif  /* CDRATTR_HEADER_INCLUDED */

