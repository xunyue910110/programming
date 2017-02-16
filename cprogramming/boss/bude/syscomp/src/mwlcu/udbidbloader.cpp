#include "mwlcu/udbidbloader.h"
#include <exception>

using namespace std;
using namespace UDBI;
using namespace ebpm4c;
namespace ebpm4c{


UDBIDBDefinitionsLoader::~UDBIDBDefinitionsLoader(){
}

void UDBIDBDefinitionsLoader::refresh(){
  if(factory){
    //TODO以后根据更新情况动态加载
    this->load();
  }
}
//并没有用过这个函数
void UDBIDBDefinitionsLoader::load(PHandleFactory _factory){
    this->setHandleFactory(_factory);
    this->load();
}

 
 bool UDBIDBDefinitionsLoader::removebyName(const string& name){
	base_ProcessDefinitions::const_iterator pos = str_defs.find(name);
	PProcessDefinition pdef;
	if(pos != str_defs.end()){
		pdef = pos->second;
		str_defs.erase(name);
		id_defs.erase(pdef->getId());
		return true;
	}
	return false;//没有找到
}

 
PProcessDefinition UDBIDBDefinitionsLoader::refreshDefinitionById(const int64_t& id){
  if(factory){
    //TODO以后根据更新情况动态加载
    this->loadbyId(id);
  }
  return getDefinitionById(id);
}

PProcessDefinition UDBIDBDefinitionsLoader::refreshDefinitionByName(const string& name){
	if(factory){
		//TODO以后根据更新情况动态加载
		this->loadbyName(name);
	}
	base_ProcessDefinitions::const_iterator pos = str_defs.find(name);
	PProcessDefinition pdef;
	if(pos != str_defs.end()){
	    pdef = pos->second;
	}
	if(pdef == 0){
		THROW(LCUExecutionException,"[LCU]流程定义没有找到:" + name);
	}
    return pdef;
}


PProcessDefinition UDBIDBDefinitionsLoader::getDefinitionById(const int64_t& id){
    map_ProcessDefinitions::const_iterator pos = id_defs.find(id);
	PProcessDefinition pdef;
    if(pos != id_defs.end()){
        pdef = pos->second;
    } else {
        pdef = loadbyId(id);
        if ( !pdef ){
            THROW(LCUExecutionException,"[LCU]流程定义id没有找到");
        }
    }
    return pdef;
}

PProcessDefinition UDBIDBDefinitionsLoader::getDefinitionByName(const string& name){
    base_ProcessDefinitions::const_iterator pos = str_defs.find(name);
	PProcessDefinition pdef;
    if(pos != str_defs.end()){
        pdef = pos->second;
    } else {
    	pdef = refreshDefinitionByName(name);//不存在重新到库里读一下
    	if(pdef == 0){
           THROW(LCUExecutionException,"[LCU]流程定义没有找到:" + name);
    	}
    }
    return pdef;
}

void UDBIDBDefinitionsLoader::setHandleFactory(PHandleFactory  _factory){
    this->factory = _factory;
}
//加载错误编码表
void UDBIDBDefinitionsLoader::loadErrorCodeTable(map<int,string>& errtable_map){
	
	JTable * rst = pcon->CreateJTable();
	
    string info;
    int code;
    int count = 0;
    try{
	    rst->AddSql("select ERRORCODE, ERRORINFO from TD_S_ERRORCODE n order by ERRORCODE");
		rst->ExecSelect();
		
		while(rst->Fetch()){
			code = rst->GetInt("ERRORCODE");
			info = rst->GetString("ERRORINFO");
			count ++ ;
			errtable_map.insert(pair<int ,string>(code,info));
	    }
	   
	}catch(exception& e){
	     LOG_TRACE_P1(lcutrace,"[LCU]加载错误编码表异常:%s",e.what());	     
	}catch(...){
	}
	delete rst;
    LOG_TRACE_P1(lcutrace,"[LCU]加载错误编码表%d条",count);
}

/**
 * Load all Definitions form database;
 * 继承实现并父类的动作
 */
void UDBIDBDefinitionsLoader::load(){
   str_defs.clear();
   id_defs.clear();
    
	
   PProcessDefinition pDef;
   int64_t id;
   string name, displayname;//, description;
   int count = 0;
  
   //cout << "[LCU][dbloader]---------------------------------------------" << endl;
   LOG_TRACE(lcutrace,"[LCU]从数据库读取LCU流程定义开始.");
   
   JTable *rst = pcon->CreateJTable();
   // JTable *rst = pcon->getJTable();
   //cout<<"//取得所有最新版本得流程的名字和id"<< endl;
   try {
   	    //rst->Clear();
		//rst->AddSql("select t.ID, t.NAME, nvl(t.displayname,'') DISPLAYNAME, nvl(t.description,'') DESCRIPTION");
		//rst->AddSql(" from TD_BPM_DEFINITION t , TD_BPM_EXT_ATTRIBUTE ex where ex.name = 'category' and ex.value='lcu' and ex.element= t.id and t.version_ = ");
		//rst->AddSql("(select max(version_) from TD_BPM_DEFINITION t2 where t2.name=t.name) ");// order by t.ID");
		string sql = "select v.ID, v.NAME from (";
		sql += "select distinct(t2.name) as NAME, max(t2.id) as ID ";
		sql += " from TD_BPM_DEFINITION t2  group by t2.name) v ,";
		sql += " TD_BPM_EXT_ATTRIBUTE ex ";
		sql += " where ex.name = 'category' and ex.value='lcu' and ex.element= v.ID ";
		sql += " order by 2";

		rst->AddSql(sql);
		rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);
		rst->ExecSelect();

		//取得所有最新版本得流程的名字和id
	    //cout<<"[LCU]取得所有最新版本得流程的名字和id"<< endl;

		while(rst->Fetch()){
			id = rst->GetInt64("ID");
			name = rst->GetString("NAME");

			//cout << "[LCU]Load Definition:id=" << id << " name=" << name << endl;
			//pDef = this->loadOneDefinition(id,this->factory);
			pDef = PProcessDefinition(new ProcessDefinition());
			pDef->setId(id);
			pDef->setName(name);
			pDef->setDescription("");
			//增加到流程组里去
			str_defs.insert(base_ProcessDefinitions::value_type(name, pDef));
			id_defs.insert(map_ProcessDefinitions::value_type(id, pDef));
			count++ ;
	    }//end while
        delete rst;
    } catch (Exception & e){
		string info = "[LCU_DBLoader]无法读取流程定义:";
		info += e.toString();
		LOG_TRACE(lcutrace, info.c_str());
		lcutrace.alert(Grade::MAJOR, e.getCode(), e ,info.c_str());
		delete rst;
    	throw;
    } catch(...){       
       delete rst;
       LOG_TRACE(lcutrace, "读数据库其它异常");
       throw;
    }   
	//cout << "[LCU]从数据库读取流程定义, 合法流程: "<< count <<" 件. 总计：" << count  <<" 件."<< endl;
	//cout<<"[LCU]Load 读取节点的Delegations"<< endl;
	//cout << "[LCU]Loading . .";
	loadDelegations();
	//cout<<"[LCU]Load 读取流程的节点"<< endl;
	//cout << " . .";
	loadNodes();
	//cout << " . .";
	//cout<<"[LCU]Load 读取流程的连接"<< endl;
	loadTransitions();
	//cout << " . .";
	setActions();
	//cout << " . .";
	setEndNodes();
	//cout << " . .";
	//cout<<"[LCU]Load 读取流程的属性"<< endl;
	//setExtendsAttributes(str_defs);//设置一个等于两个都设置了
	setExtendsAttributes(id_defs);//以id来设置有唯一性比name好
	//cout << " . . Done"<<endl;
	map_pDelegation.clear();
	map_pNode.clear();
    LOG_TRACE_P1(lcutrace,"[LCU]从数据库读取流程定义, 合法流程: %d 件.", count);
	//cout << "[LCU][dbloader]---------------------------------------------" << endl;
}


