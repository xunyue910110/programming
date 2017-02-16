#ifndef __TD_M_BACKUPDATETASK_H_
#define __TD_M_BACKUPDATETASK_H_

#include <vector>
#include <sstream>

#include "base/config-all.h"
#include "udbi/udbi.h"

USING_NAMESPACE(std)

/**=!===============SEGMENG2==========================*/
namespace ParaDom {

using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class Backupdatetask
 *  @brief TD_M_BACKUPDATETASK表的实体类
 *  映射数据库TD_M_BACKUPDATETASK表的所有字段为数据成员，
 *  并对状态字段的有意义解释映射进行封装（暂由cparam提供）
 */
class Backupdatetask {

public:
	//! 缺省构造函数，目前没有实现
    Backupdatetask() { init(); };

	//! 析构函数    
    virtual ~Backupdatetask() { };
    
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
    friend ostream& operator<<(ostream& out, const Backupdatetask& rhs)
	{
		out << "updateId: " << rhs.updateId << endl;
		out << "updateDllName: " << rhs.updateDllName << endl;
		out << "updateInterval: " << rhs.updateInterval << endl;
		out << "updatePri: " << rhs.updatePri << endl;
		out << "updateTag: " << rhs.updateTag << endl;
		out << "remark: " << rhs.remark << endl;
		out << "updateTime: " << rhs.updateTime << endl;
		out << "updateStaffId: " << rhs.updateStaffId << endl;
		out << "updateDepartId: " << rhs.updateDepartId << endl;
		out << "eparchyCode: " << rhs.eparchyCode << endl;
		
		return out;
	}	
    
public:
	//! 数据成员，可用数据类型：int、string
    string  updateId; 
    string  updateDllName; 
    int  updateInterval; 
    int  updatePri; 
    string  updateTag; 
    string  remark; 
    string  updateTime; 
    string  updateStaffId; 
    string  updateDepartId; 
    string  eparchyCode; 
    
};

/** @brief Backupdatetask对象指针的vector类型定义
 *  对象数组，用于保存查询结果为多条记录时的数据
 */
typedef vector<Backupdatetask> VBackupdatetask;


/**=~===============SEGMENG4==========================*/

/** @class BackupdatetaskDAO
 *  @brief TD_M_BACKUPDATETASK表的数据访问接口
 *  对TD_M_BACKUPDATETASK表的数据库访问进行封装，
 *  提供所有SQL执行方法，方便在业务组件中直接引用
 */
class BackupdatetaskDAO : public MTable {
public:
	//! 缺省构造函数
    BackupdatetaskDAO(): MTable("TD_M_BACKUPDATETASK") { };
    
    //! 析构函数
    virtual ~BackupdatetaskDAO() { };
    
    //! 返回类名
    const char* ClassName() { return "BackupdatetaskDAO"; }
    
    //! 多条记录查询函数，返回实体对象数组
    int jselect(Backupdatetask &rBackupdatetask, const string &selBy);

    //! 单条记录查询函数，返回实体对象
    int jselect(VBackupdatetask &vrBackupdatetask, const string &selBy);
    
    //! 批量插入记录函数
    int jinsert(VBackupdatetask &vrBackupdatetask, const string& insBy);
    
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
	BackupdatetaskDAO(const BackupdatetaskDAO& rhs);
	BackupdatetaskDAO& operator=(const BackupdatetaskDAO& rhs);
	
};

} // namespace ParaDom

#endif
