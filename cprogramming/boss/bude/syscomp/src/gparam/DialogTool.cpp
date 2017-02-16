

#include "util/Util.h"
#include "DialogTool.h"
#include "Manager.h"

#include <stdarg.h>
#include <string>
#include <stdio.h>

void DialogTool::input(char const* prompt, char* buf, size_t buf_size)
{
    char* p;
    
    do 
    {
        if( m_contex.size()>0 )
             printf("%s",m_contex.c_str());
        printf("%s",prompt);
        *buf = '\0';
        fgets(buf, buf_size, stdin);
        p = buf + strlen(buf);
    } while (p <= buf+1);

    if (*(p-1) == '\n') {
        *--p = '\0';
    }
}

DialogTool::DialogTool(Manager * gparam)
{
    m_contex="";
    if(gparam==0)
        cout<<" DialogTool:gparam=0 !"<<endl;
    m_gparam = gparam;
    m_needDestory = false;
}

DialogTool::DialogTool()
{
    m_contex="";
    m_gparam = Manager::GetInstance("Client");
    m_needDestory = true;
}



DialogTool::~DialogTool()
{
	if (m_needDestory)  {
        if (m_gparam != NULL) 
            m_gparam->disconnect(); 
        delete m_gparam ;
        m_gparam = NULL ;
    }
}

char* DialogTool::dump(char* buf) 
{
  buf += sprintf(buf, "\n");
  buf += sprintf(buf, "\n");
  return buf;
}

void DialogTool::dialog() 
{ 
  	char buf[32*1024];
  	string buf_tmp;
    
      while (true) 
      {
  	  	input("> ", buf, sizeof buf);
            
            buf_tmp=lrtrim(buf);
            strcpy(buf,buf_tmp.c_str());
            
            if ( strcmp(buf, "exit") == 0 || strcmp(buf, "quit") == 0 ) { 
                break;
            } else if (strcmp(buf, "help") == 0 || strcmp(buf, "?") == 0) { 
                help();
            } else if (strcmp(buf, "..") == 0) {
                m_contex = "";  
            } else if (strcmp(buf, "server") == 0) {
                       m_contex = "server";        
            } else if (strcmp(buf, "data") == 0) {
                   m_contex = "data";        
            } else if (strncmp(buf, "connect",7) == 0) {
                   connect(buf);                                               
            } else if (strcmp(buf, "disconnect") == 0) {
                   m_gparam->disconnect();      
            } else if ((strcmp(buf, "close") == 0) || (strcmp(buf, "disconnect") == 0)) {
                 // close the connection 
            } else if (strncmp(buf, "open",4) == 0) {
                   // open the socket and estabilish a connection to sever
  	  	} else if (m_contex != "") {
                handleRequest(buf);
            } else { 
                   printf("下列命令没有找到: \n%s\n\n",buf);
            }
      }
}



void DialogTool::help() 
{
	char * msg = (char *)"\n"
    " 下列指令有效:\n"
    "\n"
      " 全局命令:\n"
      " ..                    - 移到上一层上下文级。\n"
      " ?                     - 显示命令列表。\n"
      " help                  - 显示命令列表。\n"      
      " connect [IP] [port]   - 连接服务器。\n"
      " data                  - 转换到数据查询状态。\n"
      " disconnect            - 断开服务器连接。\n"
      " exit                  - 退出程序。\n"
      " quit                  - 退出程序。\n"
      " server                - 转换到服务器状态。\n"    
      " set                   - 更新设置。\n"   
      "\n"
    ;
    
	char * msg_data = (char *)"\n"
    "此上下文中命令：\n"
    " find [..]              - 参数查询命令。\n"
    " dump [table_name]      - 输出参数表记录到文件。\n"
    " gettable               - 参数表查询命令。\n"
    " updata                 - 参数更新命令。\n"
    "\n"
    ;
    
	char * msg_server = (char *)"\n"
    "此上下文中的命令:\n"
    " show                   - 显示信息命令。\n"
    " stop                   - 停止服务命令。\n"
    " start_listener         - 启动侦听命令。\n"
    " stop_listener          - 停止侦听命令。\n"
    "\n"
    ;
    
    trace(msg);
    
    if(m_contex.compare("data")==0) {
        trace(msg_data);
    }
    if(m_contex.compare("server")==0) {
        trace(msg_server);
    }
    
}     
  
void DialogTool::trace(char const* fmt, ...)
{ 
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
} 

void DialogTool::connect(const char *buf)
{
    char *p;
    int port;
    string ip;
    char tmp[1024];
    
    strcpy(tmp,buf);
    p=strtok(tmp," ");  
    if(p==NULL) {
        printf("命令错误： %s \n",buf);      
        return;
    }
          
    p=strtok(NULL," ");
    if(p==NULL) {
         printf("命令错误： %s \n",buf);    
         return ;
      }
    ip=p;
    
    p=strtok(NULL," ");
    if(p==NULL) {
         printf("命令错误： %s \n",buf);    
         return ;
    }
         
    cout<<"ip = "<<ip<<", port = "<<p<<endl;
    cout<<"\n请稍候..."<<flush;
    if( m_gparam->connect(ip.c_str(),atoi(p)) == 0) {
        m_contex = "data";  
        printf("服务器连接成功！\n\n");
    } else     {
        printf("服务器连接失败( %s )\n\n",buf);       
    }            
}

