class Rational {
  public:
    Rational ( int numerator = 0, int innominator = 1):
      num(numerator), den(innominator) {}

//    const Rational operator*(const Rational& rhs) {
 //     return Rational(num* rhs.num, den* rhs.den) ;
  //  }

    int num;
    int den;
};

const Rational operator* ( const Rational& lhs, const Rational& rhs){
   return Rational(lhs.num * rhs.num  , lhs.den* rhs.den);
}

int main() {
  Rational r1 = 23;
  Rational r2 = r1 * 2;
  Rational r3 = 2 * r1;

}
