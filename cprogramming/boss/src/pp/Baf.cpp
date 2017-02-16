// ##########################################
// Source file : Baf.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "Baf.h"

//##ModelId=4855C5790167
bool Baf::initialize(vector<string> aParameterList)
{
    char c_tmp[100];
    int len, i_mod_len, i;
    string cp_mod_code, theArgname;

    for( int j = 0; j < aParameterList.size(); j++)
    {
        theArgname = aParameterList[j];
        len = theArgname.size();
        strcpy(c_tmp, theArgname.c_str());

        //得到空格前的串,即字module段的名称
        for(i = 0; i < strlen(c_tmp); i++)
        {
            if (c_tmp[i] == ' ' || c_tmp[i] == '\t')
            {
                break;
            }
        }

        cp_mod_code = theArgname.substr(0,i);
        theArgname = theArgname.substr(i,len-i);
        len = theArgname.size();
        strcpy(c_tmp, theArgname.c_str());

        //得到空格后的串,即module段的长度
        for(i = 0; i < strlen(c_tmp); i++)
        {
            if (c_tmp[i] != ' ' && c_tmp[i] != '\t')
            {
                break;
            }
        }
        i_mod_len = atoi(theArgname.substr(i, len - i).c_str());

       //将模块的相关信息插入容器中
       m_bafMap.insert( map<string, int>::value_type(cp_mod_code, i_mod_len));
    }
    return true;
}

//##ModelId=4855C5900203
int Baf::getLenth(const unsigned char *buffer, const int length, const string &s_FieldName,
                    const int i_StartByte, const int i_sequence, int &moduleLength)
{
    int i_post=0, i_seq=0, j,k, mlen;
    char c_module_id[5];
    unsigned char c_tmp[10];
    char tmp1[3];
    map<string, int>::iterator iteor;
    int flag = 0, pos = 0, seqTemp = 0;         //used by indefinite module

    //读取Module contents识别码.
    memcpy(c_tmp, buffer + i_StartByte, 2);

    char s[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    for (int j = 0; j < 2; j++)
    {
        c_module_id[j*2] = s[(c_tmp[j]&0xf0)>>4];
        c_module_id[j*2+1] = s[c_tmp[j]&0x0f];
    }
    c_module_id[4] = 0;

    //判断是否为配置文件中定义的module id,如:"NORT_M_003C"
    //若当前模块不是配置中定义的,则根据该模块的长度跳过,取下一模块

    while(1)
    {
        // 如果找到匹配模块,并且出现的顺序和配置文件中一致,才跳出
        if(s_FieldName.compare(7, 4, c_module_id) == 0)
        {
            if(s_FieldName.compare(5, 1, "I") == 0)
            {
                i_seq = -10;     //是不定长模块    我们假定几个同一种类型的不定长模块的是出现在一块的。并且只出现一次。
                pos = i_post;
                flag = 1;
                mlen = 0;
            }
            if( i_seq == i_sequence)
            {
                break;
            }
            ++i_seq;
            //纪录定长模块的序号
            if( i_seq >= 0 )
            {
                seqTemp = i_seq;
            }

            if( flag == 1 )
            {
                i_seq = -10;
            }
        }

        iteor = m_bafMap.find(string(c_module_id));

        //按module代码查找
        if( iteor != m_bafMap.end())
        {
           if ( iteor->second == 0 )
           {
                i_seq = -10;
                mlen = 0;
           }

           while(i_seq == -10 )    //连续的几个不定长模块
           {
               memcpy(c_tmp, (char *)(buffer) + i_StartByte + i_post + 2, 2);

               for (int j = 0; j < 2; j++)
               {
                    c_module_id[j*2] = s[(c_tmp[j]&0xf0)>>4];
                    c_module_id[j*2+1] = s[c_tmp[j]&0x0f];
               }
               c_module_id[4] = 0;
               k = atoi(c_module_id);
               if ( flag == 1 )
               {
                    mlen += k;  //求得几个变长模块的长度
               }

               i_post += k;
               //iteor->second += j; //改变定义的长度，如果不行还可以删除元素，再插入新元素
               memcpy(c_tmp, (char *)(buffer) + i_StartByte + i_post, 2);

               for (int j = 0; j < 2; j++)
               {
                    c_module_id[j*2] = s[(c_tmp[j]&0xf0)>>4];
                    c_module_id[j*2+1] = s[c_tmp[j]&0x0f];
               }
               c_module_id[4] = 0;

               if(iteor->first.compare(c_module_id) == 0)
               {
                  i_seq = -10;
               }
               else
               {
                  i_seq = -11;
               }
           }

           if( flag ==1)
           {
                //化成单位为byte的长度
                moduleLength = mlen * 8;
                return(i_StartByte + pos);
           }

           if(i_seq >= 0)
           {
                i_post += iteor->second;
           }
           else
           {
                i_seq = seqTemp;
                //i_post += mlen;//定长模块在不定长模块后面的情形
           }
        }
        else
        {
            i_post += 2;        // 没有找到则自增2,防止死循环
        }

        if((i_StartByte + i_post) >= (length - 2))
        {
            return 0;           //若超出原始cdr长度还没有找到匹配,则返回0
        }
        //读出下一个module id
        memcpy(c_tmp, (char *)(buffer) + i_StartByte + i_post, 2);

        for (int j = 0; j < 2; j++)
        {
             c_module_id[j*2] = s[(c_tmp[j]&0xf0)>>4];
             c_module_id[j*2+1] = s[c_tmp[j]&0x0f];
        }
        c_module_id[4] = 0;
    }
    return( i_StartByte + i_post );
}

