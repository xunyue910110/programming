#include "mwlcu/fmloperator.h"
#include <sstream>
FmlOperator::FmlOperator()
{
}

FmlOperator::~FmlOperator()
{
}


inline string& FmlOperator::ltrim(string &ss){
	string::iterator p = ss.begin();
	while(p!= ss.end()){
		if(*p!=' '&&*p!='\t'&& *p!='\n' && *p!='\r') break;
		p++;
	}
	ss.erase(ss.begin(),p);
	return ss;
}

inline string& FmlOperator::rtrim(string &ss){
	string::reverse_iterator p = ss.rbegin();
	while(p!= ss.rend()){
		if(*p!=' '&&*p!='\t' && *p!='\n' && *p!='\r') break;
		p++;
	}
	ss.erase(p.base(),ss.end());
	return ss;
}

inline string& FmlOperator::trim(string &st){
	ltrim(rtrim(st));
	return st;
}



void FmlOperator::print(const OperItem& item){
	Operation operi = item.oper;
	string oper;
	switch(operi){
		case OP_NONE:          oper="OP_NONE         ";break;
		case OP_BUF_ADD:       oper="OP_BUF_ADD      ";break;
		case OP_BUF_MUTI:      oper="OP_BUF_MUTI     ";break;
	    case OP_PRINT_VALUE:   oper="OP_PRINT_VALUE  ";break;
	    case OP_COVER_LINE:    oper="OP_COVER_LINE   ";break;
		case OP_BUF_NULL:      oper="OP_BUF_NULL     ";break;
		case OP_COPY_BUFF:     oper="OP_COPY_BUFF    ";break;
		case OP_FLD_NULL:      oper="OP_FLD_NULL     ";break;
		case OP_COPY_FLD:      oper="OP_COPY_FLD     ";break;
		case OP_SET_VALUE_BUF: oper="OP_SET_VALUE_BUF";break;
		case OP_SET_VALUE:     oper="OP_SET_VALUE    ";break;
		case OP_SET_VALUE_STR: oper="OP_SET_VALUE_STR";break;
		case OP_PLUSPLUS:      oper="OP_PLUSPLUS     ";break;
		case OP_SUBTRACT:      oper="OP_SUBTRACT     ";break;
		case OP_OTHER:         oper="OP_OTHER        ";break;
    }
	cout << "-----------------------" << endl;
	cout << "origin  : "<< item.origin <<endl;
	cout << "oper    : " << oper  <<endl;
	cout << "leftbuf : " << item.leftbuf  <<endl;
	cout << "leftfld : " << item.leftfld  <<endl;
	cout << "leftidx : " << item.leftidx  <<endl;
	cout << "rightbuf: " << item.rightbuf  <<endl;
	cout << "rightfld: " << item.rightfld  <<endl;
	cout << "rightidx: " << item.rightidx  <<endl;
	cout << "value   : " << item.value  <<endl;
	cout << "isAll   : " << item.isAll  <<endl;
	cout << "leftidx_var : " << item.leftidx_var  <<endl;
	cout << "rightidx_var: " << item.rightidx_var  <<endl;
	cout << "isIdx_Var   : " << item.isIdx_Var  <<endl;
	cout << "-----------------------" << endl;
}


//用流去解析
void FmlOperator::parser(const string& config,vector <OperItem>& operlist)
{
	ostringstream oss;
	istringstream fss(config);
    char delimiter = '\n';
    string line;
    //过滤掉注释
    while(!fss.eof() && !fss.fail())
	{
		getline(fss, line, delimiter);
		trim(line);
		if(!line.empty() && line.at(0)!='#'){
			oss << line ;
	    }
	}
    istringstream iss(oss.str());
    delimiter = ';';
    //取出每一条;
    while(!iss.eof() && !iss.fail())
	{
		getline(iss, line, delimiter);
		trim(line);
		//cout << "IN: " << line << endl;
		if(!line.empty() && line.at(0)!='#'){
			OperItem item;
			parserline(line,item);
			//print(item);
			if(item.isAll){
				operlist.push_back(item);
			} else {
			    operlist.push_back(item);
			    THROW_P1(LCUException,"[LCU]不合法的脚本: %s",line.c_str());
			    //cout << "[LCU]不合法的脚本: "<< line<<endl;
			}

		} else {
		   continue;
		}
	}
	//cout << "parser finished" <<endl;
}

