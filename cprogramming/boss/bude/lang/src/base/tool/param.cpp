
#include "base/BCDUtil.h"
#include "base/StringUtil.h"
#include "base/SysParam.h"
#include "base/TimeUtil.h"
#include "base/CryptUtil.h"

#include <iostream>
#include <stdio.h>
#include <string>

int main() {
	
    SysParam p;
    p.initialize("boss.conf");
    p.setSectionPath("\\SYSTEM");
    p.dump();



////
////vector<string> ret;
////char buffer[100]; memset(buffer,0,sizeof(buffer));
////char nbuffer[100]; memset(nbuffer,0,sizeof(nbuffer));
////
////StringUtil::EBCD_incode(buffer,"122342342");
////StringUtil::EBCD_decode(nbuffer,buffer);
////cout << "buffer:" << buffer << endl; 
////cout << "nbuffer:" << nbuffer << endl; 
////
////StringUtil::split("1,2,3,4,,5,", ":",ret);
////cout << "size: " << ret.size() << endl; 
////
////for (int i=0; i < ret.size(); ++i) {
////	cout << " " << i << " :[" << ret[i] << "]" << endl;
////}
////
////ret=StringUtil::split("5,6,7,8,,9,",",");
////cout << "size: " << ret.size() << endl; 
////for (int i=0; i < ret.size(); ++i) {
////	cout << " " << i << " :[" << ret[i] << "]" << endl;
////}
////
////
////StringUtil::command("ls -l",ret);
////cout << "size: " << ret.size() << endl; 
////for (int i=0; i < ret.size(); ++i) {
////	cout  << ret[i]<< endl;
////}
////
////
////string str;
////str="this is a table";
////
////StringUtil::replace(str,"a","ee");
////
////cout << " :str" << str << endl;
////
////str = StringUtil::replace("this is a book","is" ,"si");
////cout << str << endl;
////
////cout << StringUtil::matches("this a book","*book*") << endl;
////
////cout << "result:" << StringUtil::format()  << endl;
////cout << "result:" << StringUtil::format(0,"%y")  << endl;
////
//////////////////////////////////////////////////////////////////////////////
   
	
	//char  buffer[200];
	//
	//strcpy(buffer,"this is a table");
	//
	//string str = "this is a table";
	//int ret = StringUtil::eraseChar();
	//cout << " buffer:" << ret << " "<< buffer << endl;

//    // following section tests for CrytpUtil	
//	string source_data = "1234567890123";
//	cout << "   source data:" << source_data << endl;
//	
//	string encypted_data;
//	CryptUtil::encrypt(source_data.c_str(),encypted_data) ;
//	cout << " encypted data:\\\\pak{" << encypted_data << "}\\" << endl;
//	
//	string decypted_data;
//	CryptUtil::decrypt(encypted_data.c_str(),decypted_data) ;
//	cout << " decypted data:\\\\pak{" << decypted_data << "}\\" << endl;
//	
//	
//	///////////////////////////////////////////////////
//	string data = "\\\\pak{" + encypted_data + "}\\\\";
//	data +=   "|ok what|" +data + " |" + "\\\\pak{}\\";
//
//
//	cout << data << endl;
//	cout.flush();
//	cout << " result:" << CryptUtil::decrypt_pakage(data) << endl;
//	cout << " decrypt data:" << data << endl;
//	
//	
//	data = "\\\\pwd{1234567890123}\\|another|\\\\pwd{ok}\\";
//	CryptUtil::encrypt_pakage(data);
//	cout << "pak:" << data << endl;
//	CryptUtil::decrypt_pakage(data);
//	cout << "pwd:" << data << endl;
	
	
	
	CryptUtil::translate();
	
	

	return 0;
}
