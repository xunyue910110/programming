class Dog {
  public:
    virtual Dog * clone() { return (new Dog(*this));}
};

class Yellowdog : public Dog {
 public:
   virtual Yellowdog * clone( ) { return (new Yellowdog(*this));}
};

void foo ( Dog* d ) {
  //Dog * c = new Dog(*d);
  Dog * c = d->clone();
}

int main() {
  Yellowdog d ;
  foo ( &d);
}
