#ifndef __MANAGER_H
#define __MANAGER_H

#include "SingletonPtr.h"

class Manager : public CSingletonPtr<Manager>
{
    friend class CSingletonPtr<Manager>;
protected:
Manager()
{
    cout << "Manager constructor called\n";
    m_count = 100;
    cout << "Manager constructor end\n";
} 

public:
~Manager()
{
    cout << "Manager destructor called\n";
}
public:
    void Print()
    {
        cout << "Manager::Print() called, m_count = "<< ++ m_count << endl;
    }
private:
    int m_count;
};

#endif
