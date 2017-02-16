#include "config-all.h"
#include <fstream>
#include <iostream>
//#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
//#include <locale.h>
#include <fnmatch.h>
//#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>

USING_NAMESPACE(std);

struct ArgvParam
{
    bool bReverse;//反向转换
    bool bRecursive;//目录递归扫描
    //vector<string> vectFileList;//文件列表
    vector<string> vectPatternList;//需要匹配处理的文件模式,如*.cpp,*.h,[mM]akefile
};

//path最后带'/'
int ProcFileU2D(const string& path,const string& filename)
{
    if (filename.length() == 0)
        return 2;//文件未改变

    ifstream  ifs((path+filename).c_str(), ios::in|ios::binary);
    if (!ifs)
    {
        cerr<<"File Open Error:"<<path+filename<<endl;
        return 0;
    }

    string tmpFile = path+"." + filename ;
    ofstream ofs(tmpFile.c_str(), ios::out|ios::binary);
    char tmp = ifs.get();
    char ptmp;
    bool isModify=false;
    if (!ifs.eof())
    {
        do
        {
            ptmp = tmp;
            tmp = ifs.get();

            if (ifs.eof())//文件结束，最后如果出现0D，则补一个0A
            {
                if (ptmp == 0x0D)
                {
                    tmp=0x0A;
                    isModify=true;
                    ofs << tmp;
                }
                else
                    ofs << ptmp;
                break;
            }
            if (tmp == 0x0A)
            {
                if (ptmp != 0x0D)
                {
                    ofs << ptmp;
                    ofs.put(0x0D) ;
                    isModify=true;
                }
                else//已经有一个0D了
                    ofs.put(ptmp) ;
            }
            else
                ofs.put(ptmp) ;
        } while ( 1 );
    }
    ifs.close();ifs.clear();
    ofs.close();ofs.clear();
    if(isModify)
    {
        if(rename(tmpFile.c_str(),(path+filename).c_str()) != 0)
        {
            cerr<<"Rename File Error: rename "<<tmpFile<<" to "<<path+filename<<endl;
            return 0;
        }
        return 1;
    }
    else
    {
        if(unlink(tmpFile.c_str()) != 0)
        {
            cerr<<"Delete File Error: unlink "<<tmpFile<<endl;
            return 0;
        }
        return 2;
    }
}
int ProcFileD2U(const string& path,const string& filename)
{
    if (filename.length() == 0)
        return 2;

    ifstream  ifs((path+filename).c_str(), ios::in|ios::binary);
    if (!ifs)
    {
        cerr<<"File Open Error:"<<path+filename<<endl;
        return 0;
    }

    string tmpFile = path + "." + filename ;
    ofstream ofs(tmpFile.c_str(), ios::out|ios::binary);
    char tmp;
    bool isModify=false;
    while (1)
    {
        tmp = ifs.get();
        if (ifs.eof())
            break;
        if (tmp == 0x0D)
        {
            isModify=true;
            continue;
        }
        else ofs<<tmp;
    }
    ifs.close();ifs.clear();
    ofs.close();ofs.clear();
    if(isModify)
    {
        if(rename(tmpFile.c_str(),(path+filename).c_str()) != 0)
        {
            cerr<<"Rename File Error: rename "<<tmpFile<<" to "<<path+filename<<endl;
            return 0;
        }
        return 1;
    }
    else
    {
        if(unlink(tmpFile.c_str()) != 0)
        {
            cerr<<"Delete File Error: unlink "<<tmpFile<<endl;
            return 0;
        }
        return 2;
    }
}
void Usage()
{
    printf("dos2unix v1.0\n");
    printf("    Transform Dos Text File Into Unix Format And Modify Origin File Content!\n\n");
    printf("Usage: dos2unix [-d|-u] [-r] [-p {[\"pattern1\"] [\"pattern2\"] ...}] {{-f} {file1} [file2]... }\n");
    printf("Options:\n");
    printf("    -u                        Transform Dos TextFile into Unix Format! This Is Default Option! \n");
    printf("    -d                        Transform Unix TextFile into Dos Format!\n");
    printf("    -r                        Scan SubDirectory Recursive!\n");
    printf("    -p {[\"pattern1\"] [\"pattern2\"] ...}  \n"
           "                              File Pattern String(such as \"*.cpp\" \"*.h\" \"[Mm]akefile\" ...),Default is -p \"*\" !\n");
    printf("    -f {[file1] [file2] ...}  File List!\n");
    printf("    -h                        Print This Help Information!\n");
    printf("    -?                        Same To -h!\n");
    printf("Example:\n");
    printf("    1. Transform testdata.txt into UNIX Format.\n");
    printf("        dos2unix -f testdata.txt\n");
    printf("    2. Transform testdata1.txt testdata2.txt into Dos Format.\n");
    printf("        dos2unix -d -f testdata1.txt testdata2.txt\n");
    printf("    3. Transform the diretory 'mydir' into Dos Format Recursively only for *.cpp and *h.\n");
    printf("        dos2unix -d -r -p \"*.cpp\" \"*.h\" -f testdata1.txt testdata2.txt\n");
}

