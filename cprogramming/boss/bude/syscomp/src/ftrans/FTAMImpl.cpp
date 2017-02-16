
#include "FTAMImpl.h"


FTAMImpl::FTAMImpl()
{ 
}

FTAMImpl::~FTAMImpl()
{
}

int FTAMImpl::initialize()
{
  return 0;
}

int FTAMImpl::connect(const string & host, const string & port,
                      const string & user, const string & passwd)
{
  return 0;
}

int FTAMImpl::disconnect()
{
  return 0;
}

int FTAMImpl::getFile(const string & source, const string & dest)
{
  return 0;
}

int FTAMImpl::putFile(const string & dest, const string & source)
{
  return 0;
}

int FTAMImpl::setMode(const int & mode)
{
  return 0;
}

int FTAMImpl::setOption(const int & type, const int & value)
{
  return 0;
}

int FTAMImpl::changeDirectory(const string & dir)
{
  return 0;
}

int FTAMImpl::listDirectory(const string & dir, vector<string> & files,
                            const string & pattern)
{
  return 0;
}

int FTAMImpl::getFileInfo(const string & file, unsigned long & size)
{
  return 0;
}

int FTAMImpl::delFile(const string & file)
{
  return 0;
}

int FTAMImpl::rename(const string & old_file, const string & new_file)
{
  return 0;
}

int FTAMImpl::end()
{
  return 0;
}

void FTAMImpl::getErrInfo(int & code, string & msg)
{
}
