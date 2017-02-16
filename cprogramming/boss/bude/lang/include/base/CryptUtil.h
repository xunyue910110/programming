

#ifndef __CRYPTUTIL_H__
#define __CRYPTUTIL_H__

#include "config-all.h"

#include <string>
#include <stdio.h>

USING_NAMESPACE(std);

/** 
    @class CryptUtil 
    
    @brief 加密解密工具

 */

class CryptUtil {
  public:
  
    /**
     * 加密数据
     * @param data 待加密的数据
     * @param encrypted 加密后结果
     * @return 是否加密成功
     */
    static bool encrypt(const char * data,string & encrypted) ;

    /**
     * 解密数据
     * @param data 待解密的数据
     * @param encrypted 解密后结果
     * @return 是否解密成功
     */
    static bool decrypt(const char * data,string & decrypted);


    /**
     * 解密一数据包  
     * 数据包:一种简单的封装形式.
     *          "\\pak{....}" 一种密文数据包;
     *          "\\pwd[....]" 相对应的明文数据包.
     * @param value 待解密的数据
     * @param erase 是否去除明文数据包的封装标志 
     * @return 是否成功
     */
    static bool decrypt_pakage(string & value,bool erase_pwd = true) {  
        
      size_t lpos = value.find("\\\\pak{",0);
      if( lpos != string::npos ) {
        int rpos= value.find("}\\", lpos);
        if ( rpos != string::npos)  {
            string encrypted = value.substr(lpos+6,rpos-(lpos+6));
            string decrypted;
            //cout << "encrypted" << encrypted << endl;
            if (!CryptUtil::decrypt(encrypted.c_str(),decrypted)) {
              return false;
            }
            value= value.substr(0,lpos)+decrypted+value.substr(rpos+2);
            return decrypt_pakage(value,erase_pwd);
        }
      }

      if (!erase_pwd)
         return true;
      
      // 明码数据包
      lpos = value.find("\\\\pwd[",0);
      if( lpos != string::npos ) {
        int rpos= value.find("]\\", lpos);
        if ( rpos != string::npos)  {
            string source = value.substr(lpos+6,rpos-(lpos+6));
            value= value.substr(0,lpos)+ source +value.substr(rpos+2);
            return decrypt_pakage(value,erase_pwd);
        }
      }

      return true;
    };


    /**
     * 加密一个数据包
     * 数据包:一种简单的封装形式.
     *    "\\pak{....}" 一种密文数据包;
     *    "\\pwd[....]" 相对应的明文数据包.
     * 该函数作用后\\pwd[....]数据包被替换为:\\{...}.
     * @param value 待解密的数据
     * @return 是否成功
     */
    static bool encrypt_pakage(string & value) {  
      size_t lpos = value.find("\\\\pwd[",0);
      if( lpos != string::npos ) {
        int rpos= value.find("]\\", lpos);
        if ( rpos != string::npos)  {
            string buffer = value.substr(lpos+6,rpos-(lpos+6));
            string encrypted;
            if (!CryptUtil::encrypt(buffer.c_str(),encrypted)) {
              return false;
            }
            value= value.substr(0,lpos)+"\\\\pak{"+encrypted+"}\\"+value.substr(rpos+2);
            return encrypt_pakage(value);
        }
      }
      return true;
    };


    /**
     * 密文/明文翻译工具
     *
     *   顺序检索输入数据,遇到的\\pak{...}将被解密,去除封装标记后原数据
     * 被相应的解密后明文替换;包\\pwd[...]内容则被加密并以 \\pak{...}的形式替换。
     */
    static int translate() {
        const int MAX_BUFFER_SIZE = 1024*1024; 
        
        char* buffer = new char[MAX_BUFFER_SIZE +1];
        while (fgets(buffer, MAX_BUFFER_SIZE , stdin) != 0) {
            string data = buffer;
            decrypt_pakage(data,false);
            encrypt_pakage(data);
            fputs(data.c_str(), stdout);
        }
        delete[] buffer;
        return 0;
    }


};

#endif /*__CRYPTUTIL_H__*/

