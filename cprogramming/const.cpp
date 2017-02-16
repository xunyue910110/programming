    #include <iostream>
#include <stdio.h>
#include <vector>

#include "FastDelegate.h"
using namespace fastdelegate;
using namespace std;
int func1(int a){ cout << "global func1 called" << endl;}
class A;
class CA{
public:
    typedef int (CA::*pClassFun)(int, int);
public:
    int max(int a, int b){return a> b? a: b;}
    int min(int a, int b ) {return a< b? a:b;}
    int Result(pClassFun fun, int a, int b)
    {
        return (this->*fun)(a, b);
    }
public:
    pClassFun func;
};

typedef FastDelegate<void (void)> Callback;
void GlobalFunc()
{
    printf("GlobalFunc called\n");
}
static void StaticFunc()
{
    printf("StaticFunc called\n");
}

class Test{
    public:
    static void ClassStaticFunc() {
        printf("ClassStaticFunc called\n");
    }
    void ClassMemberFunc()
    {
        printf("ClassMemberFunc called\n");
    }
    virtual void VirtualMemberFunc()
    {
        printf("VirtualMemberFunc called\n");
    }
};

class Test2 : public Test
{
    public:
    virtual void VirtualMemberFunc()
    {
        printf("Test2::VirtualMemberFunc called\n");
    }
};

void TestCallback()
{
    Test t1;
    Test2 t2;
    Test * pt1 = &t2;
    Callback cb;
    cb.bind(GlobalFunc);
    cb();
    cb = Callback(StaticFunc);
    cb();
    cb.bind(&Test::ClassStaticFunc);
    cb();
    cb.bind(&t1,&Test::ClassMemberFunc);
    //cb  = Callback(&t1, &Test::ClassMemberFunc);
    cb();
    cb.bind(&t1, &Test::VirtualMemberFunc);
    cb();
    cb.bind(&t2, &Test::VirtualMemberFunc);
    cb();

    cb.bind(pt1, &Test::VirtualMemberFunc);
    cb();
}
void TestMemberFunc()
{
    CA ca;
    int a = 3;
    int b = 4;
    printf("test member func\n");
    printf("Max reuslt = %d\n", ca.Result(&CA::max, a, b));
     
}
void TestMemberFunc2()
{
/*
    int a  = 3;
    int b = 4;
    typedef int (CA::*pf)( int, int);
    pf  f1 = &CA::max;
    CA ca;
    //cout << (*f1)(&ca, a, b); 
   */
    int a = 3;
    int b = 4;
    CA ca;
    ca.func = &CA::max;
    //(*(ca.CA::func))(a, b);
}
class VectorRef 
{
    std::vector<int> vecInts;
    public:
    VectorRef(int size = 5){
       for ( int i = 0; i < size; i++)
        vecInts.push_back(i); 
    }
/*
std::vector<int>& GetVecInts()
{
    return vecInts;
}
*/
    const std::vector<int>& GetVecInts()const
    {
    return vecInts;
    }
       void f(){}
    void f(int& i){}
    void f(const int& i){}
    void f(A& a){}
    void f( const A & a){}
    static int f(int i){ cout << "A::f called" << endl;}
};

void PrintVecInts(const std::vector<int>& vecInts)
{
//    printf("\n");
    for (size_t i = 0; i < vecInts.size(); i++)
        printf("%d current value = %d\n", i, vecInts[i]);
}
void TestVecInts()
{
    VectorRef vRef;
    std::vector<int> v = vRef.GetVecInts();
    v[0] = 100;
    PrintVecInts(v);
    PrintVecInts(vRef.GetVecInts());
}
void TestPointerToPointer()
{
    const char *name[] = {"jacky", "marry", "Tom", NULL};
    const char **ppstr = name;
    for (int i = 0 ; i < 3; i++)
        printf("%s \n", ppstr[i]);
    while (*ppstr != NULL)
        printf("%s \n", *ppstr++);
}

void CreateIntPtr(int **out)
{
    int*ret = new int(100);
    *out = ret;
}

void FreeIntPtr(int **in)
{
    if (*in)delete[] *in;
    *in = NULL;
}

int main()
{
/*
    int a  = 1;
    const int& b = a ;
    std::cout << b;
*/
    TestVecInts();
    TestPointerToPointer();

    int *out = NULL;
    CreateIntPtr(&out);
    printf("%d\n", *out);
    FreeIntPtr(&out);
    printf("%x", out);

    const double pi = 3.14;
    printf("%x \n", &pi);
    double pi3 = pi *3.0;
    const double * cptr = &pi;
    printf("the cptr value = %f\n", *cptr);
    *(const_cast<double*>(cptr)) = 5.0;
    printf("the cptr value = %f\n", *cptr);
    printf("%x \n", &*cptr);
    void *pvoid = NULL;
    int *pint = NULL;
    pvoid = pint;
    pint = (int*)pvoid;
    const VectorRef vr;
    vr.GetVecInts();
    VectorRef vr2;
    vr2.GetVecInts();

/*
    double d = 7.8;
   // int n = d;
    int n = static_cast<int>(d);
    double * pd = &d;
    //int *pi2 = reinterpret_cast<int *>( pd);
    int * pint2 ;
    short *pshort ;
    static_cast<int *>(pshort);
    printf ("%d\n", *pi2);
*/
     int *pnull = NULL;
//    std::cout << *pnull << std::endl;
/*(
    int (*sort) (int)  = NULL;
    int f1(int );
    sort = &f1; 
    sort = &VectorRef::f;
    sort(3);
    sort = &func1;
   (* sort) (3);
*/
// TestMemberFunc();   
//
    TestCallback();    
}

int f1(int a){ return 3;}
