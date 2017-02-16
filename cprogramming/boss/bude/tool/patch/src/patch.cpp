/**patchinfo generator create by juyf@lianchuang.com */
	
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "file/File.h"
#include "base/Properties.h"
#include "base/StringUtil.h"


using namespace std;

class SOPatch_Generator{

public :
	
	map<string,string> solist;//列表
	void init(){
		solist.clear();
	}
	//过滤
	void filter(const string& str){
		bool isso = true;
		int p = str.find("lib");	
		if(p!=0){isso = false;}
		if(isso) p = str.find(".a");
		if(p >= 0){isso = false;}
		if(isso) p = str.find(".so");
		if(p < 3){isso = false;}		
		if(!isso){
			//cout << " : " << str <<endl;
			return;
		}
		string name = str.substr(3,p-3);		
		string key ;
		string version;	
		p = name.find("-");
		if( p > 0)	{
			key = name.substr(0,p);
			version = name.substr(p+1);
	    } else {
			key = name;
			version = "";//不用写1.0,空着就行了
		}	
		string oldver; 	
		map<string,string>::iterator it = solist.find(key);
		if(it==solist.end()){
			solist.insert(pair<string ,string> (key,version));
		} else {
			oldver  = it->second;
			const char* old = oldver.c_str();
			const char* ver = version.c_str();
			if(strcmp(old , ver) < 0){			
				solist.erase(key);
				solist.insert(pair<string ,string> (key,version));
			}
		}	
	}
	//列出文件
	void listfile(string solib){	
		File file(solib);
		if(!file.exists() || !file.isDirectory()){
			cout << "[GBOSS]不是目录或不存在："<< solib << endl;
			throw exception((solib + "不是目录或不存在").c_str());			
		}		
		std::list<string> file_list;
		int a = file.listDirectory(file_list);
	    std::list<string>::iterator it = file_list.begin();
	    for(; it !=file_list.end();it++){	           
	            filter(*it);//过滤          
		}
		cout <<"[GBOSS]patch目录:" << solib << " 共含文件:" << file_list.size() << " 有效.so文件:" << solist.size() <<endl;
	}
	
	//输出so
	void write(ostream& fout,string patch_version){
		map<string,string>::iterator it = solist.begin();		
		while( it != solist.end() ){
			fout << it->first << "=" << patch_version << endl;//it->second;					
			it++;
		}	
	}
    //输出unso
	void write_mark(ostream& fout,string patch_version){
		map<string,string>::iterator it = solist.begin();		
		while( it != solist.end() ){
			fout << "#" << it->first << "=" << patch_version << endl;//it->second;					
			it++;
		}	
	}
    
    void copySoTolculib(string lculib,string solib,string version){
    	string src;
    	string trg;
    	string script;
    	map<string,string>::iterator it = solist.begin();		
		while( it!= solist.end() ){
			src = "lib" + it->first;
			if(it->second != "") src += "-" + it->second;	
		    src += ".so";
		    
		    trg = "lib" + it->first + "-" + version + ".so";
		    script = "cp -f ";
		    script += solib + "/" + src  + " "+ lculib + "/" + trg;
		    cout << script <<endl;
		    system(script.c_str());
			it++;
		}			
    }
	
};

class Patch_Generator{
	private:
		string patch_root;
		string boss_lib;
		
		string mw_patchlevel;
		string so_patchinfo;
		string lcu_patchinfo;
		string sql_patchinfo;
		
		SOPatch_Generator so_patch;
		string patch_so_filename;
		string patch_lcu_filename;
		string patch_sql_filename;
		string e_mail;
		bool isUnpatch;
	public:
		
	Patch_Generator(string _patch_root,string _boss_lib
	       ,string _mw_patchlevel,string _so_patchinfo,
	       string _lcu_patchinfo,string _sql_patchinfo,string mail){
		this->patch_root = _patch_root;
		this->boss_lib = _boss_lib;
		
		this->mw_patchlevel = _mw_patchlevel;
		this->so_patchinfo = _so_patchinfo;
		this->lcu_patchinfo = _lcu_patchinfo;
		this->sql_patchinfo = _sql_patchinfo;
		this->e_mail = mail;
		this->patch_so_filename = "/patch_so.txt";
		this->patch_lcu_filename = "/patch_lcu.txt";
		this->patch_sql_filename = "/patch_sql.txt";	
		
		this->isUnpatch = false;		
	}
	
