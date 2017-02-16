#ifndef __TD_S_FTPHOST_H_
#define __TD_S_FTPHOST_H_

#include <vector>
#include <sstream>

#include "config-all.h"
#include "udbi/udbi.h"

USING_NAMESPACE(std)

/**=!===============SEGMENG2==========================*/
namespace ParaDom {

using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class Ftphost
 *  @brief TD_S_FTPHOST表的实体类
 *  映射数据库TD_S_FTPHOST表的所有字段为数据成员，
 *  并对状态字段的有意义解释映射进行封装（暂由cparam提供）
 */
class Ftphost {

public:
	//! 缺省构造函数，目前没有实现
    Ftphost() { init(); };

	//! 析构函数    
    virtual ~Ftphost() { };
    
    //! 初始化函数
    void init();
    
    //! 把对象的数据成员以格式化的字符串输出
    inline string toString()
    {
    	ostringstream ss;
		ss << *this;
		return ss.str();	
	}

    //! << 运算符重载
    friend ostream& operator<<(ostream& out, const Ftphost& rhs)
	{
		out << "eparchyCode: " << rhs.eparchyCode << endl;
		out << "ftpHost: " << rhs.ftpHost << endl;
		out << "ftpUsrid: " << rhs.ftpUsrid << endl;
		out << "ftpPasswd: " << rhs.ftpPasswd << endl;
		out << "ftpPath: " << rhs.ftpPath << endl;
		out << "ftpMod: " << rhs.ftpMod << endl;
		out << "remark: " << rhs.remark << endl;
		out << "updateTime: " << rhs.updateTime << endl;
		out << "updateStaffId: " << rhs.updateStaffId << endl;
		out << "updateDepartId: " << rhs.updateDepartId << endl;
		
		return out;
	}	
    
public:
	//! 数据成员，可用数据类型：int、string
    /** 归属地州编码 */
    string  eparchyCode; 

    /** FTP主机 */
    string  ftpHost; 

    /** FTP用户 */
    string  ftpUsrid; 

    /** FTP口令 */
    string  ftpPasswd; 

    /** FTP路径 */
    string  ftpPath; 

    /** FTP模式 ：1－支持断点续传，其它－不支持断点续传  */
    string  ftpMod; 

    /** 备注 */
    string  remark; 

    /** 更新时间 */
    string  updateTime; 

    /** 更新员工 */
    string  updateStaffId; 

    /** 更新部门 */
    string  updateDepartId; 

    
};

/** @brief Ftphost对象指针的vector类型定义
 *  对象数组，用于保存查询结果为多条记录时的数据
 */
typedef vector<Ftphost> VFtphost;


/**=~===============SEGMENG4==========================*/

/** @class FtphostDAO
 *  @brief TD_S_FTPHOST表的数据访问接口
 *  对TD_S_FTPHOST表的数据库访问进行封装，
 *  提供所有SQL执行方法，方便在业务组件中直接引用
 */
class FtphostDAO : public MTable {
public:
	//! 缺省构造函数
    FtphostDAO(): MTable("TD_S_FTPHOST") { };
    
    //! 析构函数
    virtual ~FtphostDAO() { };
    
    //! 返回类名
    const char* ClassName() { return "FtphostDAO"; }
    
    //! 多条记录查询函数，返回实体对象数组
    int jselect(Ftphost &rFtphost, const string &selBy);

    //! 单条记录查询函数，返回实体对象
    int jselect(VFtphost &vrFtphost, const string &selBy);
    
    //! 批量插入记录函数
    int jinsert(VFtphost &vrFtphost, const string& insBy);
    
    //! 更新记录函数
    inline int jupdate(const string &updBy)
    {
    	try
    	{
    		return ExecUpdate(updBy);
    	} catch (EUdbiException& er) 
    	{ 
    		string expmsg("\nDaoName: "); 
    		expmsg += ClassName(); 
    		expmsg += ", Oper: Update, updBy: "+updBy+'\n'; 
    		expmsg += parToString(); 
    		THROW_A(DAOException, er, expmsg.c_str()); 
    	}
    }
    
    //! 删除记录函数
    inline int jdelete(const string &delBy)
    {
    	try
    	{
    		return ExecDelete(delBy);
    	} catch (EUdbiException& er) 
    	{ 
    		string expmsg("\nDaoName: "); 
    		expmsg += ClassName(); 
    		expmsg += ", Oper: Delete, delBy: "+delBy+'\n'; 
    		expmsg += parToString(); 
    		THROW_A(DAOException, er, expmsg.c_str()); 
    	}
    }
    
    //! 插入记录函数
    inline int jinsert(const string &insBy)
    {
    	try
    	{
    		return ExecInsert(insBy);
    	} catch (EUdbiException& er) 
    	{ 
    		string expmsg("\nDaoName: "); 
    		expmsg += ClassName(); 
    		expmsg += ", Oper: Insert, insBy: "+insBy+'\n'; 
    		expmsg += parToString(); 
    		THROW_A(DAOException, er, expmsg.c_str()); 
    	}
    }
    
    //! 扩展SQL语句执行函数
    inline int jexecext(const string& extBy, int exTimes)
    {
    	return ExecExt(extBy, exTimes);
    }

private:
	//! 拷贝构造函数与赋值运算符，未实现，限制调用
	FtphostDAO(const FtphostDAO& rhs);
	FtphostDAO& operator=(const FtphostDAO& rhs);
	
};

} // namespace ParaDom

#endif
