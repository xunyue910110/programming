
#ifndef __SIMPLECONFIGURE_H__
#define __SIMPLECONFIGURE_H__

#include "config-all.h"

////////////////////////////////////////////////////////////////////////////////
/// Process information in simple configuration files.
///
/// Classes to perform cinfigure operations including:
/// - Read and parse configuration file
/// - Search etc. the retrieved configuration information
///
/// The Simple Configure is defined as a text file with sections and entries in the 
/// form of "name=value" strings in each section. 
///
/// For an explanation of the syntax of the Simple Configure  file, see the
/// C++ Toolkit documentation.
/// 


#include <stdio.h>
#include <string>

USING_NAMESPACE(std);

////////////////////////////////////////////////////////////////////////////////
///
/// SimpleConfigure --
/// 
/// 简单配置文件读取
///
/// Define the SimpleConfigure.
///
/// 读取访问配置文件功能
/// 
///
/// 配置例子:
/// @code
/// 
/// 	[DB]
/// 	USER          = xjwjjs		#用户名
/// 	PASSWD        = xjwjjs123	#口令
/// 	SERVER        = XJ_SRV		#服务
/// 	
/// 	[GENERAL1]
/// 	SOURCEPATH    = /data/xjwjjs/qyx/normal		#入库数据存放路径
/// 	NOTINPATH     = /data/xjwjjs/qyx/nornotin		#入库失败文件存放路径
/// 	LIMITNUM      = 1000		#未用
/// 	DATEFIELD     = dloc	#需要转换的日期型字段
/// 
/// @endcode
///
/// @sa SysParam 更为复杂的配置文件处理类
///

class SimpleConfigure {
    public:
        SimpleConfigure();
        ~SimpleConfigure();
        
        /// 初始化配置
        /// @param filename 配置文件名
        /// @return 0 成功,1 失败
        int initialize (const char *filename);
        int end();

        /// 取得配置信息
        /// @param sect 节名
        /// @param param 参数名
        /// @param val 返回值
        /// @param len 长度
        /// @return 0 表示成功,1 错误
        int getval(const char *sect, const char *param,
                   char *val, int len);


        /// 取得配置信息
        /// @param sect 节名
        /// @param param 参数名
        /// @param val 返回值
        /// @param default 缺省值
        /// @return 0 表示成功,1 错误
        string getval(const char *sect, const char *param,
                      const char * defaultvalue = "");

        ///定义配置文件 注释符 缺省 '#'
        int setremark(const char remark);          //defaule='#'

        ///定义配置文件 键值分割符 缺省 '='
        int setspacemark(const char spacemark);    //defaule='='
        
        /// 取得
        
        int geterrmsg(char *errmsg, const int len);         
    private:
        char cRemark;
        char cSpacemark;
        FILE *hCfg;
};

#endif /*__CONFIG_H__*/