//解析一条语句，不包括';\n'
void FmlOperator::parserline(string& line,OperItem& opItem){
	opItem.origin   = line;
	opItem.oper     = OP_NONE;
	opItem.leftidx  = -1;
	opItem.rightidx = -1;
	opItem.isAll    = false;

    opItem.isIdx_Var = false;

	string left;
	string right;
    int pad = line.find_first_of("+-*");
	int pos = line.find_first_of('=');//得到左边
	if (pos < 0 && pad > 0){
		pad = line.find("++");
		if(pad > 0){
			left = line.substr(0 , pad);
			opItem.oper = OP_PLUSPLUS;
		}
		pad = line.find("--");
		if(pad > 0){
		 	left = line.substr(0 , pad);
		    opItem.oper = OP_SUBTRACT;
		}
	}
	else if (pos > 0 && pad > 0){
		pad = line.find("+=");
		if(pad > 0){
			left = line.substr(0 , pad);
			opItem.oper = OP_BUF_ADD;
		}
		pad = line.find("*=");
		if(pad > 0){
		 	left = line.substr(0 , pad);
		    opItem.oper = OP_BUF_MUTI;
		}

    }
    //分左右
	if (pos > 0){
		if( opItem.oper == OP_BUF_ADD || opItem.oper == OP_BUF_MUTI){
		    left=left.substr(0,pos-1);
		} else {
			left = line.substr(0,pos);
	    }
		pad = line.size() - 1;
		if(pos < pad)
			right = line.substr(pos + 1, pad - pos);
		else
		    right = "";
	}

    //下面解析left
    rtrim(left);
    pos = left.find('.');
    pad = left.find('[');

    if(pos < 0 ){
		opItem.leftbuf = left;
		if(opItem.oper == OP_NONE) opItem.oper = OP_COPY_BUFF;            //针对Buff操作的 有可能是NULL有可能是cover
        if(pad > 0){
        	opItem.oper = OP_COVER_LINE ;
        	opItem.leftbuf = left.substr(0,pad);
        	pos = pad;

			pad = left.find_last_of(']');
			if(pad > 0){
				string tm = left.substr(pos + 1,pad-pos-1);
				trim(tm);
				int p = tm.find_first_not_of("0123456789");
				if( p >= 0) {
				    opItem.leftidx_var = tm;
				    opItem.isIdx_Var = true;
				} else {
					opItem.leftidx = atoi(tm.c_str());
			    }
			} else {
				opItem.isAll = false;
				cout << "[LCU]操作符'[]'不匹配" << endl;
				return ;//不存在操作符号
			}
       }
   } else if(pos > 0 && pad < pos && pad > 0){
	   		opItem.oper = OP_COVER_LINE ;
	       	opItem.leftbuf = left.substr(0,pad);
	       	pos = pad;

			pad = left.find_last_of(']');
			if(pad > 0){
				string tm = left.substr(pos + 1,pad-pos-1);
				trim(tm);
				int p = tm.find_first_not_of("0123456789");
				if( p >= 0) {
				    opItem.leftidx_var = tm;
				    opItem.isIdx_Var = true;
				} else {
					opItem.leftidx = atoi(tm.c_str());
			    }
			} else {
				opItem.isAll = false;
				cout << "[LCU]操作符'[]'不匹配" << endl;
				return ;//不存在操作符号
			}
    } else {
		opItem.leftbuf = left.substr(0,pos);
		if(pad > 0){
			opItem.leftfld = left.substr(pos + 1,pad-pos-1);
			if(opItem.oper == OP_NONE) opItem.oper = OP_SET_VALUE_BUF;           //可能是，不能全确定

		} else {
			opItem.leftfld = left.substr(pos + 1,left.size() - 1 - pos);
			if(opItem.oper == OP_NONE) opItem.oper = OP_COPY_FLD;      //可能是，但不能确定
		}
		if(pad > 0){
			pos = pad;

			pad = left.find_last_of(']');
			if(pad > 0){
				string tm = left.substr(pos + 1,pad-pos-1);
				trim(tm);
				int p = tm.find_first_not_of("0123456789");
				if( p >= 0) {
				    opItem.leftidx_var = tm;
				    opItem.isIdx_Var = true;
				} else {
					opItem.leftidx = atoi(tm.c_str());
			    }
			} else {
				opItem.isAll = false;
				cout << "[LCU]操作符'[]'不匹配" << endl;
				return ;//不存在操作符号
			}
		}
    }

    //如果是++ -- 就不用再解析了
    if(opItem.oper == OP_PLUSPLUS || opItem.oper == OP_SUBTRACT){
   	       if(opItem.leftidx < 0) opItem.leftidx  = 0;
           opItem.isAll = !opItem.leftfld.empty();//不是空才正确 leftbuf为空是允许的
           return ;
    }

    //下面解析right
    trim(right);
    if( opItem.oper == OP_BUF_ADD || opItem.oper == OP_BUF_MUTI){
        	 opItem.rightbuf = right;
        	 opItem.isAll = !opItem.leftbuf.empty() && !opItem.rightbuf.empty();
		     return;
    }
    if(right.empty()){
	     opItem.isAll = false;
	     return ;
	}
    if(right == "NULL" || right == "null"){
         if(opItem.oper == OP_COPY_BUFF){
			opItem.oper = OP_BUF_NULL;
			opItem.isAll = true;
         } else {
			opItem.oper = OP_FLD_NULL;
			opItem.isAll = !opItem.leftfld.empty();
         }
         return;
     }
     pos = right.find_first_of("-1234567890");
     if( pos == 0){ //数字常量
     	 pos = right.find_first_not_of("1234567890.-");
     	 if( pos >= 0){
     	 	cout << "[LCU]数字中有非法字符"<<endl;
     	 	opItem.isAll = false;
     	 	return ;
     	 }
     	 //pos = right.find('.');//运行时决定是int还是double
     	 if(opItem.leftidx < 0) opItem.leftidx = 0;
     	 opItem.oper = OP_SET_VALUE;
     	 opItem.value = right;
         opItem.isAll = !opItem.leftfld.empty();//不是空才正确 leftbuf为空是允许的
     	 return ;
     }
     //字符串常量
     pos = right.find('\"');
     if( pos >= 0){
     	pad = right.find_last_of('\"');
     	if(pad > pos){
			if(opItem.leftidx < 0) opItem.leftidx = 0;
			opItem.oper = OP_SET_VALUE;
			opItem.value = right.substr(pos+1,pad-pos-1);
			opItem.isAll = !opItem.leftfld.empty();//不是空才正确 leftbuf为空是允许的
     	} else {
     	    cout << "[LCU]不合法表达式：字符串常量"<<endl;
     	 	opItem.isAll = false;
     	}
     	return ;
     }
	//buffer拷贝
	pos = right.find('.');
	if(pos < 0){
		if(opItem.oper == OP_COVER_LINE ){
		    //后面继续分析

		} else  {
			if(opItem.oper == OP_COPY_BUFF){
				opItem.rightbuf = right;
				opItem.isAll = true;
			}else {
			    cout << "[LCU]不合法表达式：buffer拷贝" << endl;
			 	opItem.isAll = false;
			}
			return ;
	    }
	}

	pad = right.find('[');
	opItem.rightbuf = right.substr(0,pos);
	if(opItem.oper != OP_COVER_LINE){
		if(pad > 0){//有下标 表示是[0] = [0]
			 opItem.rightfld = right.substr(pos + 1,pad-pos-1);
			 //不管前面有没有下标，后面有下标就为普通复值
			 opItem.oper = OP_SET_VALUE_BUF;
		} else {
		    //没有有下标 表示是
			opItem.rightfld = right.substr(pos + 1,right.size() - 1 - pos);
			//前面有没有决定是不是普通复值,
			if(opItem.oper == OP_NONE) opItem.oper = OP_COPY_FLD;      //前面已经决定了,没有必要再设
		}
    }else{
        if(pad < 0) {opItem.isAll = false ; return;}
       	opItem.rightbuf = right.substr(0,pad);

    }
	if(pad > 0){
		pos = pad;

		pad = right.find_last_of(']');
		if(pad > 0){
			string tm = right.substr(pos + 1,pad-pos-1);
			trim(tm);
			int p = tm.find_first_not_of("0123456789");
			if( p >= 0) {
			    opItem.rightidx_var = tm;
			    opItem.isIdx_Var = true;
			} else {
				opItem.rightidx = atoi(tm.c_str());
		    }
		} else {
			opItem.isAll = false;
			cout << "[LCU]操作符'[]'不匹配" << endl;
			return ;//不存在操作符号
		}
	}

	if(opItem.leftidx < 0 ) opItem.leftidx = 0;
	if(opItem.rightidx < 0 ) opItem.rightidx = 0;
	opItem.isAll = !opItem.rightfld.empty();//右字段名不为空
	if(opItem.oper == OP_COVER_LINE) opItem.isAll = !opItem.rightbuf.empty();

	return ;
}


