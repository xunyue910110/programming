
#ifndef __FILE_H__
#define __FILE_H__

/**
 * \class File File.h
 * \brief A system-independent representation of file and directory names.
 * 
 * Different operating systems employ various conventions for the naming of 
 *  files and directories.  
 * This class presents an abstract, system-independent view of file and 
 *  directory pathnames, together with convenient methods for manipulating 
 *  them.
 */

#include <iostream>
#include <list>
#include <string>

using namespace std;

class FileSystem;


class File {
  public:
    /**
     * 根据子父路径构造对象. 其中父是已经存在的File对象
     * @param parent 父文件对象
     * @param child  子路径
     */
    File(const File& parent, const string& child);

    /**
     * 根据路径构造对象
     * @param pathname 路径名
     */
    explicit File(const string& pathname);

    /**
     * 根据子父路径构造对象
     * @param parent 父路径
     * @param child  子路径
     */
    File(const string& parent, const string& child);

    /**
     * 拷贝构造
     */
    File(const File& rhs);

    /**
     * 测试文件是否存在
     * @return true 文件存在,false 不存在。
     */
    bool exists() const;
    
    /**
     * 测试文件是否可读
     * @return  true 文件可读,false 不可读。
     */
    bool canRead() const;
    
    /**
     * 测试文件是否可写
     * @return  true 文件可写,false 不可写。
     */
    bool canWrite() const;

    /**
     * 删除文件
     * @return  >=0 表示成功,<0 失败。
     */
    int deleteFile() const;
    
    /**
     * 文件改名
     * @param file 新文件对象
     * @return  >=0 表示成功,<0 失败。
     */
    int renameTo(const File& file) const;

    /**
     * 获取绝对路径名
     * @param path 绝对路径名
     * @return  >=0 表示成功,<0 失败。
     */
    int getAbsolutePath(string& path) const;
    
    /**
     * 获取规范化的路径
     * @param path 规范化的路径
     * @return  >=0 表示成功,<0 失败。
     */
    int getCanonicalPath(string& path) const;
    
    /**
     * 返回文件名
     */
    string getName() const;
    
    /**
     * 返回父文件名
     */
    string getParent() const;

    /**
     * 取得父文件对象
     */
    File getParentFile() const;
    
    /**
     * 返回当前文件对象的路径
     */
    string getPath() const;
    
    /**
     * 测试是否是绝对路径
     * @return  true 是,false 不是。
     */
    bool isAbsolute() const;

    /**
     * 测试当前文件对象是否表示一个目录
     * @return true 是,false 不是。
     */
    bool isDirectory() const;

    /**
     * 测试当前文件对象是否表示一个文件
     * @return  true 是,false 不是。
     */
    bool isFile() const;

    /**
     * 返回文件长度
     */
    size_t length() const;
    
    /**
     * 获取当前文件对象路径下的文件列表
     * @param file_list 文件列表
     * @return  >=0 表示成功,<0 失败。
     */
    int listDirectory(std::list<string> & file_list) const;
  
    /**
     * 创建新的文件
     * @return  >=0 表示成功,<0 失败。
     */
    int createNewFile() const;

    /**
     * 创建目录
     * @return  >=0 表示成功,<0 失败。
     */
    int mkdir() const;

    /**
     * 创建目录.包括父目录
     * @return  >=0 表示成功,<0 失败。
     */
    int mkdirs() const;

    /**
     * 获取最后修改时间
     * @param time 最后修改时间
     * @return  >=0 表示成功,<0 失败。
     */
    int getLastModified(time_t & time) const;

    /**
     * 设置最后修改时间
     * @param time 最后修改时间
     * @return  >=0 表示成功,<0 失败。
     */
    int setLastModified(const time_t & time) const;

    /**
     * 设置只读属性
     * @param bReadOnly 是否只读
     * @return  >=0 表示成功,<0 失败。
     */   
    int setReadOnly(bool bReadOnly) const;

    /**
     * 文件合并
     * @param file1  文件路径1
     * @param file2  文件路径2
     * @return  >=0  表示文件合并成功, <0 失败。
     */ 
    int concateFile(const string& file1, const string & file2);
    
    /**
     * 文件合并
     * @param file1  文件对象1
     * @param file2  文件对象2
     * @return  >=0  表示文件合并成功, <0 失败。
     */ 
    int concateFile(const File& file1, const File& file2);

    /**
     * 文件合并
     * @param file1  文件对象1
     * @return  >=0  表示文件合并成功, <0 失败。
     */ 
    int concateFile2(const File& file1);

    /**
     * 转储文件对象信息
     * @param os 输出流对象
     */ 
    void dump(ostream & os);

    /**
     * 返回分割字符
     */
    static char getSeparatorChar();

    /**
     * 返回分割字符串
     */
    static string getSeparator();

    /**
     * 获取当前工作目录路径
     * @param path 当前工作目录路径
     * @return  >=0 表示成功,<0 失败。
     */
    static int getCurrentDirectory(string& path);

    /**
     * 获取规范化的路径
     * @param path    源文件路径
     * @param ret_path 规范化的文件路径
     */
    static void getCanonicalPath(const string& path, string& ret_path);

  private:
    File& operator=(const File& rhs);

    static string normalizePath(const string& path);
  
  private:
    FileSystem * m_fileSystem;
    string m_path;
    size_t m_prefixLength;
};


#endif //__FILE_H__
