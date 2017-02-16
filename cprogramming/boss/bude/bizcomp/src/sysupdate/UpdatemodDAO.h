#ifndef __TD_M_UPDATEMOD_H_
#define __TD_M_UPDATEMOD_H_

#include <vector>
#include <sstream>

#include "config-all.h"
#include "udbi/udbi.h"

USING_NAMESPACE(std)

/**=!===============SEGMENG2==========================*/
namespace ParaDom {

using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class Updatemod
 *  @brief TD_M_UPDATEMOD表的实体类
 *  映射数据库TD_M_UPDATEMOD表的所有字段为数据成员，
 *  并对状态字段的有意义解释映射进行封装（暂由cparam提供）
 */
class Updatemod {

public:
	//! 缺省构造函数，目前没有实现
    Updatemod() { init(); };

	//! 析构函数    
    virtual ~Updatemod() { };
    
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
    friend ostream& operator<<(ostream& out, const Updatemod& rhs)
	{
		out << "modCode: " << rhs.modCode << endl;
		out << "modVersion: " << rhs.modVersion << endl;
		out << "modBuildno: " << rhs.modBuildno << endl;
		out << "modSize: " << rhs.modSize << endl;
		out << "updateMethod: " << rhs.updateMethod << endl;
		out << "mustUpdateTag: " << rhs.mustUpdateTag << endl;
		out << "regUpdateTag: " << rhs.regUpdateTag << endl;
		out << "remark: " << rhs.remark << endl;
		out << "updateTime: " << rhs.updateTime << endl;
		out << "updateStaffId: " << rhs.updateStaffId << endl;
		out << "updateDepartId: " << rhs.updateDepartId << endl;
		out << "modPath: " << rhs.modPath << endl;
		out << "delTag: " << rhs.delTag << endl;
		out << "modName: " << rhs.modName << endl;
		out << "modType: " << rhs.modType << endl;
		
		return out;
	}	
    
public:
	//! 数据成员，可用数据类型：int、string
    /** 模块编码：子系统编码＋三位序列 */
    string  modCode; 

    /** 模块版本号 */
    string  modVersion; 

    /** 模块编译号 */
    int  modBuildno; 

    /** 模块大小 */
    string  modSize; 

    /** 更新方式：0－在线更新，1－退出更新 */
    string  updateMethod; 

    /** 强制更新标志：0－不强制，1－强制 */
    string  mustUpdateTag; 

    /** 更新注册标志：0－注册表，1－自身 */
    string  regUpdateTag; 

    /** 备注 */
    string  remark; 

    /** 更新时间 */
    string  updateTime; 

    /** 更新员工 */
    string  updateStaffId; 

    /** 更新部门 */
    string  updateDepartId; 

    /** 模块更新路径 */
    string  modPath; 

    /** 如果用户不更新该模块，是否删除它：0－不删除，1－删除 */
    string  delTag; 

    string  modName; 
    string  modType; 
    
};

/** @brief Updatemod对象指针的vector类型定义
 *  对象数组，用于保存查询结果为多条记录时的数据
 */
typedef vector<Updatemod> VUpdatemod;


/**=~===============SEGMENG4==========================*/

/** @class UpdatemodDAO
 *  @brief TD_M_UPDATEMOD表的数据访问接口
 *  对TD_M_UPDATEMOD表的数据库访问进行封装，
 *  提供所有SQL执行方法，方便在业务组件中直接引用
 */
class UpdatemodDAO : public MTable {
public:
	//! 缺省构造函数
    UpdatemodDAO(): MTable("TD_M_UPDATEMOD") { };
    
    //! 析构函数
    virtual ~UpdatemodDAO() { };
    
    //! 返回类名
    const char* ClassName() { return "UpdatemodDAO"; }
    
    //! 多条记录查询函数，返回实体对象数组
    int jselect(Updatemod &rUpdatemod, const string &selBy);

    //! 单条记录查询函数，返回实体对象
    int jselect(VUpdatemod &vrUpdatemod, const string &selBy);
    
    //! 批量插入记录函数
    int jinsert(VUpdatemod &vrUpdatemod, const string& insBy);
    
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
	UpdatemodDAO(const UpdatemodDAO& rhs);
	UpdatemodDAO& operator=(const UpdatemodDAO& rhs);
	
};

} // namespace ParaDom

#endif