	void addPatchHead(string old,string version){
		
		ofstream fout("head_tmp" ,ios_base::out);
		fout << endl;
		fout << "!" << (isUnpatch ? "un" : "" ) << "patch " << version  << endl;				
		fout << "#" << (isUnpatch ? "un" : "" ) << "patching by " << e_mail <<" at " << getDateTimeStr() <<endl;
		fout.close();
		catFile(old, "head_tmp");
		File t("head_tmp");
		t.deleteFile();
	}	
	
	//合并文件插在头上
    int catFile(string old,string append){    	
		File atmp("patch_tmpfile");	
		File src1(old);	
		File src2(append);
		if(!src1.exists()){throw exception( (old + " file  not found" ).c_str());}	
		if(!src2.exists()){throw exception( (append + " file  not found" ).c_str());}		
		int i = atmp.concateFile(src2,src1);	
		if(  i >= 0 ){	
		    src1.deleteFile();	    
			i = atmp.renameTo(src1);
	    }
	    if(i < 0) throw exception((old + " throw exception  when cat file " + append).c_str());
		return i;
	}
	
	//生成so列表文件
	void generator_so(string version){
		so_patch.init();
		string patchlib = patch_root + version;
		
		so_patch.listfile(patchlib);			
		ofstream fout((patchlib + patch_so_filename).c_str() ,ios_base::out);
		
		so_patch.write(fout,version);		
		//fout <<"#generator total :" << so_patch.solist.size()<< endl ;
		fout << endl;		
		fout.close();		
	}
	
	void execute_patch(string version){
			
		//generator so
		generator_so(version);
		
		//copy so
		so_patch.copySoTolculib(boss_lib, patch_root + version, version);
		
		//so
		string append = patch_root + version + patch_so_filename;		
		catFile(so_patchinfo,  append);
		addPatchHead(so_patchinfo, version);
		
		//lcu
		append = patch_root + version + patch_lcu_filename;		
		catFile(lcu_patchinfo, append);
		addPatchHead(lcu_patchinfo, version);
		
		//sql
		append = patch_root + version + patch_sql_filename;		
		catFile(sql_patchinfo, append);
		bool temp =	this->isUnpatch;
		this->isUnpatch = false;
		addPatchHead(sql_patchinfo, version);
		this->isUnpatch = temp;
				
		//level
		addPatchHead(mw_patchlevel, version);		
		cout << "[GBOSS]系统更新完毕, 当前patch = " << version << endl;
	}
	
	void execute_unpatch(){	
		this->isUnpatch = true;	
		string currverion;
		string version = foundHistoryVersion(currverion);
		cout << "[GBOSS]回退前版本:"  << currverion  <<endl;
		if(version == "" || currverion == "" ){
		   cout << "[GBOSS]历史版本没有找到，无法回退，请手动修改 mw_patchlevel 文件" << endl;
		   return ;	   	
		}else{
		   cout << "[GBOSS]系统准备回退到 历史版本:" << version <<"."<< endl;		   
		}
	    markSoPatch(currverion,version);
	    markLCU_SQLPatch(currverion,version);
	    //level
		addPatchHead(mw_patchlevel, version);		
		cout << "[GBOSS]系统回退完毕, 当前patch = " << version << endl;
		
	}
	
	void markLCU_SQLPatch(string currverion,string hisverion){
		//lcu
	    string 	append = patch_root + currverion + patch_lcu_filename;		
		catFile(lcu_patchinfo, append);
		append = patch_root + hisverion + patch_lcu_filename;		
		catFile(lcu_patchinfo, append);
		addPatchHead(lcu_patchinfo, hisverion);
		
		//sql
		append = patch_root + currverion + patch_sql_filename;		
		catFile(sql_patchinfo, append);
	    append = patch_root + hisverion + patch_sql_filename;		
		catFile(sql_patchinfo, append);
		
		bool temp =	this->isUnpatch;
		this->isUnpatch = false;
		addPatchHead(sql_patchinfo, hisverion);
		this->isUnpatch = temp;		
	}
	