PProcessDefinition UDBIDBDefinitionsLoader::loadbyId(const int64_t & def_id){
	PProcessDefinition pDef;
	if ( def_id <= 0) return pDef;
	//cout << "[LCU][loader]----------------------------------------" << endl;
	int64_t id;
	string name, displayname, description;
	int count = 0;
	UDBIDefBuilder builder;
	//开一个数据库连接
	JTable *rst = pcon->CreateJTable();
	try {
		rst->AddSql("select t.ID, t.NAME, nvl(t.displayname,'') DISPLAYNAME, nvl(t.description,'') DESCRIPTION ");
		rst->AddSql(" from TD_BPM_DEFINITION t  , TD_BPM_EXT_ATTRIBUTE ex where ex.name = 'category' and ex.value='lcu' and ex.element= t.id and t.ID = ");
		rst->AddSql(string(builder.int64str(def_id)));
		rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);
		rst->ExecSelect();		
	} catch (Exception & e){
		string info = "无法从数据库读取流程定义:";
		info += e.toString();
		LOG_TRACE(lcutrace, info.c_str());
		lcutrace.alert(Grade::MAJOR, e.getCode(), e ,info.c_str());
		delete rst;
		throw;
	}
	//取得所有最新版本得流程的名字和id

	LOG_TRACE_P1(lcutrace,"[LCU]从数据库读取流程定义开始.%s", "");
	
	while(rst->Fetch()){
	  try {

		id = rst->GetInt64("ID");
		name = rst->GetString("NAME");
		description = rst->GetString("DESCRIPTION");
		//cout << "[LCU]Load Definition:id=" << id << " name=" << name << endl;
		LOG_TRACE_P1(lcutrace,"[LCU]从数据库读取流程定义, Definition.name=%s", name.c_str());
		
		pDef = this->loadOneDefinition(id,this->factory);
		
		pDef->setId(id);
		pDef->setName(name);
		pDef->setDescription(description);
		
		id_defs.erase(id);
		//str_defs.erase(name);
		//增加到流程组里去
		//str_defs.insert(base_ProcessDefinitions::value_type(name, pDef));
		id_defs.insert(map_ProcessDefinitions::value_type(id, pDef));
        
		builder.setExtendsAttribute(pDef,id);//设置一个等于两个都设置了
		count++ ;
      } catch (exception & ex){
		cout << "[LCU]读取流程定义 throw:"  << ex << endl;	   	
      } catch (...){
		cout << "[LCU]读取流程定义失败.id:" << id << " name:" << name <<endl;        
      }
	}//end while
	delete rst;	
	LOG_TRACE_P1(lcutrace,"[LCU]从数据库读取流程定义完成, 共计: %d 件.", count);
	//cout << "[LCU][loader]----------------------------------------" << endl;
    return pDef;
}


