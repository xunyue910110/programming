
#ifndef __FILEREADER_H__
#define __FILEREADER_H__

#include "config-all.h"
#include "file/File.h"

#include <fstream>
#include <string>
#include <vector>


USING_NAMESPACE(std)

static const int MAX_READ_BUFFER_SIZE = 1024 * 2 +1;

class  FileAccessor {
  public:
    FileAccessor();
    virtual ~FileAccessor();

  public:
    string    m_name;
    string    m_path;

    string    m_fieldDelimiter;    
    string    m_lineDelimiter; 

    string    m_fullName;
    fstream   m_fstream;
    int       m_flag;

    char      m_buffer[MAX_READ_BUFFER_SIZE+1];
    vector<string> m_array;

  public:
    string  getName();
    void  setName(const string &fileName,const string &path,
        const string &field_delimiter = ",",const string &line_delimiter ="\n");

    bool open(int mode=ios::in);
    bool is_open();

    void close();
    int read(string &str);
    int read(string &str,const int &length);
    int write(const string &data);
    int write(const char * data);
};

#endif
