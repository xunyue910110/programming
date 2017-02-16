#include <boost/algorithm/string.hpp>
using namespace std;
#include <string>
#include <iostream>
using namespace boost;

bool is_executable(string & filename){
   return iends_with(filename, ".exe")||
          iends_with(filename, ".com");
}

int main(){
{
string str1(  " hello world ");
to_upper(str1);
trim(str1);
cout << str1 << endl;

string str2 ( " hello world ");
str2 =
  to_lower_copy(
      ireplace_first_copy(
        str2, "hello", "goodbye"));
cout << str2 << endl;

string str3 ( " hello world hello ");
//ireplace_first(str3, "hello", "goodbye");
ireplace_last(str3, "HELLO", "goodbye");
cout << str3 << endl;

string str4 ( " hello world hello ");
str4 = trim_copy(str4);
cout << str4 << endl;
string str5 ( " hello world hello ");
cout << trim_copy(to_upper_copy(str5)) << endl;
}

{
string str1("command.com");
cout <<
  str1
  << (is_executable(str1) ? "is" : "is not")
  << " an executable"
  << endl;

  char text1[] = "hello1";
  cout << text1
       << (all(text1, is_lower() )? " is": " is not ")
       << " written in the lower case"
       << endl;
}


string str1 = "   hello world!      ";
string str2 = trim_left_copy(str1);
string str3 = trim_right_copy(str1);
trim(str1);

string phone="004322222444";
trim_left_if(phone, is_any_of("0"));
cout << phone << endl;

{
  char text[]  = "hello dolly!";
  iterator_range<char*> result = find_last(text, "ll");
  transform(result.begin(), result.end(), result.begin(), bind2nd(plus<char>(), 1));
  cout << text<< endl;
  to_upper(result);

  cout << text<< endl;

  if ( find_last(text, "dolly"))
  {
    cout << "Dolly is there"  << endl;
  }
  cout << text<< endl;
}
{
  string str1="Hello  Dolly,   Hello World!";
  replace_first(str1, "Dolly", "Jane");
  replace_last(str1, "Hello", "Goodbye");
  erase_all(str1, " ");
  cout << str1 << endl;
  erase_head(str1, 6);
  cout << str1 << endl;
}


{
  string str1("abc-*-ABC-*-aBc");
  typedef find_iterator<string::iterator> string_find_iterator;
  for ( string_find_iterator It =
        make_find_iterator(str1, first_finder("abc", is_iequal()));
        It != string_find_iterator();
        ++It)
  {
    cout << copy_range<std::string>(*It) << endl;
  }

  typedef split_iterator<string::iterator> string_split_iterator;
  for ( string_split_iterator It = make_split_iterator(str1, first_finder("-*-", is_iequal()));
      It!= string_split_iterator();
      ++It)
  {
    cout << copy_range<std::string>(*It) << endl;
  }
}


}