PProcessDefinition UDBIDBDefinitionsLoader::loadbyName(const string & defname){
	PProcessDefinition pDef;
	if(defname == "") return pDef;
	//cout << "[LCU][loader]----------------------------------------" << endl;

	int64_t id;
	string name, displayname, description;
	int count = 0;

	//开一个数据库连接
	JTable *rst = pcon->CreateJTable();
	try{
		rst->AddSql("select t.ID, t.NAME, nvl(t.displayname,'') DISPLAYNAME, nvl(t.description,'') DESCRIPTION");
		rst->AddSql(" from TD_BPM_DEFINITION t , TD_BPM_EXT_ATTRIBUTE ex where ex.name = 'category' and ex.value='lcu' and ex.element= t.id and t.NAME = :DEFNAME");
		rst->AddSql("  and t.version_ = (select max(version_) from TD_BPM_DEFINITION t2 where t2.name=t.name) order by t.ID");
		rst->SetParam(":DEFNAME",string(defname));
		rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);

		rst->ExecSelect();

	} catch (Exception & e){
		
		string info = "无法从数据库读取流程定义:";
		info += e.toString();
		LOG_TRACE(lcutrace, info.c_str());
		lcutrace.alert(Grade::MAJOR, e.getCode(), e ,info.c_str());
	    delete rst;
		throw;
	}
	//取得所有最新版本得流程的名字和id
		
	LOG_TRACE_P1(lcutrace,"[LCU]从数据库读取流程定义开始.%s", "");
	while(rst->Fetch()){
      try {
			id = rst->GetInt64("ID");
			name = rst->GetString("NAME");
			description = rst->GetString("DESCRIPTION");
			//cout << "[LCU]Load Definition:id=" << id << " name=" << name << endl;
			LOG_TRACE_P1(lcutrace,"[LCU]从数据库读取流程定义, Definition.name=%s", name.c_str());
			pDef = this->loadOneDefinition(id,this->factory);
			
			pDef->setId(id);
			pDef->setName(name);
			pDef->setDescription(description);
			
			id_defs.erase(id);
			str_defs.erase(name);
			//增加到流程组里去
			str_defs.insert(base_ProcessDefinitions::value_type(name, pDef));
			id_defs.insert(map_ProcessDefinitions::value_type(id, pDef));
			//cout<<"//取得流程的扩展属性"<< endl;
			UDBIDefBuilder builder;
			builder.setExtendsAttribute(pDef,id);
			//设置一个等于两个都设置了
			count++ ;
      } catch (exception & ex){
			cout << "[LCU]读取流程定义 throw:"  << ex << endl;		
			delete rst;
			throw;
      } catch (...){
			cout << "[LCU]读取流程定义失败.id:"<< id << " name:" << name <<endl;			
			delete rst;
			throw;
      }
    }//end while
	delete rst;
	//cout << "[LCU]从数据库读取指定流程定义完成, 共计: "<< count <<" 件"<< endl;
	LOG_TRACE_P1(lcutrace,"[LCU]从数据库读取流程定义完成, 共计: %d 件.", count);
	//cout << "[LCU][loader]----------------------------------------" << endl;
	return pDef;
}


