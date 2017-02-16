#ifndef __UTILITY_H__
#define __UTILITY_H__


/**
 * \class Utility
 * \brief 提供一些工具函数
 */

using namespace std;

#include <string>
#include <vector>

class Utility
{
  public:
    Utility(){};

    static void regx(vector<string> & sources, 
                     vector<string> & results, const string & pattern);  

    static int renameFile(const string & old_file, 
                          const string & new_file);
    static int linkFile(const string & old_file, const string & new_file);

    static int unLinkFile(const string & file);
    
    static int compressFile(const string & cmd, const string & file, 
                            string & suffix);

    static int uncompressFile(const string & cmd, const string & file, 
                              int & suffix_length);

  private:
    static bool matchFile(const char *file, const char *pattern);
    static bool exits(const string & file);
};

#endif //__UTILITY_H__
