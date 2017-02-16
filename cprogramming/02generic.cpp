#include <iostream>
using namespace std;
template <typename T>
class Generic_Parser{
  public:
    void parse_preorder (){
      process_node();
      //parse_perorder();
      //parse_perorder();
    }
  public:
   void process_node(){
//     static_cast<T*>(this)  ->process_node();
     dynamic_cast<T*>(this)  ->process_node();
   }
   virtual ~Generic_Parser(){}
};

class EmployeeChart_Parser : public Generic_Parser <EmployeeChart_Parser> {
  public:
    void process_node() {
      cout << "Customized "  << endl;
    }
};

int main() {
  EmployeeChart_Parser ep;
  ep.parse_preorder();
}
