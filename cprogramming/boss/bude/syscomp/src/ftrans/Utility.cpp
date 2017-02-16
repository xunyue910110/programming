
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "Utility.h"
#include "base/PatternMatcher.h"

void Utility::regx( vector<string> & sources, 
                   vector<string> & results, const string & pattern) {
  vector<string>::iterator sourceIter;

  results.clear();

  for (sourceIter = sources.begin();
       sourceIter != sources.end(); ++sourceIter)  {
    
    if ((*sourceIter) == ".")
        continue; 

    if (  (pattern.empty()) 
        || matchFile(sourceIter->c_str(), pattern.c_str()) )
    {
      results.push_back(*sourceIter);
      (*sourceIter) = ".";
    }
  }




}

int Utility::renameFile(const string & old_file,const string & new_file)
{ 
  if ( ::rename(old_file.c_str(), new_file.c_str()) < 0 )
    return -1;
    
  unLinkFile(old_file);
  
  return 0;
}

int Utility::linkFile(const string & old_file, const string & new_file)
{
  unLinkFile(new_file);
  
  if ( ::link(old_file.c_str(), new_file.c_str()) < 0 )
    return -1;
  
  return 0;
}

int Utility::unLinkFile(const string & file)
{
  if ( ::unlink(file.c_str()) < 0 )
    return -1;

  return 0;
}

int Utility::compressFile(const string & cmd, const string & file, 
                          string & suffix)
{
  string systemCmd;
  struct stat fileStat;

  suffix = "";  
  if (::stat(file.c_str(), &fileStat) < 0) {
      return -1;
  }
  
  if (fileStat.st_size == 0) {
    return 0;
  }
          
  if (cmd == "compress") {          
    systemCmd = cmd + " -f " + file + " > /dev/null";
    if (system(systemCmd.c_str()) < 0) {
      return -1;          
    }
    suffix = ".Z";
    if (! exits(file + suffix)) {
      suffix = "";
    }
    
  } else if (cmd == "gzip") { 
    systemCmd = cmd + " -f " + file + " > /dev/null";                    
    if (system(systemCmd.c_str()) < 0) {
      return -1;              
    }
    suffix = ".gz";
    if (! exits(file + suffix)) {
      suffix = "";
    }

  } else {
    // 其它压缩方式
    ;
  }

  return 0;
}

int Utility::uncompressFile(const string & cmd, const string & file, 
                            int & suffix_length)
{
  string systemCmd;
  string suffix;
  
  suffix_length = 0;
  if (cmd == "uncompress") {
    suffix = file.substr(file.size() - 2);
    if (suffix != ".Z") {
      return 0;
    }
    
    systemCmd = cmd + " -f " + file + " > /dev/null";
    if (system(systemCmd.c_str()) < 0) {
      return -1;              
    }
    suffix_length = 2;
    
  } else if (cmd == "gunzip") {
    suffix = file.substr(file.size() - 3);
    if (suffix != ".gz") {
      return 0;
    }
        
    systemCmd = cmd + " -f " + file + " > /dev/null";                    
    if (system(systemCmd.c_str()) < 0) {
      return -1;              
    }
    suffix_length = 3;
    
  } else {
    // 其它解压缩方式
    ;
  }

  return 0; 
}


bool Utility::matchFile(const char *file, const char *pattern)  
{
    return PatternMatcher::matches(pattern, file);
}

bool Utility::exits(const string & file)
{
  if ( ::access(file.c_str(), F_OK) < 0 ) {
    return false;
  }

  return true;
}
