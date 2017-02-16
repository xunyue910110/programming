#include <termios.h>
#include <string>
#include <vector>
#include "common.h"
#include "MdbDmlSql.h"
#include "functions.h"

using namespace std;

//将一个字符串回车替换为空格
//将一个字符串中连续的多个空格替换为单个空格
//trim，去除两头的空格 
//将一个字符串根据空格分隔一个vector 
//发现子字符串 


//
// add by yih@
// 修正BUG: 修正SQL语句在分号 ; 后键入空格再回车的情况下识别不了的错误

const int GET_STR_LEN =20000;
const int ARROW_UP_KEY = 1000;
const int ARROW_DN_KEY = 1001;
const int ARROW_RIGHT_KEY = 1002;
const int ARROW_LEFT_KEY = 1003;
const int RETURN_KEY     = 10;
const int BACKSPACE_KEY  = 8;

int getch (void)
{
    int ch;
    struct termios oldt, newt;
  
    tcgetattr(STDIN_FILENO, &oldt);
    memcpy(&newt, &oldt, sizeof(newt));
    newt.c_lflag &= ~( ECHO | ICANON | ECHOE | ECHOK | 
                       ECHONL | ECHOPRT | ECHOKE | ICRNL);
    newt.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    if(ch==27)
    {
    	ch = getchar();
    	if(ch == 91)
    	{
    		ch = getchar();
    		switch(ch)
    		{
    		case 65:
    			ch = ARROW_UP_KEY;
    			break;
    		case 66:
    			ch = ARROW_DN_KEY;
    			break;
    		case 67:
    			ch = ARROW_RIGHT_KEY;
    			break;
    		case 68:
    			ch = ARROW_LEFT_KEY;
    			break;
    		}
    			
    	}	
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}


int main(int argc, char* argv[])
{
    init();
    
    char getStr[GET_STR_LEN]; //每次取到的输入
    char commandStr[GET_STR_LEN]; //总的输入命令 
	vector<string> vCommands;
	vCommands.clear();
	int iCommandSeq = 0;
     
    memset(commandStr, 0, sizeof(commandStr));
    
    bool isCompleteCmd = true;
    
    while(g_run)
    {
        //////////////////////////////////////////////////////////
        //命令获取 
        //////////////////////////////////////////////////////////
        
        if(isCompleteCmd)
        {
            //清空上次取得的命令  
            memset(commandStr, 0, sizeof(commandStr));
            //全新的命令以command开头 
            printf("command>");
        }
        else
        {
            //之前有未结束命令，本行不提示新的command 
            printf("       >");
        }        
        
        //接收输入的字符串 
        //fgets(getStr,GET_STR_LEN,stdin);   
        int cInput,i=0;
        bool isBreak = false;
    	memset(getStr, 0, sizeof(getStr));
        while(1)
        {
        	cInput = getch();
        	switch(cInput)
        	{
        	case ARROW_UP_KEY:
        		if(vCommands.size()!=0 
        			&& iCommandSeq>=0
        			&& iCommandSeq<=vCommands.size())
        		{
	        		if( iCommandSeq!=0 )
	        			--iCommandSeq;      
	        		
	        		strcpy(getStr,vCommands[iCommandSeq].c_str());
	        		i = vCommands[iCommandSeq].length();
	        					
	        		printf("\rcommand>%s",getStr);
	        		printf("%c7",27); // save cursor position 
	        		printf("                                                      ");
	        		printf("%c8",27); // restore cursor position 
	        	}
        		break;
        	case ARROW_DN_KEY:
        		if(vCommands.size()!=0 
        			&& iCommandSeq>=0)
        		{
	        		if(iCommandSeq<vCommands.size())
	        		{
	        			strcpy(getStr,vCommands[iCommandSeq].c_str());
	        			i = vCommands[iCommandSeq].length();
	        			++iCommandSeq;      		
	        		
		        		printf("\rcommand>%s",getStr);
		        		printf("%c7",27); // save cursor position 
		        		printf("                                                      ");
		        		printf("%c8",27); // restore cursor position 
		        	}
	        		else if(iCommandSeq == vCommands.size() )
		        	{
		        		//全新的命令以command开头 
	            		printf("\rcommand>");
	            		printf("%c7",27); // save cursor position 
		        		printf("                                                      ");
		        		printf("%c8",27); // restore cursor position 
	            		memset(getStr, 0, sizeof(getStr));
	            		i = 0;
	            	}
	        	}	        	
        		break;
        	case ARROW_RIGHT_KEY:
        		if(i<strlen(getStr))
        		{
        			printf("%c",getStr[i]); 
        			++i;
        		}
        		break;
        	case ARROW_LEFT_KEY:
        		if(i>0)
        		{
        			printf("\b"); 
        			--i;
        		}
        		break;
        	case RETURN_KEY:
        		printf("\n");
        		isBreak = true;
        		break;	
        	case BACKSPACE_KEY:
        		if(i>0)
        		{
        			printf("\b \b"); 
        			// 如果是一行的最末字符被删除
        			if( i == strlen(getStr)-1 )
        			{
        				--i;
        				getStr[i] = 0;
        			}
        			else //如果不是
        			{
        				char cTmp[GET_STR_LEN];
        				memset(cTmp,0,GET_STR_LEN);
        				
        				// 屏幕显示部分        				
        				printf("%c7",27); // save cursor position 
        				printf("%s",getStr+i);	
		        		printf(" ");
		        		printf("%c8",27); // restore cursor position 
		        		// 后台命令行存储部分
		        		strcpy(cTmp,getStr+i);
		        		getStr[strlen(getStr)] = 0;
		        		--i;
		        		strcpy(getStr+i,cTmp);
        			}
        		}
        		break;
        	default:
        		// 如果是重新编辑的指令
        		if(i<strlen(getStr))
        		{
            		char cTmp[GET_STR_LEN];
        			memset(cTmp,0,GET_STR_LEN);
        			
        			// 屏幕显示部分        				
        			printf("%c",cInput);
        			printf("%c7",27); // save cursor position 
        			printf("%s                             ",getStr+i);	
		        	printf("%c8",27); // restore cursor position 
		        	// 后台命令行存储部分
		        	strcpy(cTmp,getStr+i);
		        	getStr[i] = cInput;
        			strcpy(getStr+i+1,cTmp);    			
        			getStr[strlen(getStr)] = 0;
		        }
        		else //如果是全新输入的指令
        		{
        			getStr[i] = cInput;
        			printf("%c",cInput);
        		}        		
        		++i;
        		break;	
        	}
        	if(isBreak)
        		break;	
        }     
        
        //for(int i = 0; i<255; i++)
        //{
        //    if(getStr[i]=='\0')
        //    {
        //        break;
        //    }       
        //    cout << (int)getStr[i] << ",";
        //}
        //
        ////cout << "|" << getStr << "|" << endl;
        //exit(0);
        //将最后的回车符变成'\0' 
        //getStr[strlen(getStr)-1]='\0'; 
        
        
        //直接回车，没有输入 
        if(strlen(getStr)==0) 
        {
            //之前也什么都没有输入的情况下，可以认为重新开始 
            if(strlen(commandStr)==0)
            {
                isCompleteCmd = true;
            }    
            else
            {
                ;//isCompleteCmd = false;
            }    
            continue;
        }    
        
        //将本次获取的输入行，连接到前面已经输入的命令之后 
        strncat(commandStr, getStr, sizeof(getStr));
        //如果最后一个字符不是分号
        
        // add by yih@
        
        bool isEnd = false;
        
        strcpy(getStr,trim(string(getStr)).c_str());       
         
        if(getStr[strlen(getStr)-1] != ';')
        {
            //不是分号，说明命令没有结束，给本次拼接之后的命令之后加空格，
            //以防止连在一起，换行前后两个命令重叠 
            strncat(commandStr, " ", 1);
            isCompleteCmd = false;
            continue;
        } 
        //是分号 
        else
        {
            //cout << "您的输入是:"<< commandStr << endl;
            isCompleteCmd = true;
        }    
        
        
        
        //////////////////////////////////////////////////////////
        //命令处理
        //////////////////////////////////////////////////////////
        MdbDmlSql* t_dmlSql = new MdbDmlSql(m_db,m_session,m_memMgr,m_lineMode);
        vCommands.push_back(commandStr);
        iCommandSeq = vCommands.size();
		//只处理 select/update/delete 等有where条件的dml语句，
		//其余命令将返回false，由原来的逻辑执行
        if (t_dmlSql->setSQL(commandStr))
        {
        	t_dmlSql->parseSQL();
    		delete t_dmlSql;
    		continue;
        }
        else
        	delete t_dmlSql;
        
        dealCmd(commandStr);
        
    }    
    return 0;    
}