void UDBIDBDefinitionsLoader::setExtendsAttributes(map_ProcessDefinitions& pDefs){
	//string strSQL ="select e.element as DEF_ID ,t.name as DEF_NAME, nvl(e.name,'') as ATTRKEY, nvl(e.value,'') as ATTRVALUE from TD_BPM_EXT_ATTRIBUTE e , TD_BPM_DEFINITION t ";
	//strSQL += "where  e.element=t.id  and t.version_ = (select max(version_) from TD_BPM_DEFINITION t2 where t2.name=t.name) order by e.element";

	string strSQL ="select e.element as DEF_ID , e.name as ATTRKEY, nvl(e.value,'') as ATTRVALUE from TD_BPM_EXT_ATTRIBUTE e , TD_BPM_DEFINITION t ";
	strSQL += "where  e.element=t.id  order by e.element";
	//把旧版本的读过来也没有关系，反正用不到
	
	JTable *rst = pcon->CreateJTable();
	rst->AddSql(strSQL);
	rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);
	try {
		rst->ExecSelect();
		int64_t id, his_id = 0;
		string name,attrkey,attrvalue;
	
		PProcessDefinition pDef;
	
		while(rst->Fetch()){
			id = rst->GetInt64("DEF_ID");
			//name = rst->GetString("DEF_NAME");
			attrkey = rst->GetString("ATTRKEY");
			attrvalue = rst->GetString("ATTRVALUE");
			if(id != his_id){
		   		//base_ProcessDefinitions::const_iterator it = pDefs.find(name);
		   		map_ProcessDefinitions::const_iterator it = pDefs.find(id);
		   		if (it != pDefs.end()){
		   			pDef = it->second;
		   		}else{
		   		   continue;
		   		}
	   			his_id = id;
			}		
			pDef->setExtendedAttribute(attrkey,attrvalue);
		}
    }catch(...){
       delete rst;
       throw;
    }
	delete  rst;
}



void UDBIDBDefinitionsLoader::loadDelegations(){
	PDelegation  pDel; int64_t id;
	string classname, configuration;
    if(pcon==0) cout << "pcon->CreateJTable()error" <<endl;
	JTable * rst = pcon->CreateJTable();
	rst->AddSql("select t.ID, nvl(t.classname,'') CLASSNAME, nvl(t.CONFIGURATION,'') CONFIGURATION");
	rst->AddSql(" from TD_BPM_DELEGATION t ");
	try {			
		rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);	
		rst->ExecSelect();
	
		while(rst->Fetch()){
			id = rst->GetInt64("ID");
			classname = rst->GetString("CLASSNAME");
			configuration = rst->GetString("CONFIGURATION");
	
			pDel = PDelegation(new Delegation());
			pDel->setHandleFactory(this->factory);
			pDel->setId(id);
			pDel->setClassName(classname);
			pDel->setConfiguration(configuration);
	
			this->map_pDelegation.insert(pair<int64_t,PDelegation>(id,pDel));
		}
		delete  rst;
    }catch(Exception & e){
       cout << e <<endl;
       delete rst;
       throw ;
    }
}

void UDBIDBDefinitionsLoader::setActions(){
	PNode pNode; PState pState; PDelegation pdelegation;
	int64_t element, actiondelegation;

	JTable * rst = pcon->CreateJTable();
	try{
	rst->AddSql("select t.id, nvl(t.element, -1) ELEMENT, nvl(t.actiondelegation,-1) ACTDELEG   from TD_BPM_ACTION t,TD_BPM_NODE n");
	rst->AddSql(" where t.element = n.id order by t.element , t.index_");
	rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);
	rst->ExecSelect();
	while(rst->Fetch()){
       	element = rst->GetInt64("ELEMENT");
	    actiondelegation = rst->GetInt64("ACTDELEG");

        if(element > 0 && actiondelegation > 0){
       		pdelegation = this->findPDelegationInMap(actiondelegation);
   			pNode = findPNodeInMap(element);
       		if(pdelegation!=0 && pNode !=0){ //两个都存在的情况下
         		PAction action(new Action(pdelegation));
         		pState = dynamic_pointer_cast < State >(pNode);
		  		pState->addAction(action);
       		}

        }
	}
    delete rst;
    } catch(...){       
       delete rst;
       LOG_TRACE(lcutrace, "异常读数据库TD_BPM_ACTION ,TD_BPM_NODE");
       throw;
    }   
}

void  UDBIDBDefinitionsLoader::setEndNodes(){
    int64_t element;
	string name;
	string value;
	string key = "alert";
	PEndState pend;
	PNode pNode;
	int64_t lastnode = -1;

	JTable * rst = pcon->CreateJTable();
	try{
	rst->AddSql(" select e.NAME,nvl(e.value,'') as VALUE,nvl(e.element, -1) ELEMENT from TD_BPM_EXT_ATTRIBUTE e, TD_BPM_NODE node where e.element = node.id and node.class = 'EndState' and (e.name = 'alert' or e.name = 'commit')");
	rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);

	rst->ExecSelect();

	while(rst->Fetch()){
       	element = rst->GetInt64("ELEMENT");
	  	name =  rst->GetString("NAME");
 	    value = rst->GetString("VALUE");

    	if(element!=lastnode) {
   			pNode = findPNodeInMap(element);
       		if(pNode !=0){ //两个都存在的情况下
         		pend = dynamic_pointer_cast < EndState >(pNode);
         		if(name == key){
         			pend->setAlertCode(atoi(value.c_str()));
         		} else {
         		    pend->setCommit(value);
         		}
         		lastnode = element;
       		}
   	     } else {
   	     	if(name == key){
         		pend->setAlertCode(atoi(value.c_str()));
         	} else {
         		pend->setCommit(value);
         	}
   	     }
	}
    delete rst;
    } catch(...){       
       delete rst;
       LOG_TRACE(lcutrace, "异常读数据库TD_BPM_EXT_ATTRIBUTE , TD_BPM_NODE ");
       throw;
    }   
}

