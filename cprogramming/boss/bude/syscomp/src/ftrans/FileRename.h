/**
 Config Examples
 <A> A is added to destination file name
 #   the char in source file name is omitted
 ?   the same char as the source file name
 *   the same string as the source file name
 <ftrans>
	<FileRename>
	   Count = 3
	   SrcFilePattern001 = AC???D*
	   DstFilePattern001 = VGA<A>#?????
	</FileRename>
    .....	
 </ftrans>
*/
#include <ctype.h>
#include <string>


class FileRename
{
    private:
        string m_srcPattern;
        string m_dstPattern;
    public:
        void setSrcPattern(const string& str)    	
	    {
	        m_srcPattern = str;	
	    }
        
        void setDstPattern(const string& str)    	
	    {
	        m_dstPattern = str;	
	    }
        
        bool isMatchPattern(const char* str)
        {
           for(int j=0,k=0; j< m_srcPattern.length(); j++,k++)
	       {
	            if((m_srcPattern[j] == '?')||(m_srcPattern[j] == str[k]))
	            {
	                 	   continue;
	            }   		
	         	else
	         	{
	         	     if(m_srcPattern[j] == '*')
	         	         return true;
	         	     else
	                     return false;         	     	
	         	 }
	        }
	       	
	       	if(strlen(str) == m_srcPattern.length())
	       	   return true;
	       	else   
	       	   return false;
        }
        
        bool getDstFileName(const char * srcFileName, char * dstFileName)
        {
            int j;
            
            for(int i=0,srcPos=0,dstPos=0;i<m_dstPattern.length();)
            {
            	switch(m_dstPattern[i])
            	{
            	   case '?':
            	       dstFileName[dstPos]=srcFileName[srcPos];
            	       i++;
            	       srcPos++;
            	       dstPos++;
            	       break;
            	  case '*':
            	       strcat(dstFileName+dstPos,srcFileName+srcPos);
            	       return true;
            	  case '#':
            	       srcPos ++;
            	       i++;
            	       break;
            	  case '<':
            	       for(j=i+1;m_dstPattern[j]!='>';j++)
            	       {
            	          dstFileName[dstPos]= m_dstPattern[j];
            	          dstPos ++;
            	       }
            	       i = j+1;    	
            	       break;
            	  default:
            	       if(isalnum(m_dstPattern[i]))
            	       {  
            	       		dstFileName[dstPos]=m_dstPattern[i];
            	            dstPos++;
            	            srcPos++;
            	            i++;
            	            break;
            	       }  
            	       else
            	           return false;
            	}
            }	
        	return true;
        }
        
};	