	void markSoPatch(string currverion,string hisverion){		
		ofstream ofout("tmp_mark",ios_base::out);			
		ifstream fin(so_patchinfo.c_str(),ios_base::in);
		bool start = false;
		string linedata;
		while(!fin.eof() && !fin.fail())
		{       
			getline(fin, linedata, '\n');
			StringUtil::trim(linedata);
		    if(linedata.size() > 0 && linedata.at(0)=='!'){
				if( start == false){
					int q = linedata.find("patch");
					if(q > 0){						
						string ver = linedata.substr(q+6);						
						if(ver == hisverion){
							start = true;
						}
					}
				}
			}
			if(start) {
				ofout << linedata << endl;
			} else {
				ofout <<"#" <<linedata << endl;
			}
		}
		ofout.close();
		
		File tmp("tmp_mark");
		File so_pf(so_patchinfo);
		if(tmp.exists() && so_pf.exists()){
			  so_pf.deleteFile();
			  tmp.renameTo(so_pf);
		}else{
		   throw new exception ((so_patchinfo + " not found!").c_str());
		}
				
		addPatchHead(so_patchinfo, hisverion);		
	}
	
	
	string foundHistoryVersion(string& currverion){
		
		ifstream fin(mw_patchlevel.c_str(),ios_base::in);
		if(!fin.is_open()) {
	    	cout << "[GBOSS] mw_patchlevel 文件打不开." <<endl;
	    	return "";
	    }
	    string version;
		string linedata;
		int undo = 0;
		while(!fin.eof() && !fin.fail())
		{       
			getline(fin, linedata, '\n');
			StringUtil::trim(linedata);
			if(linedata.size() > 0 && linedata.at(0)=='!'){
				if(linedata.size() > 7){
					int p = linedata.find("unpatch");
					int q = linedata.find("patch");
					if(p > 0){
						undo --;
						if(undo == 1){ 
				     		currverion = linedata.substr(9);;
				        }	
					}else if(q > 0){
					    undo ++;
					    if(undo == 1){ 
				     		currverion = linedata.substr(7);;
				        }					    
					}				    
					if(undo > 1){
						version = linedata.substr(7);
						
						break;	
					}						
				}	  
			}		    		
		} 		
		fin.close();
				
		return version;
	}
	
	string getDateTimeStr(){
		time_t st = time(0);
		tm ltm = *localtime( &st);
	    char buffer[ 32 ];
	    sprintf( buffer, "%04d-%02d-%02d %02d:%02d:%02d", ltm.tm_year + 1900, ltm.tm_mon + 1,
	             ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec );
	    return buffer;
	}
	
};

int main(int argc, char *argv[])
{   	
	SOPatch_Generator so_patch;
	cout << "--start--"  << endl;
    if(argc < 2 || string(argv[1]).at(0) != '-' ){
    	cout << "[GBOSS]注意：请把patch_lcu.txt,patch_sql.txt和要发布的*.so 拷贝到以patch版本号为名称的目录中,如:1.0" <<endl;
    	cout << "[GBOSS]      然后输入命令行: " <<endl;
    	cout << "[GBOSS]        -p 指向这个 patch版本号.    例:./patch -p 1.0" <<endl;
    	cout << "[GBOSS]        -u 回退,系统自动获得版本号. 例:./patch -u " <<endl;
    	return 0;
    }else if(string(argv[1])=="-p" && argc < 3) {
    	cout << "[GBOSS]请用  -p 指向patch版本目录. 例:./patch -p 1.0" <<endl;
    	return 0;
    }
    ifstream fin("patch.properties",ios_base::in);
    Properties env;
    env.load(fin);      
    
    Patch_Generator pg(env.getProperty("PATCH_ROOT"),env.getProperty("BOSS_LIB"),
    env.getProperty("MW_PATCHLEVEL"),env.getProperty("SO_PATCHINFO"),env.getProperty("LCU_PATCHINFO"),env.getProperty("SQL_PATCHINFO"),env.getProperty("E_MAIL"));
    string version;
    try{
		if(string(argv[1])=="-p"){
			version = string(argv[2]);
			pg.execute_patch(version);
		}else if (string(argv[1])=="-u"){
			pg.execute_unpatch();
		}
		
    }catch(exception & e){
    	cout << "[GBOSS]异常退出." << e.what() << endl;
    }catch(...){
        cout << "[GBOSS]异常退出" <<endl;
    }
	cout << "-end-" <<endl;
	return 0;
}