//设置本流程所有的Transition */
void UDBIDBDefinitionsLoader::loadTransitions(){
   //字段名 id, name, description, definition, FROM_, TO_
   PTransition  pTran;
	string name, description;
	int64_t id,from_id,to_id;
	JTable * rst = pcon->CreateJTable();
	try{
	rst->AddSql("select ID ,nvl(name,'') NAME ,nvl(FROM_,-1) FROM_ , nvl(TO_,-1) TO_ from TD_BPM_TRANSITION t order by id");
	rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);
	rst->ExecSelect();
	PNode  pfromNode, ptoNode;
    while(rst->Fetch()){
       id = rst->GetInt64("ID");
       name = rst->GetString("NAME");
       //description = rst->GetString("DESCRIPTION");
       from_id = rst->GetInt64("FROM_");
       to_id   = rst->GetInt64("TO_");

       pTran = PTransition(new Transition());
       pTran->setId(id);
       pTran->setName(name);
       //pTran->setDescription(description);
       if(from_id < 0 || to_id < 0) continue;
       pfromNode = findPNodeInMap(from_id);
       if(pfromNode){
     	    pfromNode->addLeavingTransition(pTran);
     	    //cout << "addLeavingTransition() from:"<<pfromNode->getName()<<endl;
       }
       ptoNode = findPNodeInMap(to_id);
       if(ptoNode){
       	 pTran->setTo(ptoNode.get());
       	 //cout << "addLeavingTransition() to:"<<ptoNode->getName()<<endl;
       }
   }
   delete rst;
   } catch(...){       
       delete rst;
       LOG_TRACE(lcutrace, "异常读数据库TD_BPM_EXT_ATTRIBUTE , TD_BPM_NODE ");
       throw;
    }   
}

/**
 * 设置本流程所有的State
 */
void UDBIDBDefinitionsLoader::loadNodes(){
	string nodeclass,name,description;
	PNode pstate;
	int64_t id,deleg;
	int endcode;
	string endstr;
	int64_t def_id;
	PProcessDefinition pDefinition;
	JTable * rst = pcon->CreateJTable();
	try{
	rst->AddSql("select ID, CLASS,nvl(name,'') as NAME,nvl(endcode,0) as ENDCODE ,nvl(n.delegation,-1) DELEGATION , n.definition as DEF_ID from  TD_BPM_NODE n");
	rst->AddSql(" where (n.class='StartState' or n.class='State' or n.class='EndState' or n.class='Decision' or n.class='ProcessState') ");
	rst->AddSql(" order by ID");
	rst->SetPrefetchRecs(UDBI_Prefetch_SIZE);
	rst->ExecSelect();
	while(rst->Fetch()){
			id = rst->GetInt64("ID");
			def_id = rst->GetInt64("DEF_ID");
			nodeclass = rst->GetString("CLASS");
			name = rst->GetString("NAME");

			endcode = rst->GetInt("ENDCODE");
			deleg = rst->GetInt64("DELEGATION");
			//description = rst->GetString("DESCRIPTION");

			pDefinition = findPProcessDefinitionInMap(def_id);
			if(pDefinition){//不存在就不加
				if (nodeclass == "State") {
				 	 pstate = PState(new State());
				}else if(nodeclass == "EndState"){
					 PEndState pend(new EndState());
					 pend->setEndCode(endcode);
					 endstr = rst->GetString("ENDCODE");
					 if(endcode == 0 && endstr!="0") {
					 	ostringstream oos;
					 	oos <<"[LCU]Error EndNode.name =" << name << " endcode=" << endstr << endl;
						LOG_TRACE(lcutrace, oos.str().c_str());
					 }
					 pstate = pend;
				}else if( nodeclass == "StartState"){
				     PStartState pstartstate(new StartState());
				     pstate = pstartstate;
				     pDefinition->setStartState(pstartstate);
				}
			    if(deleg > 0){
		             PDelegation pdelegation = this->findPDelegationInMap(deleg);
					 if(nodeclass == "Decision"){
		      			 PDecision pDeci = PDecision(new Decision(pdelegation));
		      			 pstate = pDeci;
		          	 } else if (nodeclass == "ProcessState"){
						PProcessState pstart2 = PProcessState(new ProcessState(pdelegation));
						pstate = pstart2;
				     }
				}
				if(pstate){
					pstate->setId(id);
					pstate->setName(name);
					//pstate->setDescription(description);
					//添加到节点堆里

					pDefinition->addNode(pstate);
				    this->map_pNode.insert(pair<int64_t,PNode>(id,pstate));
			    }
			}
    }
    delete rst;
    } catch(...){       
       delete rst;
       LOG_TRACE(lcutrace, "异常读数据库 TD_BPM_NODE ");
       throw;
    }   
}

