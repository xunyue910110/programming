#include "iostream"
#include "string"
class dog {
  public:
    std::string m_name;
    dog () : m_name("Bob"){}

    void bark() const { // *this is constant
     std::cout << "My name is " << m_name << std::endl;
     //m_name = "Henry";
     const_cast<dog*>(this) ->m_name = "Henry";
    }
};
