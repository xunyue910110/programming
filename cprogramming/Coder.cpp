#include <iostream>
using namespace std;

class Coder{
public:
  virtual void encode() = 0;
  virtual void decode() = 0;
  virtual ~Coder(){}
};

class rm : public Coder{
public:
  void encode(){cout << "rm encodeing " << endl;}
  void decode(){cout << "rm decodeing " << endl;}
};

class divx : public Coder{
public:
  void encode(){cout << "divx encodeing " << endl;}
  void decode(){cout << "divx decodeing " << endl;}
  
};

class File {
private:
  Coder* c;
public:
  //void setCoder(Coder* c){ this->c = c;}
  void setCoder(Coder& c){ this->c = &c;}
  void zip(){ c->encode();}
  void unzip(){ c->decode();}
};


int main()
{
  //Coder * prm = new rm;
  //Coder * pdivx = new divx;
  rm zrm;
  divx zdivx; 
  File f;
  f.setCoder(zrm);
  f.zip();
  f.unzip();

  f.setCoder(zdivx);
  f.zip();
  f.unzip();

  //delete prm;
  //delete pdivx;
}