/**
 * 遍历所有的Transition，
 * 在Node里找出 from_id 对应的Node，
 * 把自己设成他的leavingTrans,
 * 找出to_id把 这个 toNode设置成自己的TO
 */
//void UDBIDefBuilder::linkTransitions(){
//}

/**找到那个Delegation//没有找到就是一个空指针*/
PDelegation UDBIDBDefinitionsLoader::findPDelegationInMap(const int64_t & id){
	map_PDelegation::const_iterator pos = map_pDelegation.find(id);
	PDelegation pDelegation;
    if(pos != map_pDelegation.end()){
        pDelegation = pos->second;
    }
    return  pDelegation;
}

/**找到那个Delegation//没有找到就是一个空指针*/
PNode UDBIDBDefinitionsLoader::findPNodeInMap(const int64_t & id){
	map_PNode::const_iterator pos = map_pNode.find(id);
	PNode pPNode;
    if(pos != map_pNode.end()){
        pPNode = pos->second;
    }
    return  pPNode;
}

PProcessDefinition UDBIDBDefinitionsLoader::findPProcessDefinitionInMap(const int64_t & id){
    map_ProcessDefinitions::const_iterator pos = id_defs.find(id);
	PProcessDefinition pPNode;
    if(pos != id_defs.end()){
        pPNode = pos->second;
    }
    return  pPNode;

};


PProcessDefinition UDBIDBDefinitionsLoader::loadOneDefinition(const int64_t& def_id,PHandleFactory _factory){
	//cout << "/*设置开始*/" <<endl;

	PProcessDefinition pDef(new ProcessDefinition());
	pDef->setId(def_id);

	UDBIDefBuilder * defbuilder = new UDBIDefBuilder();

 	defbuilder->setDef_ID(def_id);
 	defbuilder->setHandleFactory(_factory);
 	defbuilder->setPDefinition(pDef);

	//cout << "/*设置开始节点StartState节点*/" <<endl;
   	defbuilder->setStartStateNode(def_id);

	//cout << "/*设置本流程所有的Delegation*/" << endl;
	defbuilder->setDelegationSet(def_id);

	//cout << "/*设置本流程所有的State*/" <<endl;
	defbuilder->setStateSet(def_id);

	//cout << "/*设置本流程所有的Decision*/" <<endl;
	defbuilder->setDecisionSet(def_id);

	//cout << "/*设置本流程的EndState节点*/" <<endl;

	//cout << "/*设置本流程的节点的活动*/" << endl;
	defbuilder->setActionSet(def_id);

	//cout << "/*设置本流程所有的Transition*/" <<endl;
	defbuilder->setTransitionSet(def_id);

	defbuilder->setEndNodeSet(def_id);
	//cout << "/*把所有的都连接起来*/" <<endl;
	//里面设置扩展属性
	//defbuilder->setExtendsAttribute(pDef,def_id);

	delete defbuilder;
    //cout << "/*做完后清空一下流程构件器*/" <<endl;
	return pDef;
};


void UDBIDefBuilder::setExtendsAttribute(PProcessDefinition pDef,const int64_t& def_id){
	//cout << "[LCU]为单一流程设置扩展属性" <<endl;
	JTable *rst = pcon->CreateJTable();
	rst->AddSql("select nvl(e.name,'') as ATTRKEY, nvl(e.value,'') as ATTRVALUE from TD_BPM_EXT_ATTRIBUTE e ");
    rst->AddSql("where e.element=");
	rst->AddSql(int64str(def_id));
	rst->ExecSelect();

	while(rst->Fetch()){
		pDef->setExtendedAttribute(rst->GetString("ATTRKEY"),rst->GetString("ATTRVALUE"));
	}
	delete  rst;
}


/**设置本流程所有的Delegation*/
void UDBIDefBuilder::setDelegationSet(const int64_t& def_id){
    //select id,definition,classname,configuration
	PDelegation  pDel; int64_t id;
	string classname, configuration;

	JTable * rst = pcon->CreateJTable();
	rst->AddSql("select t.ID, nvl(t.classname,'') CLASSNAME, nvl(t.CONFIGURATION,'') CONFIGURATION");
	rst->AddSql(" from TD_BPM_DELEGATION t where t.definition =");
	rst->AddSql(int64str(def_id));
	rst->ExecSelect();
	while(rst->Fetch()){
		id = rst->GetInt64("ID");
		classname = rst->GetString("CLASSNAME");
		configuration = rst->GetString("CONFIGURATION");

		pDel = PDelegation(new Delegation());
		pDel->setHandleFactory(this->handleFactory);
		pDel->setId(id);
		pDel->setClassName(classname);
		pDel->setConfiguration(configuration);

		this->map_pDelegation.insert(pair<int64_t,PDelegation>(id,pDel));
	}

	delete  rst;
}

