#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
using namespace boost::interprocess;
#include <string.h>

int main(int argc, char*argv[])
{
  try{
    shared_memory_object sharedmem1(create_only, "hello1", read_write);
    sharedmem1.truncate(256);
    mapped_region mmap(sharedmem1, 256);
    std::strcpy(static_cast<char*>(mmap.get_address()), "Hello World!\n");

  }catch(interprocess_exception&e)
  {
    
  }
}
