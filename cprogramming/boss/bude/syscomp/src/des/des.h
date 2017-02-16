#ifndef __DES_H_LC_
#define __DES_H_LC_

#include <string>

using std::string;

/**
 *  class DES
 *
 *  NOTE: A file named 'key.key' must be provided with the following three mode:
 *		  1. at the program directory;
 *		  2. at current user's HOME directory;
 *		  3. defined by the enviroment variable 'DES_KEY';
 *
 */
class DES {
public:

    /**
     *  加密 每8个字符加密生成16位密文 不足8位按8位计
	 *　return  0  success
	 *         -1  open des-key file failure
     */
    static int encrypt(const string& _mtext,  string& _etext);

    /**
     *  解密
	 *  return  0  succes
	 *         -1  open des-key file failure
	 *         -2  invalid charactor
     */
    static int decrypt(const string& _etext, string& _mtext);

};

#endif