void UDBIDefBuilder::setActionSet(const int64_t& def_id){
	// t.id,t.element_type,t.element,t.eventtype,t.actiondelegation
	PNode pNode; PState pState; PDelegation pdelegation;
	int64_t element, actiondelegation;

	JTable * rst = pcon->CreateJTable();
	rst->AddSql("select t.id, nvl(t.element, -1) ELEMENT, nvl(t.actiondelegation,-1) ACTDELEG from TD_BPM_ACTION t,TD_BPM_NODE n");
	rst->AddSql(" where t.element = n.id  and n.definition =");
	rst->AddSql(int64str(def_id));
	rst->ExecSelect();
	while(rst->Fetch()){
       	element = rst->GetInt64("ELEMENT");
		actiondelegation = rst->GetInt64("ACTDELEG");

		if(element > 0 && actiondelegation > 0){
       		pdelegation = this->findPDelegationInMap(actiondelegation);
       		pNode = pDefinition->getNodeByID(element);
       		if(pdelegation!=0 && pNode !=0){ //两个都存在的情况下
         		PAction action(new Action(pdelegation));
         		pState = dynamic_pointer_cast < State >(pNode);
				pState->addAction(action);
       		}
        }
	}
    delete rst;
}

//设置本流程所有的Transition */
void UDBIDefBuilder::setTransitionSet(const int64_t& def_id){
   //字段名 id, name, description, definition, FROM_, TO_
	PTransition  pTran;
	string name, description;
	int64_t id,from_id,to_id;
	JTable * rst = pcon->CreateJTable();
	rst->AddSql("select ID ,nvl(name,'') NAME ,nvl(FROM_,-1) FROM_ , nvl(TO_,-1) TO_ from TD_BPM_TRANSITION t where t.definition =");
	rst->AddSql(int64str(def_id));
	rst->ExecSelect();
	PNode  pfromNode, ptoNode;
    while(rst->Fetch()){
       id = rst->GetInt64("ID");
       name = rst->GetString("NAME");
       //description = rst->GetString("DESCRIPTION");
       from_id = rst->GetInt64("FROM_");
       to_id   = rst->GetInt64("TO_");

       pTran = PTransition(new Transition());
       pTran->setId(id);
       pTran->setName(name);
       //pTran->setDescription(description);

       pfromNode = pDefinition->getNodeByID(from_id);
       if(pfromNode){
     	    pfromNode->addLeavingTransition(pTran);
     	    //cout << "addLeavingTransition() from:"<<pfromNode->getName()<<endl;
       }
       ptoNode = pDefinition->getNodeByID(to_id);
       if(ptoNode){
       	 pTran->setTo(ptoNode.get());
       	 //cout << "addLeavingTransition() to:"<<ptoNode->getName()<<endl;
       }
   }
   delete rst;
}

/**设置本流程的StartState节点*/
void UDBIDefBuilder::setStartStateNode(const int64_t& def_id){
	//n.id ,n.name,n.description,n.delegation , nvl(description, '') DESCRIPTION
	PStartState pstart(new StartState());
	int64_t id = 0; string name,description;

	JTable * rst = pcon->CreateJTable();
	rst->AddSql("select ID ,nvl(name,'') NAME from TD_BPM_NODE n ");
	rst->AddSql(" where  n.class='StartState' and n.definition =");
	rst->AddSql(int64str(def_id));
	rst->ExecSelect();

	if(rst->Fetch()){
		id = rst->GetInt64("ID");
		name = rst->GetString("NAME");
		//description = rst->GetString("DESCRIPTION");

		pstart->setId(id);
		pstart->setName(name);
		//pstart->setDescription(description);
		this->pDefinition->addNode(pstart);
	}
	this->pDefinition->setStartState(pstart);

	delete rst;
}

/**
 * 设置本流程所有的State
 */