void DialogTool::handleRequest(const char *buf)
{
    int n;
    char *p;    
    string result;
    char tmp[32*1024];
    char tmp_1[32*1024];

    if(m_contex.compare("data")==0) {
        tmp_1[0]=0;
        strcpy(tmp,buf);
		tmp[strlen(buf)]=0;
        for( p=strtok(tmp," "); p!=NULL; p=strtok(NULL," ") ) 
        {
            strcat(tmp_1,p);
            strcat(tmp_1,",");
        }
        tmp_1[strlen(tmp_1)-1]=0;
        
        if( strlen(tmp_1)<4 ) {
            printf(" 命令有错误！%s\n\n",tmp_1);
            return;
        }
            
        if( strncmp(tmp_1,"dump",4) == 0) {            
            dataDump(tmp_1);    // dump参数表记录
            return;
        }
        
        if( strcmp(tmp_1,"find")==0 ) {
            strcpy(tmp_1,"getquery");
        }
		if( strcmp(buf,"updata")==0 ) {
			strcpy(tmp_1,"updata");
		}
		if( strncmp(tmp_1,"find,",5)==0) {			
			n = m_gparam->handleRequest(m_contex + ":" + tmp_1,tmp);	
			result = tmp;
		} else {
        n = m_gparam->handleRequest(m_contex + ":" + tmp_1,result);    
		}
    } else {
        n = m_gparam->handleRequest(m_contex + ":" + buf,result);    
    }    

    if( n >= 0 ) {
        if( result.compare(0,9,"error:-10") == 0 ) {
            p = (char *)result.c_str() + 9;
            printf("\n %s\n\n",p);
        } else if( result.size() == 2 ) {
            printf("\n 没有找到！\n\n");
        } else {
            printf("\n%s\n\n",result.c_str());
        }
    } else {
        printf(" 有错误！\n\n");
    }                 
}

void DialogTool::dataDump(char* buf)
{
    int count;
    char tmp[1024];
    string result,name;
    vector<string> tables;
    
    m_gparam->handleRequest(m_contex + ":" + buf,result);
    
    cout<<"x = "<<m_contex + ":" + buf<<endl;
    
    if(strcmp(buf,"dump")==0) {
        split(",",result.c_str(),true,tables);
        for(int i=0;i<tables.size();i++)
        {
            printf("dump %s\n",tables[i].c_str());
        }
        printf("\n");
        return;
    }
    
    if( strlen(buf) < 5 ) {
        printf(" 有错误！\n\n",buf);
        return ;
    }
    
    cout<<"result = "<<endl;
    
    count = atoi(result.c_str());
    
    if( result.compare(0,8,"error:-1") == 0 ) {
        printf(" 表%s不存在！\n\n",buf+5);
        return;
    }
    if ( count == 0 ){
        printf(" 表%s记录数是 0 ！\n\n",buf+5);
        return;
    }
    
    while(1)
    {
        cout<<"\n  文件名: "<<buf+5<<".txt, 记录数为: "<<count<<endl<<endl;
        input("> 请输入参数表存放路径: ", tmp, sizeof tmp);
        name = string(tmp)  ;
        checkPath(name);
        name += string(buf+5) + ".txt";
        
        cout<<"\n  文件名: "<<name<<endl<<endl;
        input("> 正确吗(Yes/No/Quit)? ", tmp, sizeof tmp);
        if( tmp[0]=='y' || tmp[0]=='Y' ) {
            break;
        }
        if( tmp[0]=='q' || tmp[0]=='Q' ) {
            return;
        } 
    }
        
    ofstream df;
    df.open(name.c_str(),ios::out);
    if (df.rdstate() != ios::goodbit)
    {
        cout<<" 文件"<<name<<"打开失败!"<<endl;
        return ;
    }
    
    // 获取参数表记录数
    
    int i,j;
    double x;
    char tt[80];
    for(i=count,j=1; i>0; i--,j++)
    {
        x = (j*100/count);
		sprintf(tmp,"dump,getCdr,,%d",j-1);
        sprintf(tt,"    正在执行, 已完成 %5.1f%% 。",x);         
        cout<<"\r"<<tt<<flush;
        m_gparam->handleRequest(m_contex + ":" + tmp,result);
        
        df<<result;
    }
    df.close();
    printf("成功!\n\n");
}

void  thread_proc DialogTool::acceptThread(void* arg)
{
    ((DialogTool*)arg)->dialog();
}
 

int DialogTool::start(const bool & join_thread)
{
    m_thread.start(&acceptThread, this);
    
    if (join_thread)
      m_thread.join();
      
    return 0;
}


int DialogTool::stop()
{
    return 0;
}