int doAllFile(const vector<string>& vectFileList,const struct ArgvParam& argvParam,bool& isHasMatched)
{
    string path,filename;
    bool ret=true,isMatched=false;
    int i,retint,j;

    if(vectFileList.size()==0)
    {
        return 0;
    }
    DIR     *dir;
    struct dirent    entry;
    struct dirent    *entryP;
    struct stat myStat;

    for(i=0; i < vectFileList.size();i++)
    {
        const string& pstr=vectFileList[i];
        //判断是否是目录
        if(stat(pstr.c_str(), &myStat) != 0)
        {
            cerr<<"Get File Status Error:"<<pstr<<endl;
            continue;
        }
        if(S_ISDIR(myStat.st_mode))//目录
        {
            if(!argvParam.bRecursive)
                continue;
            if ((dir = opendir(pstr.c_str())) == NULL)
            {
                cerr<<"Open Dir Error:"<<pstr<<endl;
                continue;
            }
            vector<string> subFileList;
            entryP=NULL;
            while ((retint = readdir_r(dir,&entry,&entryP)) == 0)
            {
                if(entryP == NULL)break;
                if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0)
                  // ignore dot and dot-dot
                    continue;
                subFileList.clear();//读取一个文件处理一个文件
                subFileList.push_back(pstr+"/"+entry.d_name);
                doAllFile(subFileList,argvParam,isHasMatched);
            }
            closedir(dir) ;
            continue;
        }
        //路径分解
        char*p=strrchr((char*)(pstr.c_str()),'/');
        if(p == NULL)
        {
            path="./";
            filename=pstr;
        }
        else
        {
            path=pstr.substr(0,(string::size_type)(p-pstr.c_str())+1);
            filename=p+1;
        }
        //文件模式匹配
        isMatched=false;
        for(j = 0;isMatched==false&&j < argvParam.vectPatternList.size();j++)
        {
            //cout<<argvParam.vectPatternList[j]<<endl;
            retint = fnmatch(argvParam.vectPatternList[j].c_str(), filename.c_str(), FNM_PATHNAME);
            if(retint == 0)
            {
                isMatched=true;
            }
            else if(retint == FNM_NOMATCH)
            {
                continue ;
            }
            else
            {
                cerr<<"File Match Error: pathname="<<path+filename<<" pattern="<<argvParam.vectPatternList[j]<<endl;
            }
        }
        if(argvParam.vectPatternList.size() == 0)//没有模式时默认全部处理
            isMatched=true;
        if(!isMatched)
        {
            continue;
        }
        isHasMatched=true;
        //转换文件格式
        cout<<""<<path+filename;
        if (argvParam.bReverse)
            retint = ProcFileU2D(path,filename);
        else
            retint = ProcFileD2U(path,filename);
        if(retint==0)
        {
            cout<<" ... Failure!"<<endl;
            ret=false;
            break;
        }
        else if(retint==2)
            cout<<" ... Unchanged!"<<endl;
        else
            cout<<" ... Changed!"<<endl;
    }
    if(ret)
    {
        return 0;
    }
    else
        return -1;
}
int main(int argc, char **argv)
{

    if (argc == 1)
    {
        Usage();
        return 0;
    }
    struct ArgvParam argvParam;
    string path,filename;
    bool ret=true;
    int i;
    vector<string> vectFileList;

    argvParam.bReverse=false;
    argvParam.bRecursive=false;
    argvParam.vectPatternList.clear();
    vectFileList.clear();
    //处理参数信息，获取文件列表和模式列表
    for(i=1; i<argc;i++)
    {
        if(strcmp(argv[i],"-d") == 0)
            argvParam.bReverse = true;
        else if(strcmp(argv[i],"-u") == 0)
        {
            ;
        }
        else if(strcmp(argv[i],"-r") == 0)
            argvParam.bRecursive = true;
        else if(strcmp(argv[i],"-p") == 0)
        {
            i++;
            for(;i<argc;i++)
            {
                if(argv[i][0] == '-')
                {
                    i--;
                    break;
                }
                else
                    argvParam.vectPatternList.push_back(argv[i]);
            }
        }
        else if(strcmp(argv[i],"-f") == 0)
        {
            i++;
            for(;i<argc;i++)
            {
                if(argv[i][0] == '-')
                {
                    i--;
                    break;
                }
                else
                {
                    string tmp;
                    if(argv[i][strlen(argv[i])-1] == '/')
                        argv[i][strlen(argv[i])-1] = 0;
                    //if(strcmp(argv[i],".") == 0||strcmp(argv[i],"..") == 0)
                    //tmp=string("")+argv[i]+"/*";
                    //else
                        tmp=argv[i];
                    vectFileList.push_back(tmp);
                }
            }
        }
        else if(strcmp(argv[i],"-?") == 0 || strcmp(argv[i],"-h") == 0)
        {
            Usage();
            return 0;
        }
        else
        {
            cerr<<"Param Error:"<<argv[i]<<endl;
            Usage();
            return 0;
        }
    }
    if(vectFileList.size()==0)
    {
        cerr<<"Error: No File To Do!"<<endl;
        Usage();
        return 0;
    }
    //处理文件
    bool isHasMatched=false;
    ret=doAllFile(vectFileList,argvParam,isHasMatched);
    if(ret == 0)
    {
        if(!isHasMatched)
            cout<<"No Matched Files!"<<endl;
        return 0;
    }
    else
        return -1;
}



