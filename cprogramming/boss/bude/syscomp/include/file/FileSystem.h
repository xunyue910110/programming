
#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

/**
 * \class FileSystem FileSystem.h
 * \brief 代表一个文件系统的一个抽象的基类 
 */

#include <list>
#include <string>

using namespace std;

typedef unsigned char Byte;

class File;


class FileSystem {
  public:
    enum {EndOfFile = -1 /*!< end of file reached */ };
    
    enum Attribute {Exists      = 0x01 /*!< File exists */,
                    RegularFile = 0x02 ,
                    /*!< A regular file i.e. not a directory */
                    Directory   = 0x04 /*!< File is a directory */,
                    Hidden      = 0x08 /*!< File is hidden */,
                    ReadOnly    = 0x10 /*!< File is read-only */};
    
    enum AccessMode {ReadAccess  = 0x01 /*!< Request read access */,
                     WriteAccess = 0x02 /*!< Request write access */};

    enum CreationDisp {OpenExisting         ,
                       /*!< open existing file only */
                       OpenCreateAppend     ,
                       /*!< open existing or create new, preserve existing contents */
                       OpenCreateExclusive  ,
                       /*!< create non-existing file only */
                       OpenCreateTruncate   
                       /*!< open existing or create new, destroy existing contents */ 
                      };
            
  public:

    virtual ~FileSystem(){}

    /**
     * 取得文件系统对象
     */
    static FileSystem* getFileSystem();

    /**
     * 设置全局文件系统(FileSystem)对象
     * @param pFileSystem 文件系统对象
     */
    static void setFileSystem(FileSystem* pFileSystem);

    /**
     * 获取当前工作目录路径
     * @param path 当前工作目录路径
     * @return  true 表示获取成功,false 失败。
     */
    virtual int getCurrentDirectory(string& path) const = 0;

    /**
     * 返回目录分割符字符串
     */
    virtual string getSeparator() const;

    /**
     * 返回分割字符
     */
    virtual char getSeparatorChar() const = 0;

    /**
     * 返回规范化路径名
     * @param path 路径
     */
    virtual string normalize(const string& path) const;

    /**
     * 返回抽象路径前缀长度
     * @param path 路径
     */
    virtual size_t getPrefixLength(const string& path) const = 0;

    /**
     * 将输入路径解析为相对路径
     * @param path 输入路径(相对路径)
     * @return  >=0 表示成功,<0 失败。
     */
    virtual int resolve(string& path) const;

    /**
     * 根据父路径和子路径返回完整路径，如果子路径已经是绝对
     *     路径则直接返回该子路径
     * @param parent 父路径
     * @param child  子路径
     */
    virtual string resolve(const string& parent, const string& child) const;


    /**
     * 测试路径是否为绝对路径
     * @param path 路径
     * @return  true 是,false 不是。
     */
    virtual bool isAbsolute(const string& path) const = 0;

    /**
     * 测试文件系统是否对大小写敏感
     * @return  ture 是,false 不是。
     */
    virtual bool isCaseSensitive() const = 0;

    /**
     * 获取规范的路径
     * @param path 源路径
     * @param ret_path 规范化后的路径
     * @return  >=0 表示成功,<0 失败。
     */
    virtual int canonicalize(const string& path, string& ret_path) const;
    
    /**
     * 返回文件属性。属性为定义在 enum FileSystem::Attribute
     * @param path 文件路径
     * @return  >=0 表示成功,<0 失败。
     */
    virtual int getFileAttributeFlags(const string& path) const = 0;

    /**
     * 测试是否可以按指定的模式访问指定的文件
     * @param path  文件路径
     * @param mode 访问模式
     * @return  true 是,false 不是。
     */
    virtual bool checkAccess(const string& path, AccessMode mode) const = 0;

    /**
     * 获取文件的最后修改时间
     * @param path 文件路径
     * @param time 最后修改时间
     * @return >=0 表示成功,<0 失败。
     */
    virtual int getLastModifiedTime(const string& path, time_t& time) const = 0;

    /**
     * 返回文件长度
     * @param path 文件路径
     * @return >=0 表示成功,<0 表示错误发生。
     */
    virtual size_t getLength(const string& path) const = 0;

    /**
     * 打开文件
     * @param path         文件路径
     * @param accessMode   访问模式(读或写）
     * @param creationDisp  创建模式
     * @param attributes     文件属性
     * @return >=0 表示文件打开成功返回文件句柄,<0 失败。
     */
    virtual int openFile(const string& path,
                         const int accessMode,
                         const CreationDisp creationDisp,
                         const int attributes) const = 0;

    /**
     * 关闭文件
     * @param pFD  文件句柄
     * @return >=0 表示成功,<0 失败。
     */ 
    virtual int closeFile(int pFD) const = 0;

    /**
     * 删除文件
     * @param path 文件路径
     * @return >=0 表示成功,<0 失败。
     */ 
    virtual int deleteFile(const string& path) const = 0;

    /**
     * 文件列表
     * @param path   文件路径
     * @param file_list 文件列表
     * @return >=0 表示成功,<0 失败。
     */
    virtual int listDirectory(const string& path,
                              std::list<string> & file_list) const = 0;

    /**
     * 建立一个目录
     * @param path  路径
     * @return >=0 表示成功,<0 失败。
     */
    virtual int createDirectory(const string& path) const = 0;

    /**
     * 文件改名
     * @param old_path  源文件路径
     * @param new_path  目标文件路径
     * @return >=0 表示成功,<0 失败。
     */
    virtual int rename(const string& old_path, 
                       const string& new_path) const = 0;

    /**
     * 设置文件最后修改时间
     * @param path 文件路径
     * @param time 最后修改时间
     * @return >=0 表示成功,<0 失败。
     */
    virtual int setLastModifiedTime(const string& path, 
                                    const time_t& time) const = 0;

    /**
     * 设置只读属性
     * @param path      文件路径
     * @param bReadOnly 是否只读
     * @return >=0 表示成功,<0 失败。
     */   
    virtual int setReadOnly(const string& path, const bool bReadOnly) const = 0;
      
    /**
     * 读出文件内容.
     * @param fd      文件路径
     * @param pBuffer 数据缓冲区指针
     * @param bufLen  数据缓冲区大小
     * @return >0 读出的字节数,=0 到达文件尾,<0 出错。
     */
    virtual int readFile(int fd, Byte* pBuffer, size_t bufLen) const = 0;

    /**
     * 将指定的内容写入文件
     * @param fd      文件路径
     * @param pBuffer 数据缓冲区指针
     * @param bufLen  数据缓冲区大小
     * @return >=0 写入的字节数,<0 出错。
     */
    virtual int writeFile(int fd, const Byte* pBuffer, size_t bufLen) const = 0;
     
  private:
    static FileSystem* s_pFileSystem;
};

#endif //__FILESYSTEM_H__
