
#include "FTController.h"
#include "FSystemAgent.h"
#include "ControlInfo.h"
#include "Utility.h"

#include "base/StringUtil.h"
#include "base/TimeUtil.h"

#include <iostream>
#include <stdio.h>

FTController::FTController()
{
  m_local = NULL;
  m_remote = NULL;
  m_controlInfo = new ControlInfo();

  writeLogHook = NULL;
  writeAlertHook = NULL;
  writeStateHook = NULL;
}

FTController::~FTController()
{
  if (m_local != NULL) {
    delete m_local;
    m_local = NULL;
  }

  if (m_remote != NULL) {
    delete m_remote;
    m_remote = NULL;
  }

  if (m_controlInfo != NULL) {
    delete m_controlInfo;
    m_controlInfo = NULL;
  }
}

int FTController::initialize(const string& conf_file,
                             const string& section_path)
{
  if ((m_controlInfo->initialize(conf_file,section_path )) < 0) {
    writeAlert("config info initialize failed!");
    return -1;
  }
#ifdef _DEBUG_
  char buf[32*1024];
  m_controlInfo->dump(buf);
  cout << buf << endl;
#endif
  m_local = FSystemAgent::getInstance("FILE");
  m_local->initialize();

  m_remote = FSystemAgent::getInstance(m_controlInfo->protocal_type);
  if (m_remote == NULL) {
    return -1;
  }

  if (m_remote->initialize() < 0) {
    writeAlert(m_controlInfo->connect_remote + " initialize failed!");
    return -1;
  }

  writeState(string("连接")+ m_controlInfo->connect_remote);

  if (m_remote->connect(m_controlInfo->connect_remote,
                        m_controlInfo->connect_port,
                        m_controlInfo->connect_user,
                        m_controlInfo->connect_passwd) < 0) {

    writeAlert(m_controlInfo->connect_remote + " connect failed!" + StringUtil::toString(m_remote->m_errorCode) + "!" );
    m_isOk = false;
    return -1;
  }

  m_isOk = true;

  return 0;
}

int FTController::end()
{

  if (m_local != NULL) {
    delete m_local;
    m_local = NULL;
  }

  if (m_remote != NULL) {
    if (m_remote->disconnect() < 0) {
    	return -1;
    }

    if (m_remote->end() < 0) {
    	return -1;
    }

    delete m_remote;
    m_remote = NULL;
  }

  if (m_controlInfo != NULL) {
    delete m_controlInfo;
    m_controlInfo = NULL;
  }

  return 0;
}

int FTController::execute()
{
  int ret = -1;
  m_filenum = 0;

  if (m_controlInfo->link_type != "short") {
    if (m_controlInfo->action == "get" && m_isOk ) {
      ret = get();
    }
    else if (m_controlInfo->action == "put" && m_isOk ) {
      ret = put();
    }
  }

  if (ret < 0) {
    if (m_remote) {
      m_remote->disconnect();
      m_isOk = false;
    }

    writeState(string("连接")+ m_controlInfo->connect_remote);

    if (m_remote->connect(m_controlInfo->connect_remote,
                          m_controlInfo->connect_port,
                          m_controlInfo->connect_user,
                          m_controlInfo->connect_passwd) < 0)
    {
      writeAlert(m_controlInfo->connect_remote + " connect failed!");
      m_isOk = false;
      return -1;
    }
	
	m_isOk = true;
	
    if (m_controlInfo->action == "get") {
      ret = get();
    }
    else if (m_controlInfo->action == "put") {
      ret = put();
    }

  }

  return ret;
}

