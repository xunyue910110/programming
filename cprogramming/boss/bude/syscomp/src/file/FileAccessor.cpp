

#include "FileAccessor.h"
#include "base/StringUtil.h"


FileAccessor::FileAccessor() {
    m_flag = 0;
    m_fieldDelimiter = ",";    
    m_lineDelimiter = "\n"; 
}


FileAccessor::~FileAccessor() {
}


string FileAccessor::getName() {
    return m_name;
}

void FileAccessor::setName(const string &fileName,const string &path,
    const string &fieldDelimiter ,const string &lineDelimiter )  {
    m_name = fileName;
    m_path = path;
    m_fieldDelimiter = fieldDelimiter;
    m_lineDelimiter  = lineDelimiter ;

    if (strlen(path.c_str()) > 0) {
        if (path.c_str()[path.size() - 1]!='/')
            m_path+='/'; 
        m_fullName = m_path + m_name;
    } else {
        m_fullName ="/" + m_name;
    }
    
}

bool FileAccessor::open(int mode) {
  if (m_flag==1)
      return true;

  m_fstream.open(m_fullName.c_str(), mode);
  if (!m_fstream)
      return false;

  m_flag = 1;
  return true;
}


bool FileAccessor::is_open() {
  return (m_flag == 1);
}

void FileAccessor::close() {
  if (m_flag == 1) {
      m_fstream.close();
      m_fstream.clear();
      m_flag = 0;
  }
}


int FileAccessor::read(string &str)
{
  return read(str,MAX_READ_BUFFER_SIZE);
}

int FileAccessor::read(string &str,const int &length) {
  int  len;

  if (m_flag == 0) {
    if (!open(ios::in))
        return -1;
  }

  m_array.clear();
  memset(m_buffer,0,MAX_READ_BUFFER_SIZE);
  str ="";

  if(!m_fstream.eof()) {
      m_fstream.getline(m_buffer,length,m_lineDelimiter[0]);
      m_buffer[MAX_READ_BUFFER_SIZE] = '\0';
      len = strlen(m_buffer);
      if( len== 0) 
         return 0;
      str.erase();
      str= m_buffer;
      StringUtil::split(str,m_fieldDelimiter,m_array,true);
      return 1;
  }
  else
      return 0;
}


int FileAccessor::write(const string &data) {
    return write(data.c_str());
}


int FileAccessor::write(const char *data) {
  if (m_flag==0) {
    if (!open(ios::out))
        return -1;
  }

  m_fstream << data;

  return 0;
}

