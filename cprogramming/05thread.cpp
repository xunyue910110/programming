#include <thread>
#include <map>
#include <iostream>
#include <future>
std::map<std::string, std::string> french
  {{"hello","bonjour"}, {"world", "tout le monde"}};
int main()
{
    std::string greet = french["hello"];
//    std::thread t( [](std::string & greet){std::cout << greet << ", ";greet="good";},std::ref( greet) );
    auto f = std::async([&]{return french["hello"];});

    std::string audience = french["orld"];
    try
    {
//      throw 1;
      ;
    }catch( ... )
    {
//      t.join();
      throw;
    }

    //t.join();
    std::cout << f.get() << std::endl;;
    //std::cout << audience << std::endl;
    //std::cout << greet << std::endl;
}
