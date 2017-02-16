#include <stdio.h>

class TestClass1{
public:
    TestClass1(): s(1), f(.1f){}
   const int s ;
    const float f ; 
    TestClass1(const TestClass1& hr):s(hr.s), f(hr.f) {}
    void func(TestClass1 t1){printf("t1\n");}
    void func2 ();
    
};

//int TestClass1::s = 1;
//int TestClass1::s = 1;
//const float TestClass1::f = 1.0f;
//
class TestClass2
{
public:
    TestClass2()
    {
        printf("TestClass2 default ctor\n");
        a = 1;
    }
    TestClass2(const TestClass2&right)
    {
        printf("TestClass2 copy ctor\n");
        a = right.a;
    }

    TestClass2& operator = (const TestClass2& right)
    {
        printf("TestClass assign operator\n");
        a = right.a;
        return *this;
    }
private:
    int a;
};

class TestClass3
{
    private:
    TestClass2 tc2;
    public:
    TestClass3(const TestClass2& c2):
        tc2(c2)
    {
    }
    TestClass3(){
        printf("TestClass3 default ctor\n");
        
    }
};
int main()
{
   // int s = TestClass1::s;
    //printf("%d\n", s);
    /*
    TestClass1 tc;
    float f = tc.f;
    tc.func(tc);
    printf("%f\n", tc.f);
    tc.func2();
*/
    //TestClass3 tc3;
    TestClass2 tc2;
    printf("\n");
    TestClass3 tc3(tc2);
     
    return 0;
}
