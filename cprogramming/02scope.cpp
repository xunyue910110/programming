namespace A
{
  struct X {};
  void g( X ) {}
}

  void g( A::X ) {}
int main() {
  A::X x;
  g( x);
}