void UDBIDefBuilder::setStateSet(const int64_t& def_id){
	//字段名：, nvl(description, '') DESCRIPTION
	// id, CLASS, name, description, definition, SWIMLANE, ASSIGNMENTREQUIRED, STATEAUTHENTICATION, STABLE, delegation, CORRESPONDINGJOIN, CORRESPONDINGFORK;
	string nodeclass,name,description;
	PState pstate;
	int64_t id;
	int endcode;

	JTable * rst = pcon->CreateJTable();
	rst->AddSql("select ID, CLASS,nvl(name,'') NAME,nvl(endcode,0) ENDCODE from  TD_BPM_NODE n");
	rst->AddSql(" where (n.class='State' or n.class='EndState') and n.definition = ");
	rst->AddSql(int64str(def_id));
	rst->AddSql(" order by ID");
	rst->ExecSelect();
	while(rst->Fetch()){
		id = rst->GetInt64("ID");
		nodeclass = rst->GetString("CLASS");
		name = rst->GetString("NAME");
		//description = rst->GetString("DESCRIPTION");
		 endcode = rst->GetInt("ENDCODE");
      if(nodeclass == "EndState"){
         PEndState pend(new EndState());
         pend->setEndCode(endcode);
         pstate = pend;
      }else {
         pstate = PState(new State());
      }
      pstate->setId(id);
      pstate->setName(name);
      //pstate->setDescription(description);
      //添加到节点堆里
		this->pDefinition->addNode(pstate);
    }
    delete rst;
}

/**设置本流程所有的Decision*/
void UDBIDefBuilder::setDecisionSet(const int64_t& def_id){
	 //n.id ,n.name,n.description,n.delegation   ,nvl(description, '') DESCRIPTION
	string nodeclass,name,description;
	PDecision pDeci;
	PProcessState pstart2;
	PDelegation pdelegation;
	int64_t id,deleg;

	JTable * rst = pcon->CreateJTable();
	rst->AddSql("select ID, CLASS ,nvl(name,'') NAME, nvl(n.delegation,-1) DELEGATION from TD_BPM_NODE n");
	rst->AddSql(" where (n.class='Decision' or n.class='ProcessState') and n.definition = ");
	rst->AddSql(int64str(def_id));
	rst->ExecSelect();
	while(rst->Fetch()){
		id = rst->GetInt64("ID");
		nodeclass = rst->GetString("CLASS");
		name = rst->GetString("NAME");
		//description = rst->GetString("DESCRIPTION");
      deleg = rst->GetInt64("DELEGATION");

      if(deleg > 0){
         pdelegation = this->findPDelegationInMap(deleg);
         if(pdelegation){ //设置方法与Action不同
            if(nodeclass == "Decision"){
      			pDeci = PDecision(new Decision(pdelegation));
      			pDeci->setId(id);
      			pDeci->setName(name);
      			//pDeci->setDescription(description);
            	this->pDefinition->addNode(pDeci);
          	} else if (nodeclass == "ProcessState"){
      		   pstart2 = PProcessState(new ProcessState(pdelegation));
      		   pstart2->setId(id);
      		   pstart2->setName(name);
      		   //pstart2->setDescription(description);
      		   //添加到节点堆里
      		   this->pDefinition->addNode(pstart2);
		      }
        }
      }
   }
   delete rst;
}

void UDBIDefBuilder::setEndNodeSet(const int64_t& def_id){
	 int64_t element;
	string name;
	string value;
	string key = "alert";
	PEndState pend;
	PNode pNode;
	int64_t lastnode = -1;

	JTable * rst = pcon->CreateJTable();

	rst->AddSql(" select e.NAME,nvl(e.value,'') as VALUE,nvl(e.element, -1) ELEMENT from TD_BPM_EXT_ATTRIBUTE e, TD_BPM_NODE node where e.element = node.id and node.class = 'EndState' and (e.name = 'alert' or e.name = 'commit') and node.definition = ");
	rst->AddSql(int64str(def_id));
	rst->ExecSelect();

	while(rst->Fetch()){
       	element = rst->GetInt64("ELEMENT");
	  	name =  rst->GetString("NAME");
 	    value = rst->GetString("VALUE");
    	if(element!=lastnode) {
   			pNode = pDefinition->getNodeByID(element);
       		if(pNode !=0){ //两个都存在的情况下
         		pend = dynamic_pointer_cast < EndState >(pNode);
         		if(name == key){
         			pend->setAlertCode(atoi(value.c_str()));
         		} else {
         		    pend->setCommit(value);
         		}
         		lastnode = element;
       		}
   	     } else {
   	     	if(name == key){
         		pend->setAlertCode(atoi(value.c_str()));
         	} else {
         		pend->setCommit(value);
         	}
   	     }
	}
    delete rst;

}



/**
 * 遍历所有的Transition，
 * 在Node里找出 from_id 对应的Node，
 * 把自己设成他的leavingTrans,
 * 找出to_id把 这个 toNode设置成自己的TO
 */
//void UDBIDefBuilder::linkTransitions(){
//}

/**找到那个Delegation//没有找到就是一个空指针*/
PDelegation UDBIDefBuilder::findPDelegationInMap(const int64_t & id){
	map_PDelegation::const_iterator pos = map_pDelegation.find(id);
	PDelegation pDelegation;
    if(pos != map_pDelegation.end()){
        pDelegation = pos->second;
    }
    return  pDelegation;
}



}
