#include <utility>
using namespace std;
class noncopyable
{
protected:

	noncopyable() { }
	~noncopyable() { }	// May not be virtual! Classes without virtual
				 // operations also derive from noncopyable.

  noncopyable(const noncopyable&&) {}
//private:

	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&){}

};

class child: public noncopyable {

};

int main() {
  child a;
  child b = std::move (a);
  child c;
  a = b = c;
}
