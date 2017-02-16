#include <boost/interprocess/shared_memory_object.hpp>
using namespace boost::interprocess;

int main(int argc, char*argv[])
{
  try{
    shared_memory_object sharedmem1(create_only, "hello", read_write);
    sharedmem1.truncate(256);

  }catch(interprocess_exception&e)
  {
    
  }
}
