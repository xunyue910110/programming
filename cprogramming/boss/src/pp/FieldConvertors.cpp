// ##########################################
// Source file : FieldConvertors.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "FieldConvertors.h"
//##ModelId=4851D36300BB
FieldConvertors::FieldConvertors()
{
}

//##ModelId=4851D366001F
FieldConvertors::~FieldConvertors()
{
    for(int i = 0; i < m_ConvertRules.size(); i++)
    {
        delete m_ConvertRules[i];
    }
    m_ConvertRules.clear();
}

//##ModelId=4851D379030D
bool FieldConvertors::initialize(const vector<string> &argname,const int iSubChannelNum)
{
    string ruleText; 
    string ruleName,tmpStr,tmpStr1;
    FieldConvertRule *theConvertRule;
    bool flag = true;
    string tmpCondition,tmpRuleExp,tmpCondition1;
    int size;
    int r_size=0,l_size=0;
    int num1,g,t,i;
    string tmp1,tmp2;
    vector<string> argCondition;
    vector<string> argValue;
    string operatType,operatType1;
    string leftSubExpr,rightSubExpr;
    string leftSubExpr1,rightSubExpr1;
    
    argValue.clear();
    for(int j=0; j <argname.size(); j++)
    {
        ruleText = argname[j];
        
        if (ruleText.compare(0,2,"IF") == 0)
        {
            argCondition.clear();
            if (!flag)
            {
                argCondition.clear();
                argValue.clear();
                return false;
            }
            flag = false;
            tmpCondition = ruleText.substr(3,ruleText.size()-4); //(lent>=13)AND((otherf1=7613)OR(otherf1=7615))
            size = tmpCondition.size();
            int x1 = tmpCondition.find("AND");
            if(x1 > 0)
            {
                if ((tmpCondition[x1-1]!=')') || (tmpCondition[x1+3]!='('))
                {
                    x1=0;
                }
            }
            int x2 = tmpCondition.find("OR");
            if(x2 > 0)
            {
                if ((tmpCondition[x2-1]!=')') || (tmpCondition[x2+2]!='('))
                {
                    x2=0;
                }
            }
            if ( ( x1 > 0) && (x2 > 0) )
            {
                //查找第一对括号
                i=0;
               for(;i<size;i++)
               {
                   if(tmpCondition[i]=='(')  
                   {
                       r_size++; 
                   }
            
                   if(tmpCondition[i]==')') 
                   {
                       l_size++;
                   }             
        
                   if(r_size == l_size)
                   {
                       break; 
                   }       
               }
               i++; 
 
               leftSubExpr = tmpCondition.substr(0,i);  //(lent>=13)
               rightSubExpr = tmpCondition.substr(i);   //AND((otherf1=7613)OR(otherf1=7615))
     
               size = rightSubExpr.size();    
               i = 0;
               for(;i<size;i++)
               {   
                   if(rightSubExpr[i]=='(')
                   {
                       break;
                   }        
               }  
               operatType=rightSubExpr.substr(0,i);     //"AND"
               leftSubExpr = leftSubExpr.substr(1, leftSubExpr.size()-2);
               rightSubExpr = rightSubExpr.substr(i+1,rightSubExpr.size()-i-2); //(otherf1=7613)OR(otherf1=7615)
    
               if (operatType.compare("AND") == 0)
               {
                   size = rightSubExpr.size();
                   i=0;
                   for(;i<size;i++)
                   {
                       if(rightSubExpr[i]=='(')
                       {  
                           r_size++; 
                       }
            
                       if(rightSubExpr[i]==')') 
                       {
                           l_size++;
                       }             
        
                       if(r_size == l_size)
                       {
                           break;
                       }        
                   }
                   i++;
                   leftSubExpr1 = rightSubExpr.substr(0,i); //(otherf1=7613)
                   rightSubExpr1 = rightSubExpr.substr(i);  //OR(otherf1=7615)
      
                   size = rightSubExpr1.size();    
                   i=0;
                   for(;i<size;i++)
                   {   
                       if(rightSubExpr1[i]=='(')
                       {
                           break;
                       }        
                   }  
                   operatType1=rightSubExpr1.substr(0,i);
                   rightSubExpr1 = rightSubExpr1.substr(i);
    
                   if (operatType1.compare("OR") == 0)
                   {
                       leftSubExpr1 = leftSubExpr1.substr(1,leftSubExpr1.size()-2);
                       rightSubExpr1 = rightSubExpr1.substr(1,rightSubExpr1.size()-2);
                   }
                   else 
                   {
                       return false;
                   }
     
               }
               else
               {
                   return false;
               }
               argCondition.push_back(leftSubExpr+";"+leftSubExpr1+";");
               argCondition.push_back(leftSubExpr+";"+rightSubExpr1+";");
            }
            else if(x1 > 0)
            {
               tmp2 = "";    
               for (num1 = tmpCondition.find("AND"); num1 > 0; )
               {

                       if((tmpCondition[num1-1] != ')') || (tmpCondition[num1+3] != '('))
                       {
                          tmpCondition1 = tmpCondition.substr(num1+3);
                          num1 = tmpCondition1.find("AND");
                          continue;
                       }

                   tmp1 = tmpCondition.substr(0,num1);        //得到"AND"前的内容
                   tmpCondition = tmpCondition.substr(num1+3,tmpCondition.size()-num1-3); //得到"AND"后的内容
                            
                   g = tmp1.find_last_of("(");
                   tmp1 = tmp1.substr(g+1,tmp1.size()-g-2);
                            
                   tmp2 =tmp2 + tmp1+";";
                            
                   num1 = tmpCondition.find("AND");
               }
               g = tmpCondition.find_first_of("(");
               t = tmpCondition.find_first_of(")");
                        
               tmp1 = tmpCondition.substr(g+1,t-g-1);
               tmp2 = tmp2 + tmp1 + ";";
               argCondition.push_back(tmp2);

           }
           else if(x2 > 0)
           {
               for (num1 = tmpCondition.find("OR"); num1 > 0; )
               {

                       if((tmpCondition[num1-1] != ')') || (tmpCondition[num1+2] != '('))
                       {
                          tmpCondition1 = tmpCondition.substr(num1+2);
                          num1 = tmpCondition1.find("OR");
                          continue;
                       }

                   tmp1 = tmpCondition.substr(0,num1);        //"OR"前的内容
                   tmpCondition = tmpCondition.substr(num1+2,tmpCondition.size()-num1-2); //"OR"后的内容
                            
                   g = tmp1.find_last_of("(");
                   tmp1 = tmp1.substr(g+1,tmp1.size()-g-2);
                            
                   argCondition.push_back(tmp1+";");
                            
                   num1 = tmpCondition.find("OR");
               }
               g = tmpCondition.find_first_of("(");
               t = tmpCondition.find_first_of(")");
                        
               tmp1 = tmpCondition.substr(g+1,t-g-1);
               argCondition.push_back(tmp1 + ";");
           }
           else
           { 
               argCondition.push_back(tmpCondition+";");
           }
       }
       else if(ruleText.compare(0,4,"THEN") == 0)
       {
           continue;
       }
       else if(ruleText.compare(0,4,"RULE") == 0)
       {
           int x;
           x = ruleText.find(":");
           tmpStr1 = ruleText.substr(0,x+1);
           for(int i=0; i<argCondition.size(); i++)
           {
                tmpStr = tmpStr1+argCondition[i]+ruleText.substr(x+1,ruleText.length()-x-1);
                argValue.push_back(tmpStr);
           }
       }
       else if(ruleText.compare(0,6,"ELSEIF") == 0)
       {
           argCondition.clear();
           tmpCondition = ruleText.substr(7,ruleText.size()-8);
           size = tmpCondition.size();
           int x3 = tmpCondition.find("AND");
           if (x3 > 0)
           {
               if ((tmpCondition[x3-1]!=')') || (tmpCondition[x3+3]!='('))
               {
                   x3=0;
               }
           }
           int x4 = tmpCondition.find("OR");
           if (x4 > 0)
           {
               if ((tmpCondition[x4-1]!=')') || (tmpCondition[x4+2]!='('))
               {
                   x4=0;
               }
           }
           if ( (x3 > 0) && (x4 > 0) )
           {
               //查找第一对括号
               i=0;
               for(;i<size;i++)
               {
                   if(tmpCondition[i]=='(') 
                   { 
                       r_size++;
                   } 
            
                   if(tmpCondition[i]==')') 
                   {
                       l_size++;
                   }             
        
                   if(r_size == l_size)
                   {
                       break;
                   }        
               }
               i++; 
               leftSubExpr = tmpCondition.substr(0,i);  //(lent>=13)
               rightSubExpr = tmpCondition.substr(i);   //AND((otherf1=7613)OR(otherf1=7615))
     
               size = rightSubExpr.size();    
               i=0;
               for(;i<size;i++)
               {   
                   if(rightSubExpr[i]=='(')
                   {
                       break;   
                   }     
               }  
               operatType=rightSubExpr.substr(0,i);     //"AND"
               leftSubExpr = leftSubExpr.substr(1, leftSubExpr.size()-2);
               rightSubExpr = rightSubExpr.substr(i+1,rightSubExpr.size()-i-2); //(otherf1=7613)OR(otherf1=7615)
    
               if (operatType.compare("AND") == 0)
               {
                   size = rightSubExpr.size();
                   i=0;
                   for(;i<size;i++)
                   {
                       if(rightSubExpr[i]=='(') 
                       { 
                           r_size++; 
                       }
            
                       if(rightSubExpr[i]==')') 
                       {
                           l_size++;   
                       }         
        
                       if(r_size == l_size)
                       {
                           break;
                       }        
                  } 
                  i++;
                  leftSubExpr1 = rightSubExpr.substr(0,i); //(otherf1=7613)
                  rightSubExpr1 = rightSubExpr.substr(i);  //OR(otherf1=7615)
      
                  size = rightSubExpr1.size();    
                  i=0;
                  for(;i<size;i++)
                  {   
                     if(rightSubExpr1[i]=='(')
                     { 
                         break;
                     }        
                  }  
                  operatType1=rightSubExpr1.substr(0,i);
                  rightSubExpr1 = rightSubExpr1.substr(i);
    
                  if (operatType1.compare("OR") == 0)
                  {
                      leftSubExpr1 = leftSubExpr1.substr(1,leftSubExpr1.size()-2);
                      rightSubExpr1 = rightSubExpr1.substr(1,rightSubExpr1.size()-2);
                  }
                  else 
                  {
                      return false;
                  }
              }
              else
              {
                  return false;
              }
              argCondition.push_back(leftSubExpr+";"+leftSubExpr1+";");
              argCondition.push_back(leftSubExpr+";"+rightSubExpr1+";");
          }
          else if(x3 > 0)
          {
              tmp2 = "";
              for (num1 = tmpCondition.find("AND"); num1 > 0; )
              {
           
                      if((tmpCondition[num1-1] != ')') || (tmpCondition[num1+3] != '('))
                      {
                          tmpCondition1 = tmpCondition.substr(num1+3);
                          num1 = tmpCondition1.find("AND");
                          continue;
                      }

                  tmp1 = tmpCondition.substr(0,num1);        //得到"AND"前的内容
                  tmpCondition = tmpCondition.substr(num1+3,tmpCondition.size()-num1-3); //得到"AND"后的内容
                            
                  g = tmp1.find_last_of("(");
                  tmp1 = tmp1.substr(g+1,tmp1.size()-g-2);
                            
                  tmp2=tmp2+ tmp1+";";
                            
                  num1 = tmpCondition.find("AND");
              }
              g = tmpCondition.find_first_of("(");
              t = tmpCondition.find_first_of(")");
                        
              tmp1 = tmpCondition.substr(g+1,t-g-1);
              tmp2=tmp2+ tmp1+";";
                        
              argCondition.push_back(tmp2);
          }
          else if(x4 > 0)
          {
              for (num1 = tmpCondition.find("OR"); num1 > 0; )
              {   

                  if((tmpCondition[num1-1] != ')') || (tmpCondition[num1+2] != '('))
                  {
                      tmpCondition1 = tmpCondition.substr(num1+2);
                      num1 = tmpCondition1.find("OR");
                      continue;
                  }

                  tmp1 = tmpCondition.substr(0,num1);        //"OR"前的内容
                  tmpCondition = tmpCondition.substr(num1+2,tmpCondition.size()-num1-2); //"OR"后的内容
                            
                  g = tmp1.find_last_of("(");
                  tmp1 = tmp1.substr(g+1,tmp1.size()-g-1);
                            
                  argCondition.push_back( tmp1+";");
                            
                  num1 = tmpCondition.find("OR");
              }
              g = tmpCondition.find_first_of("(");
              t = tmpCondition.find_first_of(")");
                        
              tmp1 = tmpCondition.substr(g+1,t-g-1);
              argCondition.push_back( tmp1 + ";");
          }
          else
          {
              argCondition.push_back(tmpCondition+";");
          }    
      }
      else if(ruleText.compare(0,5,"ENDIF") == 0)
      {
          argCondition.clear();
          flag = true;

      }
     else 
     {
         continue;  
     }
   }
    
    for(int i = 0; i < argValue.size(); i++)
    {
        tmpRuleExp = argValue[i];
        ruleName = getRuleName(argValue[i]);

//#ifdef DEBUG
//        cout << "ruleText is:" << tmpRuleExp << endl;
//#endif
        if (ruleName.compare("RULE_1")==0)
        {
            theConvertRule = new FieldConvertRule_1(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_2")==0)
        {
            theConvertRule = new FieldConvertRule_2(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_3")==0)
        {
            theConvertRule = new FieldConvertRule_3(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_4")==0)
        {
            theConvertRule = new FieldConvertRule_4(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_5")==0)
        {
            theConvertRule = new FieldConvertRule_5(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_6")==0)
        {
            theConvertRule = new FieldConvertRule_6(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_7")==0)
        {
            theConvertRule = new FieldConvertRule_7(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_8")==0)
        {
            theConvertRule = new FieldConvertRule_8(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_9")==0)
        {
            theConvertRule = new FieldConvertRule_9(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_10")==0)
        {
            theConvertRule = new FieldConvertRule_10(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_11")==0)
        {
            theConvertRule = new FieldConvertRule_11(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_12")==0)
        {
            theConvertRule = new FieldConvertRule_12(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_13")==0)
        {
            theConvertRule = new FieldConvertRule_13(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_14")==0)
        {
            theConvertRule = new FieldConvertRule_14(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_15")==0)
        {
            theConvertRule = new FieldConvertRule_15(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_16")==0)
        {
            theConvertRule = new FieldConvertRule_16(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_17")==0)
        {
            theConvertRule = new FieldConvertRule_17(iSubChannelNum);
        }
        else if (ruleName.compare("RULE_18")==0)
        {
            theConvertRule = new FieldConvertRule_18(iSubChannelNum);
        }
       else if (ruleName.compare("RULE_19")==0)
       {
           theConvertRule = new FieldConvertRule_19(iSubChannelNum);
       }
        else
        {
            continue;
        }

        if (!theConvertRule->initialize(tmpRuleExp))
        {
            return false;
        }

        m_ConvertRules.push_back(theConvertRule);
    }

    return true;
}

//##ModelId=4851D3920261
bool FieldConvertors::convertField()
{
    int len;

    len = m_ConvertRules.size();
    for(int i = 0; i < len; i++)
    {
        m_ConvertRules[i]->convertField();
    }

    return true;
}

//##ModelId=4851D3AE032C
string FieldConvertors::getRuleName(string &theRuleText)
{
	char cTmp[200];
	string theRuleName;


    strcpy(cTmp,theRuleText.c_str());
    theRuleName=string( strtok(cTmp,":") );      //得到冒号前的串

    return theRuleName;
}

