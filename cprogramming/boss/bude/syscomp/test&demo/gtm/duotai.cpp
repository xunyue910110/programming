#include <iostream>
#include <vector>
#include <list>

using namespace std;

class Employee 
{
private:
	string  yourname;
	short department;
public:
	Employee(const string& name, int dept);
	virtual void print() const;
};

Employee::Employee(const string& name, int dept):yourname(name), department(dept) {}

void Employee::print() const
{
	cout << "yourname=" << yourname << '\t' << "department=" << department << '\n';
}

class Manager: public Employee 
{
private:
	short level;
public:
	Manager(const string & name, int dept, int lvl);
	void print() const;
};

Manager::Manager(const string&  name, int dept, int lvl):Employee(name, dept), level(lvl) {};

void Manager::print() const
{
	Employee::print();
	cout << "\t\tlevel=" << level << '\n';
};

//void print_list(const list<Employee *> &ee) 
/*
void print_list(const list<Employee &> &ee) 
{

	//for (list<Employee *>::const_iterator iter = ee.begin(); iter != ee.end(); ++iter) {
	for (list<Employee &>::const_iterator iter = ee.begin(); iter != ee.end(); ++iter) {
		//(*iter)->print();
		(*iter).print();
	}
}
*/
int main()
{

	Employee e("ZHOU", 1234);
	Manager  m("ZHANG", 1234, 2);

	Employee &i = e;
	i.print();
	Employee &j = e;
	j.print();
	//list<Employee *> empl;
	//list<Employee &> empl;

	//empl.push_back(&e);
	//empl.push_back(e);
	//empl.push_back(m);
	//empl.push_back(&m);
	//print_list(empl);
}

