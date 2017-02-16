class C {
  public:
    struct Y {};
    static void h(Y) { }
};
    static void h(C::Y) { }

int main() {
   C::Y y;
   h(y);
}
