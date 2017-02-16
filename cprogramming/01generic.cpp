#include <iostream>
using namespace std;
class Generic_Parser{
  public:
    void parse_preorder (){
      process_node();
      //parse_perorder();
      //parse_perorder();
    }
  private:
   virtual  void process_node(){}
};

class EmployeeChart_Parser : public Generic_Parser {
  private:
    void process_node() {
      cout << "Customized "  << endl;
    }
};

int main() {
  EmployeeChart_Parser ep;
  ep.parse_preorder();
}
