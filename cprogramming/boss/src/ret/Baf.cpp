// ##########################################
// Source file : baf.cpp
// System      : Mobile Billing System
// Version     : 0.1.24
// Language    : ANSI C++
// OS Platform : ux
// Authors     : chenm
// E_mail      : chenm@lianchuang.com
// Create      : 20020412
// Update      :
// Copyright(C): 2002 by chenmin, Linkage.
// ##########################################

#include "Baf.h"

//##ModelId=3CBA5ECF024B
bool Baf::initialize( vector<string> aParameterList )
{
    char c_tmp[100];
    int len,i_mod_len,i;
    string  cp_mod_code;
    string theArgname;

    for( int j=0;j<aParameterList.size();j++)
    {
       theArgname = aParameterList[j];
       len = theArgname.size();
       strcpy( c_tmp, theArgname.c_str() );

        //得到空格前的串,即字module段的名称
	   for( i=0 ;i<strlen(c_tmp);i++)
	   {
	        if (c_tmp[i]==' ' || c_tmp[i]=='\t')
	            break;
	   }

	   cp_mod_code = theArgname.substr(0,i);
	   theArgname  = theArgname.substr(i,len-i);

	   len = theArgname.size();
	   strcpy( c_tmp,theArgname.c_str() );

	     //得到空格后的串,即module段的长度
	   for(i=0;i<strlen(c_tmp);i++)
	   {
	        if (c_tmp[i]!=' ' && c_tmp[i]!='\t')
	            break;
	   }
	   i_mod_len = atoi( theArgname.substr(i,len-i).c_str() );


	   //将模块的相关信息插入容器中
	   m_bafMap.insert( map<string,int>::value_type(cp_mod_code,i_mod_len) );

	}

	return true;
}

//##ModelId=3CBA5ECF02CD
int  Baf::getLenth(const unsigned char *buffer,const int length,
                   const string &s_FieldName,const int i_StartByte,const int i_sequence)
{
    int i_post=0,i_seq=0,j;
    char c_module_id[5];
    //--char c_tmp[10],tmp1[3];
    unsigned char c_tmp[10];
    char tmp1[3];
    map<string, int>::iterator iteor;

	//读取Module contents识别码.
    //--memcpy(c_tmp,(char *)(buffer) + i_StartByte,2);
    memcpy(c_tmp,buffer + i_StartByte,2);
    
    c_module_id[0]=0;

    for(j=0;j<2;j++)
    {
        sprintf(tmp1,"%02X",c_tmp[j]);
        strcat(c_module_id,tmp1);
    }
    c_module_id[4]=0;

    //判断是否为配置文件中定义的module id,如:"NORT_M_003C"
    //若当前模块不是配置中定义的,则根据该模块的长度跳过,取下一模块

    //cout<<"the org module name is: "<<s_FieldName<<endl;
    // modified by chenm 2005-3-7
    while(1)
    {
        // add by chenm 2005-3-7 如果找到匹配模块,并且出现的顺序和配置文件中一致,才跳出  
        if(s_FieldName.compare(7,4,c_module_id) == 0)
        {
            if( i_seq == i_sequence) 
	        {
	        	break;	
	        }
	        ++i_seq;
    	}
    	// over 2005-3-7 
    	
        iteor = m_bafMap.find( string(c_module_id) );

        //cout<<"the c_module is: "<<c_module_id<<endl;
        //按module代码查找
        if( iteor != m_bafMap.end() )  
        {
            i_post += iteor->second;
        }  
        else   
        {
            i_post += 2;        // 没有找到则自增2,防止死循环
        }

        if( (i_StartByte+i_post) >= (length-2)) 
        {
            return 0;           //若超出原始cdr长度还没有找到匹配,则返回0
        }
        //读出下一个module id
        memcpy(c_tmp,(char *)(buffer) + i_StartByte+i_post ,2);
        c_module_id[0]=0;

        for( j=0;j<2;j++)
        {
            sprintf(tmp1,"%02X",c_tmp[j]);
            strcat(c_module_id,tmp1);
        }
        c_module_id[4]=0;
    }

    return( i_StartByte+i_post );
}
