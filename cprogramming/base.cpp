#include <stdio.h>

class Base
{
    public:
    int publica;
    protected:
    int protectedb;
    private:
    int privatec;
};

class PublicDerived : public Base
{
public:
    PublicDerived()
    {
        publica = 1;
        Base::protectedb = 2;
//        privatec = 3  ;
        //Base::privatec= 3;
    }
};

int main()
{
    PublicDerived pd ;
    printf("pd.publica=%d\n", pd.publica);
    //printf("pd.protectedb=%d\n", pd.protectedb);
}
