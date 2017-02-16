#include <iostream>
#include <unistd.h>

#include "udbi.h"
#include "ProductDAO.h"

using namespace std;
using namespace UDBI;
using namespace ProductDom;

JConnect *pcon;

void prod_test();

main(int argc, char argv[])
{
	jdb::jinit("eboss", "crm1", "crm123");
	pcon = jdb::getConnect();
	
	try
	{
		pcon->TransBegin();
		prod_test();
		pcon->TransCommit();
	} catch (EUdbiException &err) 
	{
		cout << err << endl;
	}
			
	cout << "wait ... " << endl;
	sleep(6);
	
	return 0;	
}

void prod_test()
{
	ProductDAO *pp = new ProductDAO();
	string tmpStr("0001");
	VPProduct vp;
	vp = pp->jselect(SEL_BY_TYPE, tmpStr, tmpStr);
	vector<Product *>::const_iterator it = vp.begin();
	for (; it!=vp.end(); it++) {
		cout << "" << (*it)->ProductId << endl;
		cout << "" << (*it)->ProductName << endl;
	}
	
	delete pp;
}	