int FTController::get()
{
  int i,j,n;
  string filterPattern, message;
  string srcFile, srcFullPathFile, targetFile;
  string backupFile, tmpFile, distFile, sTemp;
  unsigned long srcFileSize,targetFileSize;
  char ctime[17+1];

  vector<ControlInfo_get_source *> &sources = m_controlInfo->get_sources;

  for (i = 0; i < (int)sources.size(); i++) {

    if (m_remote->changeDirectory(sources[i]->source_path) < 0) {
      writeAlert("设置路径失败: " + sources[i]->source_path);
      return -1;
    }

    if (sources[i]->list_in_source == "yes") {
      if (m_remote->listDirectory(sources[i]->source_path, sources[i]->m_queue, filterPattern) < 0)
      {
        writeAlert("文件列表失败:" + sources[i]->source_path);
        return -1;
      }
#ifdef _DEBUG_
      for (vector<string>::iterator queueIter = sources[i]->m_queue.begin();
           queueIter != sources[i]->m_queue.end(); ++queueIter) {
        cout << (*queueIter) << endl;
      }
#endif
    }
    //新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
    if (sources[i]->finish_tag_file !="")
    {
        time_t  splitTime,srcFileTime; // 标识文件修改时间
        srcFullPathFile = sources[i]->source_path + sources[i]->finish_tag_file;
        if (m_remote->getFileTime(srcFullPathFile, splitTime)== (-1))
        {
            writeAlert("标识空文件获取失败:" + sources[i]->source_path+sources[i]->finish_tag_file);
            return -1;
        }
        vector<string> tmpQueue;      
        tmpQueue.clear(); 
        for (vector<string>::iterator queueIter = sources[i]->m_queue.begin();
              queueIter != sources[i]->m_queue.end(); ++queueIter)
        {
            srcFullPathFile = sources[i]->source_path + (*queueIter);
            m_remote->getFileTime(srcFullPathFile, srcFileTime);
            if (srcFileTime<splitTime)     //如果文件时间大于标识文件时间 ,则不取
                tmpQueue.push_back(*queueIter);
        }
        sources[i]->m_queue.clear();
        for (vector<string>::iterator queueIter = tmpQueue.begin();
              queueIter != tmpQueue.end(); ++queueIter)
            sources[i]->m_queue.push_back(*queueIter);          
    }        

    vector<ControlInfo_get_source_pattern *> &patterns = sources[i]->patterns;

    for (j = 0; j < (int)patterns.size(); j++)
    {

      if (sources[i]->list_in_source == "yes") {
        //文件格式匹配
        Utility::regx(sources[i]->m_queue, patterns[j]->m_queue,
                      patterns[j]->pattern);
      } else {
          if (m_remote->listDirectory(sources[i]->source_path, patterns[j]->m_queue, patterns[j]->pattern) < 0) {
            writeAlert("文件列表失败:" + sources[i]->source_path);
            return -1;
          }
#ifdef _DEBUG_
         for (vector<string>::iterator queueIter = patterns[j]->m_queue.begin();
              queueIter != patterns[j]->m_queue.end(); ++queueIter) {
           cout << (*queueIter) << endl;
         }
#endif
      }

      if (patterns[j]->m_queue.size() > 0)
        patterns[j]->resetTime();

      for (n = 0; n < (int)patterns[j]->m_queue.size(); n++) {
        string fileSuf;  // 压缩文件名后缀
        int fileSufLength = 0; // 压缩文件名后缀长度

        srcFile = patterns[j]->m_queue[n];
        if (srcFile[0] == '#')
          continue;
        srcFullPathFile = sources[i]->source_path + srcFile;
        targetFile = patterns[j]->target_path + srcFile;

        if (!m_controlInfo->swap_path.empty())
          tmpFile = m_controlInfo->swap_path + srcFile;
        else
          tmpFile = patterns[j]->target_path + srcFile;

        //下载文件
        writeState(srcFile);

        if (m_remote->getFile(srcFullPathFile, tmpFile) < 0) {
#ifdef _DEBUG_
          int errorCode;
          string errorMsg;
          m_remote->getErrInfo(errorCode, errorMsg);
          writeAlert(errorMsg);
#endif
          writeAlert("下载文件失败: " + srcFullPathFile);
          return -1;
        }

        //得到源文件大小
        m_remote->getFileInfo(srcFullPathFile, srcFileSize);
        m_local->getFileInfo(tmpFile, targetFileSize);
        if (srcFileSize != targetFileSize) {
            writeAlert("文件大小不一致，取文件失败！" + srcFullPathFile);
            string cmd;
            cmd = "mv ";
            cmd += tmpFile;
            cmd += " ";
            cmd += tmpFile;
            cmd += "_SIZE_ERR";
            if (system(cmd.c_str()) < 0) {
              writeAlert("mv file failed: " + cmd);
            }
            return -1;          	
        }

        //解压处理
        if (!m_controlInfo->uncompress.empty()) {
          if (Utility::uncompressFile(m_controlInfo->uncompress, tmpFile,
                                      fileSufLength) < 0)
          {
            writeAlert("解压缩文件失败: " + tmpFile);
            return -1;
          }

          if (srcFileSize == 0){
            string cmd;
            cmd = "mv ";
            cmd += tmpFile;
            cmd += " ";
            cmd += tmpFile.substr(0, tmpFile.length() - fileSufLength);
            if (system(cmd.c_str()) < 0) {
              writeAlert("mv file failed: " + cmd);
              return -1;
            }
          }
          tmpFile = tmpFile.substr(0, tmpFile.length() - fileSufLength);
        }

        if (!sources[i]->change_name.empty())
        {
          if (!m_controlInfo->uncompress.empty())
            sTemp = srcFile.substr(0, srcFile.length() - fileSufLength);
          else
            sTemp = srcFile;
          sources[0]->m_name[0]=sTemp;
          TimeUtil::now(ctime);
          sources[0]->m_name[2]=ctime;
          if ((sources[i]->m_js)->matches())
          {
            writeAlert("get new file name failed: " + srcFile);
            return -1;
          }
          if (!m_controlInfo->uncompress.empty())
            sTemp = sources[0]->m_name[1] + srcFile.substr(srcFile.length() - fileSufLength, fileSufLength);
          else
            sTemp = sources[0]->m_name[1];
          sources[0]->m_name[1] = sTemp;
          targetFile = patterns[j]->target_path + sources[0]->m_name[1];
        }

        //压缩处理
        if (!m_controlInfo->compress.empty()) {
          if (Utility::compressFile(m_controlInfo->compress, tmpFile, fileSuf)
              < 0)
          {
            writeAlert("压缩文件失败: " + tmpFile);
            return -1;
          }
          tmpFile = tmpFile + fileSuf;
        }

        //分发处理
        char tmpmsgln[1024];
        string msgln="";
        if (!patterns[j]->distribute_path.empty()) {
            vector<string> dis_paths = StringUtil::split(patterns[j]->distribute_path,";");
            for (int dd = 0; dd < dis_paths.size(); ++dd) {
                distFile = dis_paths[dd] + srcFile;

                if (!sources[i]->change_name.empty())
                  distFile = dis_paths[dd] + sources[0]->m_name[1];

                if (fileSufLength != 0) {
                  distFile = distFile.substr(0, distFile.length() - fileSufLength);
                }

                if (!fileSuf.empty())
                  distFile = distFile + fileSuf;

                if (Utility::linkFile(tmpFile, distFile) < 0) {
                  writeAlert("分发文件失败: " + tmpFile);
                  return -1;
                }
                
                //增加link日志信息 2006-6-14 15:13 by liuxiang
                sprintf(tmpmsgln, " \nsource_name=%s,target_name=%s ",
                         tmpFile.c_str(),distFile.c_str());
                msgln = msgln + tmpmsgln;
            }
        }

        //swap处理
        if (!m_controlInfo->swap_path.empty()) {
          if (fileSufLength != 0) {
            targetFile = targetFile.substr(0, targetFile.length()
                                           - fileSufLength);
          }

          if (!fileSuf.empty())
            targetFile = targetFile + fileSuf;

          if (m_local->rename(tmpFile, targetFile) < 0) {
            writeAlert("移文件失败: " + tmpFile);
            return -1;
          }
          //的到本地文件大小
          m_local->getFileInfo(targetFile, targetFileSize);
        }

        //备份,删除源处理
        if (!sources[i]->backup_path.empty()) {
          backupFile = sources[i]->backup_path + srcFile;
          if (m_remote->rename(srcFullPathFile, backupFile) < 0) {
            writeAlert("备份文件失败: " + srcFullPathFile + " " + backupFile);
            return -1;
          }
        } else {
          if (m_remote->delFile(srcFullPathFile) < 0) {
            writeAlert("删除文件失败: " + srcFullPathFile);
            return -1;
          }
        }


        char tmpmsg[1024];
        sprintf(tmpmsg, "get:file_name=%s,remote=%s,size=%ld \n"
                 "file_name=%s,remote=%d%d,size=%ld",
                 (sources[i]->source_path + srcFile).c_str(),(m_controlInfo->connect_remote).c_str(),(srcFileSize),
                 targetFile.c_str(),i,j,targetFileSize);
        message = tmpmsg ;
        message = message + msgln;

        writeLog(message);
        m_filenum++;

      } // end n

      if (patterns[j]->checkTimeOut()) {
        char tmpmsg[1024];
        sprintf(tmpmsg, "太长时间(%lds)没有采到文件格式(%s)!\n", patterns[j]->check_interval, patterns[j]->pattern.c_str());
        message = tmpmsg;
        writeAlert(message);
      }
    } // end j
  } // end i

  return m_filenum;
}