/**
	GetFldType
	- 0	short int
	- 1	long int
	- 2	character
	- 3	single-precision float
	- 4	double-precision float
	- 5	char * - null terminated
	- 6	character array
	*/
bool FmlOperator::execute(FMLContext& ctx, OperItem & item)
{
	bool ret = true;
	if(!item.isAll) {
		 cout << "[LCU]FmlOperator执行失败，不合法的表达式:"<<item.origin<<endl;
	     return false;
    }
	Operation ioper = item.oper;
	PCFmlBuf pbuf = ctx.getCFmlBuf(item.leftbuf);

	if(item.isIdx_Var){
		//每次都重新取一下
	    if(!item.leftidx_var.empty()){
	    	item.leftidx = atoi(ctx.getVariant(item.leftidx_var).c_str());
	    }
	    if(!item.rightidx_var.empty()){
	    	item.rightidx = atoi(ctx.getVariant(item.rightidx_var).c_str());
	    }
	    //cout << "[LCU] Operation.leftidx=" << item.leftidx << " rightidx=" << item.rightidx  <<endl;
    }
	//cout << "[LCU]FmlOperator执行："<<item.origin<<endl;
	switch(ioper){
		case OP_BUF_NULL:
			pbuf->ClearFmlValue();
			break;
		case OP_BUF_ADD:
			{
				PCFmlBuf rbuf = ctx.getCFmlBuf(item.rightbuf);
				pbuf->Cover(*(rbuf.get()));
			}
			break;
		case OP_BUF_MUTI:
			{
				PCFmlBuf rbuf = ctx.getCFmlBuf(item.rightbuf);
				pbuf->Append(*(rbuf.get()));
			}
		    break;
		case OP_COVER_LINE:
			{
				PCFmlBuf rbuf = ctx.getCFmlBuf(item.rightbuf);
				pbuf->CoverLine(item.leftidx,*(rbuf.get()),item.rightidx);
		    }
			break;
		case OP_COPY_BUFF:
			{
			pbuf->ClearFmlValue();
			pbuf->Append(*(ctx.getCFmlBuf(item.rightbuf).get()));
		    }
			break;
    	case OP_FLD_NULL:
			if(pbuf->IsFldExist(item.leftfld.c_str())){
				pbuf->DelAllFld(item.leftfld.c_str());
		    }//不存在也不管它
			break;
		case OP_PLUSPLUS:
			{
				if(!pbuf->IsFldExist(item.leftfld.c_str())){
					 cout << "[LCU]读不存在的字段OP_PLUSPLUS: " << item.origin <<endl;
					 ret = false;
					 break;
			    }
				long i = pbuf->GetInt(item.leftfld.c_str(),item.leftidx);
				i++;
				pbuf->SetInt(item.leftfld.c_str(),i,item.leftidx);
		    }
		    break;
		case OP_SUBTRACT:
			{
				if(!pbuf->IsFldExist(item.leftfld.c_str())){
					 cout << "[LCU]读不存在的字段OP_SUBTRACT: " << item.origin <<endl;
					 ret = false;
					 break;
				}
				long i = pbuf->GetInt(item.leftfld.c_str(),item.leftidx);
				i--;
				pbuf->SetInt(item.leftfld.c_str(),i,item.leftidx);
		    }
		    break;
		case OP_SET_VALUE://是int/double运行时决定
			{
				int t = pbuf->GetFldType(item.leftfld.c_str());
				if(t==0 || t==1){
					long v = atol(item.value.c_str());
					pbuf->SetInt(item.leftfld.c_str(),v,item.leftidx);
				}else if(t == 3 || t == 4){
				    double v = atof(item.value.c_str());
					pbuf->SetFloat(item.leftfld.c_str(),v,item.leftidx);
				}else if(t == 5){
				    pbuf->SetString(item.leftfld.c_str(),item.value,item.leftidx);
				}else {
				    cout << "[LCU]类型不符OP_SET_VALUE: " << item.origin <<endl;
				    ret = false;
				}
				break;
		    }
		case OP_SET_VALUE_STR:
			{
				int t = pbuf->GetFldType(item.leftfld.c_str());
				if( t == 5 ){
					pbuf->SetString(item.leftfld.c_str(),item.value,item.leftidx);
				}else if(t == 2 ){
				    char v = item.value.empty()? ' ':item.value.at(0);
					pbuf->SetChar(item.leftfld.c_str(),v,item.leftidx);
				} else {
				    cout << "[LCU]类型不符OP_SET_VALUE_STR: " << item.origin <<endl;
				    ret = false;
				}
				break;
		    }
		    break;
		case OP_SET_VALUE_BUF:
			{
				//print(item);
				int t1 = pbuf->GetFldType(item.leftfld.c_str());
				int t2 = pbuf->GetFldType(item.rightfld.c_str());
				PCFmlBuf rbuf = ctx.getCFmlBuf(item.rightbuf);
				if(!rbuf->IsFldExist(item.rightfld.c_str())){
					 cout << "[LCU]读不存在的字段OP_SET_VALUE_BUF: " << item.origin <<endl;
					 ret = false;
					 break;
				}
				if(t1 == t2){
					switch(t1){
						case 0:
						case 1:
							{
							   long r = rbuf->GetInt(item.rightfld.c_str(),item.rightidx);
							   pbuf->SetInt(item.leftfld.c_str(),r,item.leftidx);
						    }
							break;
						case 2:
							{
							   char r = rbuf->GetChar(item.rightfld.c_str(),item.rightidx);
							   pbuf->SetChar(item.leftfld.c_str(),r,item.leftidx);
						    }
							break;
						case 3:
						case 4:
							{
							   double r = rbuf->GetFloat(item.rightfld.c_str(),item.rightidx);
							   pbuf->SetFloat(item.leftfld.c_str(),r,item.leftidx);
						    }
							break;
						case 5:
							{
							   string r = rbuf->GetString(item.rightfld.c_str(),item.rightidx);
							   pbuf->SetString(item.leftfld.c_str(),r,item.leftidx);
						    }
							break;
						case 6:
						default:
							 cout << "[LCU]类型不支持OP_SET_VALUE_BUF: " << item.origin <<endl;
							 ret = false;
						     break;
					}

				} else {
				     if(t1 == 5 && t2 == 1 ){
				     	 long r = rbuf->GetInt(item.rightfld.c_str(),item.rightidx);
				     	 ostringstream oss;
				     	 oss << r ;
				     	 string  st = oss.str();
				     	 pbuf->SetString(item.leftfld.c_str(),st,item.leftidx);
				     } else if (t1 == 1 && t2 == 5 ){
				     	 string r = rbuf->GetString(item.rightfld.c_str(),item.rightidx);
				     	 stringstream oss;
				     	  oss << r ;
				     	 long s;
				     	  oss >> s;
				     	  pbuf->SetInt(item.leftfld.c_str(),s,item.leftidx);
				     } else {
					     cout << "[LCU]类型不同的字段不能赋值: " << item.origin <<endl;
					     ret = false;
					 }
					break;
				}
		    }
		    break;
	    case OP_COPY_FLD:
	    	{
				int t1 = pbuf->GetFldType(item.leftfld.c_str());
				int t2 = pbuf->GetFldType(item.rightfld.c_str());
				PCFmlBuf rbuf = ctx.getCFmlBuf(item.rightbuf);
				if(!rbuf->IsFldExist(item.rightfld.c_str())){
					 cout << "[LCU]读不存在的字段OP_COPY_FLD: " << item.origin <<endl;
					 ret = false;
					 break;
				}
				if((t1 == t2) || (t1==5 && t2==1) || ( t1==1 && t2==5) ){
					 ret = copyFld(pbuf,rbuf,item.leftfld,item.rightfld,t1,t2);
				} else {
				     cout << "[LCU]类型不同的字段不能赋值: " << item.origin <<endl;
				     ret = false;
					 break;
				}
	    	}
	    	break;
		case OP_NONE:
		case OP_OTHER:
		default:
			cout << "[LCU]CFmlBuf操作没有找到: "<< item.origin <<endl;
			print(item);
			break;
	}
	return ret;
}
//字段对考
//字段对考
bool FmlOperator::copyFld(PCFmlBuf pbuf,PCFmlBuf rbuf,const string& leftfldname,const string& rightfldname,int fldtype,int fldtype2)
{
	ASSERT(pbuf!=0 && rbuf!=0);
	bool ret = true;

    const char* leftfld = leftfldname.c_str();
    const char* rightfld = rightfldname.c_str();
    if(pbuf->IsFldExist(leftfld)){
       pbuf->DelAllFld(leftfld);//先把自己值清掉
    }
    int count = rbuf->GetCount(rightfld);
    if(count == 0){
       	return true;//一个都没有也是允许的
    }
    for(int idx = 0;idx < count;idx++){
        if(fldtype == fldtype2){
				switch(fldtype){
					case 0:
					case 1:
						{
						   long r = rbuf->GetInt(rightfld,idx);
						   pbuf->SetInt(leftfld,r,idx);
					    }
						break;
					case 2:
						{
						   char r = rbuf->GetChar(rightfld,idx);
						   pbuf->SetChar(leftfld,r,idx);
					    }
						break;
					case 3:
					case 4:
						{
						   double r = rbuf->GetFloat(rightfld,idx);
						   pbuf->SetFloat(leftfld,r,idx);
					    }
						break;
					case 5:
						{
						   string r = rbuf->GetString(rightfld,idx);
						   pbuf->SetString(leftfld,r,idx);
					    }
						break;
					case 6:
					default:
						 cout << "[LCU]OP_COPY_FLD类型不支持: " <<endl;
						 ret = false;
					     break;
				}
		} else if ( fldtype == 5 && fldtype2 == 1 )  {
			long r = rbuf->GetInt(rightfld,idx);
			ostringstream oss;
			oss << r ;
			string  st = oss.str();
			pbuf->SetString(leftfld,st,idx);
	     } else if (fldtype == 1 && fldtype2 == 5 ){
			string r = rbuf->GetString(rightfld,idx);
			stringstream oss;
			oss << r ;
			long s;
			oss >> s;
			pbuf->SetInt(leftfld,s,idx);
	     } else {
			cout << "[LCU]OP_COPY_FLD类型不支持: " <<endl;
			ret = false;
			break;
		 }
    }//for

    return ret;
}


void FmlOperator::execute(FMLContext& ctx,vector <OperItem>& operlist){
	vector<OperItem>::iterator it=operlist.begin();
    bool r;
	while( it != operlist.end() ){
		r = execute(ctx,*it);
		if(r == false){
			//ctx.getLogger().alert(((*it).origin + " 执行失败").c_str());
			//print(*it);
			if(ctx.getLogger().isTraceEnabled())
			ctx.getLogger().trace(__FILE__, __LINE__, ((*it).origin + " 执行失败").c_str());			
		}else{
		    //cout << "[LCU]FmlOperator执行成功: " << (*it).origin << endl;
		}
		it++;
	}
}


