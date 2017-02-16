#include <iostream>
#include <functional>
#include <algorithm>
using namespace std;
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>
#include <vector>
using namespace boost;

int main()
{
  /*
  string str1("AbCdEfG");
  vector<char> vec1(str1.begin(), str1.end());
  //to_lower_copy(ostream_iterator<char>(cout), vec1);
  string str2;
  to_lower_copy(ostream_iterator<char>(cout), str1);
  to_lower(str1);
  cout << str1; 
  cout << endl;
  to_lower_copy(back_inserter(str2), str1);
  cout << str2 << endl;
  */
  /*

  string str1("abc___cde___efg");
  string str2("abc");

  iterator_range<string::iterator> rang = 
    find_first(str1,("cde"));
  to_upper(rang);
  cout << str1;

  typedef iterator_range<string::iterator> RANG;
  RANG r2 = find_head(str1, 3);
  //to_upper(r2);
  cout << string(r2.begin(), r2.end());
  cout << str1;
  cout << endl;

  r2  = find_tail(str1, 5);
  cout << string(r2.begin(), r2.end());

  char text[] = "hello dolly!";
  iterator_range<char*> r3 = find_last(text, "ll");
  transform(r3.begin(), r3.end(), r3.begin(), bind2nd(plus<char>(), 1));
  to_upper(r3);
  cout << text << endl;
  cout << starts_with(text, "h") << endl;
  cout << ends_with(text, "!") << endl;
  cout << contains(text, "MM" ) << endl;
  cout << equals(text, "dolly!") << endl;
  if ( all(";.,", is_punct())) {
    cout << "all are punctuation char\n";
  }
  if ( all("abcxxx", is_any_of("xabc") && 
          !is_space()))
    cout << "OK" << endl;
    */

  string str1("abc___def___ghi");
 // replace_nth(str1, "_", 4, "+");
  cout << str1 << endl;
  cout << erase_range_copy(str1, make_iterator_range(str1.begin()+6, str1.begin()+9)) << endl; 
  cout << endl;
  cout << replace_range_copy(str1, make_iterator_range(str1.begin()+6 , str1.begin() + 9), "" ) << endl;
  replace_first_copy(ostream_iterator<char>(cout), str1, "abc", "XYZ");
  cout << endl;
  cout << replace_all_copy(str1, "___", "---") << endl;
}
