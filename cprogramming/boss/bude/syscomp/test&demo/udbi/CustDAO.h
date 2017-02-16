#include "udbi.h"
#include "JConnect.h"
#include "MTable.h"

/**
 *
 */

const string SEL_BY_PK = "SEL_BY_PK";
const string SEL_BY_NBR = "SEL_BY_NBR";
const string SEL_BY_VIP = "SEL_BY_VIP";
const string UPD_NBR_BY_PK("UPD_NBR_BY_PK");
const string UPD_TYPE_BY_SOCIAL("UPD_TYPE_BY_SOCIAL"); 

namespace CustServAcct {
using namespace std;
using namespace UDBI;

class Cust {
public:
    Cust();
    virtual ~Cust();
    //string stateName();
public:
    string cust_id;
    int cust_seq_nbr;
    int cust_type_id;
    int social_type_id;
    string social_id;
    string cust_name;
    string sex;
    int vip_flag;
    string vip_nbr;
    string state;
    string state_date;
};

typedef vector<Cust *> PCust;

class CustDAO : public MTable {
public:
     CustDAO();
     virtual ~CustDAO();
     PCust jselect(const string &selBy, Cust &selCust);
     int jupdate(const string &updBy, Cust &updCust);
     int jdelete(const string &delBy, Cust &delCust);
     int jinsert(Cust &insCust);
};  
    
} // namespace CustServAcct

