#include <boost/lexical_cast.hpp>
#include <vector>
#include <iostream>
#include <string>
using namespace std;

int convert_strings_part(const std::string &s, std::size_t pos, std::size_t n){
   return boost::lexical_cast<int>(s.data() + pos, n);
}

void number_to_file(int number, FILE* file){
  typedef boost::array<char, 50> buf_t;
  buf_t buffer = boost::lexical_cast<buf_t>(number);
  fputs(buffer.begin(), file);
}

int main(int argc, char *argv[])
{
  using boost::lexical_cast;
  using boost::bad_lexical_cast;

  std::vector<short> args;

  while ( *++argv)
  {
    try {
      args.push_back(lexical_cast<short>(*argv));
    }
    catch (const bad_lexical_cast&)
    {
      args.push_back(0);
    }
  }

  cout << convert_strings_part("1234b6", 2, 2) << endl;
  FILE* f = fopen("lexical_result.txt", "w");
  number_to_file(5555, f);
  fclose(f);
}