int FTController::put()
{
  int i,n;
  string filterPattern, message;
  string tmpFile, targetFile, backupFile;
  string srcFile, srcFullPathFile ,swapFile;
  unsigned long srcFileSize,targetFileSize;

  vector<ControlInfo_put_source *> & sources = m_controlInfo->put_sources;

  for (i = 0; i < (int)sources.size(); i++) {

    if (m_local->changeDirectory(sources[i]->source_path) < 0) {
      writeAlert("设置路径失败: " + sources[i]->source_path);
      return -1;
    }

    //为避免连接超时，每次刷一次服务端的目录，但不告警
    if (m_remote->changeDirectory(sources[i]->target_path) < 0)
    {
//      writeAlert("设置路径错误: " + sources[i]->target_path);
      return -1;
    }

    if (m_local->listDirectory(sources[i]->source_path, sources[i]->m_queue, filterPattern) < 0)
    {
      writeAlert("文件列表失败: " + sources[i]->source_path);
      return -1;
    }

#ifdef _DEBUG_
    vector<string>::const_iterator queueIter;
    for (queueIter = sources[i]->m_queue.begin();
         queueIter != sources[i]->m_queue.end(); ++queueIter)
      cout << (*queueIter) << endl;
#endif

    if (sources[i]->m_queue.size() > 0)
      sources[i]->resetTime();

    for (n = 0; n < (int)sources[i]->m_queue.size(); n++)
    {
      string fileSuf;  // 压缩文件名后缀
      int fileSufLength=0; // 压缩文件名后缀长度

      srcFile = sources[i]->m_queue[n];
      if (srcFile[0] == '#')
        continue;
      srcFullPathFile = sources[i]->source_path + srcFile;
      tmpFile = srcFile;

      writeState(srcFile);

      //得到原始文件大小
      m_local->getFileInfo(srcFullPathFile, srcFileSize);

      //解压处理
      if (!m_controlInfo->uncompress.empty()) {
        if (Utility::uncompressFile(m_controlInfo->uncompress,
                                    srcFullPathFile, fileSufLength) < 0)
        {
          writeAlert("解压缩文件失败: " + srcFullPathFile);
          return -1;
        }

        tmpFile = tmpFile.substr(0, tmpFile.length() - fileSufLength);
        srcFullPathFile = sources[i]->source_path + tmpFile;
      }

      //压缩处理
      if (!m_controlInfo->compress.empty()) {
        if (Utility::compressFile(m_controlInfo->compress, srcFullPathFile,
                                  fileSuf) < 0)
        {
          writeAlert("压缩文件失败: " + srcFullPathFile);
          return -1;
        }
        tmpFile = tmpFile + fileSuf;
        srcFullPathFile = sources[i]->source_path + tmpFile;
      }

      //重置路径
      targetFile = sources[i]->target_path + tmpFile;
      if (!m_controlInfo->swap_path.empty()) {
        if (m_remote->changeDirectory(m_controlInfo->swap_path) < 0)
        {
          writeAlert("设置路径错误: " + m_controlInfo->swap_path);
          return -1;
        }
        swapFile = m_controlInfo->swap_path + tmpFile;
      } else {
        swapFile = targetFile;
      }

      //上传文件
      if (m_remote->putFile(srcFullPathFile, swapFile) < 0) {
#ifdef _DEBUG_
        int errorCode;
        string errorMsg;
        m_remote->getErrInfo(errorCode, errorMsg);
        writeAlert(errorMsg);
#endif
        writeAlert("上传文件失败: " + srcFullPathFile);
        return -1;
      }


      //swap处理
      if (!m_controlInfo->swap_path.empty()) {
        if (m_remote->rename(swapFile, targetFile) < 0) {
          writeAlert("移文件失败: " + swapFile);
          return -1;
        }
      }

      //取得远程文件大小
      m_remote->getFileInfo(targetFile, targetFileSize);


      //备份处理
      if (!sources[i]->backup_path.empty()) {
        backupFile = sources[i]->backup_path + tmpFile;;

        if (m_local->rename(srcFullPathFile, backupFile) < 0) {
          writeAlert("移文件失败: " + srcFullPathFile);
          return -1;
        };
      } else {
        if (m_local->delFile(srcFullPathFile) < 0) {
          writeAlert("删除文件失败: " + srcFullPathFile);
          return -1;
        }
      }

//    message = sources[i]->source_path + srcFile + "@" + tmpFile +
//              "@" + m_controlInfo->connect_remote + "@" + targetFile;

      char tmpmsg[1024];
      sprintf(tmpmsg, "put:file_name=%s,remote=%d%d,size=%ld" "\n"
               "file_name=%s,remote=%s,size=%ld" ,
               srcFullPathFile.c_str(),i,0,srcFileSize,
               (targetFile).c_str(),(m_controlInfo->connect_remote).c_str(),(srcFileSize)
               );

      message = tmpmsg;
      writeLog(message);
      m_filenum++;

#ifdef _DEBUG_
      cout << message << endl;
#endif
    } // end n

    if (sources[i]->checkTimeOut()) {
      writeAlert(sources[i]->source_path + " 太长时间没有文件" );
    }
  } // end i

  return m_filenum;
}

int FTController::writeLog(const string & msg) const
{
  if ( writeLogHook != NULL ) {
    return (*writeLogHook)(msg);
  }

  return -1;
}

int FTController::writeAlert(const string & msg) const
{
  if ( writeAlertHook != NULL ) {
    return (*writeAlertHook)(msg);
  }

  return -1;
}

int FTController::writeState(const string & msg) const
{
  if ( writeStateHook != NULL ) {
    return (*writeStateHook)(msg);
  }

  return -1;
}
