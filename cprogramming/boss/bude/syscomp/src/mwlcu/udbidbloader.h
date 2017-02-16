#ifndef EBPM_UDBI_DBDEFINITIONS_DAO_LOADER_H
#define EBPM_UDBI_DBDEFINITIONS_DAO_LOADER_H

#include "lcu/lcu.h"
#include "udbi/udbi.h"
#include <string>
#include <sys/types.h>
#include <sstream>
#include <map>
#include <vector>
#include "log4cpp/Logger.h"

using namespace UDBI;
using namespace std;
using namespace log4cpp;
//extern JConnect* pcon;

namespace ebpm4c{

class UDBIDefBuilder;
class UDBIDBDefinitionsLoader;


/**
* 把当前数据库里的所有流程都取出来
*/
class UDBIDBDefinitionsLoader: public DefinitionLoaderDao
{
	private:
	    /**公用数据库控制台*/
	  JConnect* pcon;
      PHandleFactory  factory;
      base_ProcessDefinitions  str_defs;
      map_ProcessDefinitions  id_defs;
      //bool isInited;
      Logger& lcutrace;
      enum {NONE = 0, UDBI_Prefetch_SIZE = 1000};
	public :
	      UDBIDBDefinitionsLoader():lcutrace(Logger::getLogger("gboss.lcu.trace")){
	      	 pcon = jdb::getConnect();
		  };
	      virtual ~UDBIDBDefinitionsLoader();

	      void refresh();

	      PProcessDefinition refreshDefinitionById(const int64_t & id);
	      PProcessDefinition refreshDefinitionByName(const string & name);

	      PProcessDefinition getDefinitionById(const int64_t & id);
	      PProcessDefinition getDefinitionByName(const string & name);

	      void setHandleFactory(PHandleFactory factory);
	      void load();
	      PProcessDefinition loadbyName(const string& name);
	      PProcessDefinition loadbyId(const int64_t & def_id);
	      bool removebyName(const string& name);
	      
	      void loadErrorCodeTable(map<int,string>& errtable_map); 
	private :
		/**
		 * Load all Definitions form database;
		 * 继承实现并父类的动作
		 */
		void load(PHandleFactory factory);
		/**读入一件流程*/
		PProcessDefinition loadOneDefinition(const int64_t& def_id,PHandleFactory factory);

		void setExtendsAttributes(map_ProcessDefinitions& pDefs);

		inline string int64str(const int64_t& in){
			 //std::basic_stringstream <char> ss;//不能超过int32
			 stringstream ss; string str;
			 ss << in; ss >> str;
			 return str;
		};
    protected:
		void loadDelegations();
		void setActions();
		void loadNodes();
		void loadTransitions();
		void setStartNodes();
		void setEndNodes();
	    /**暂存的集合*/
	    map_PDelegation map_pDelegation;
	    PDelegation findPDelegationInMap(const int64_t & id);
	     /**暂存的集合*/
	    map_PNode map_pNode;
	    PNode findPNodeInMap(const int64_t & id);
	    PProcessDefinition findPProcessDefinitionInMap(const int64_t & id);

};

/**
 * 读具体的一个流程，把所有相关元素都加进来.
 * 为了重用，请每次使用完后clear()里面的数据。
 */
class UDBIDefBuilder{
	private:
	  JConnect* pcon;
		/**指向那个流程对象*/
      PProcessDefinition pDefinition;
      PHandleFactory handleFactory;
      int64_t def_id_ ;
      /**暂存的集合*/
      map_PDelegation map_pDelegation;

      PDelegation findPDelegationInMap(const int64_t & id);

	public:

      UDBIDefBuilder(){
         pcon = jdb::getConnect();
      };
      virtual ~UDBIDefBuilder(){};

      /**设置流程ID*/
      void setDef_ID(const int64_t& _def_id){
         this->def_id_ = _def_id;
      };

      void setHandleFactory(PHandleFactory factory){
         this->handleFactory = factory;
      };

      /**设置流程指针*/
      void setPDefinition(const PProcessDefinition & _pDefinition){
         this->pDefinition = _pDefinition;
      };

      inline string int64str(const int64_t& in){
         //std::basic_stringstream <char> ss;
         stringstream ss;
         string str;
         ss << in; ss >> str;
         return str;
      };

      void setExtendsAttribute(PProcessDefinition pdef,const int64_t& def_id);

		/**清除对象内的数据*/

		/**设置本流程所有的Delegation*/
		void setDelegationSet(const int64_t& def_id);

		/**设置本流程所有的Action*/
      void setActionSet(const int64_t& def_id);

      /**设置本流程所有的Transition 把所有的Node串起来*/
      void setTransitionSet(const int64_t& def_id);

      /**设置本流程的StartState节点*/
      void setStartStateNode(const int64_t& def_id);

      /**设置本流程所有的State EndState节点*/
      void setStateSet(const int64_t& def_id);

      /**设置本流程所有的Decision*/
      void setDecisionSet(const int64_t& def_id);
       /**设置本流程所有的EndNode*/
      void setEndNodeSet(const int64_t& def_id);

};

}

#endif // EBPM_UDBI_DBDEFINITIONSLOADER_H

