

#include "regexp.h"
#include <iostream>


int test(void)
{
    // Simple way to use regular expressions
    Regexp pattern("D\\w*g");
    cout << pattern.GetMatch("The Dodgers play baseball.") << endl;
    cout << "" << endl;
    // Perl compatible regular expression pattern to match
    string pat("(q.*k).*f?x");
    pattern.Set(pat);
    
    // String to find matching pattern in
    string text("The quick brown fox jumped over the lazy dogs.\n");
    text += "Now is the time for all good men to come to the aid of their ";
    text += "country.\nTwas the night before Christmas and all through the ";
    text += "house, not a\n creature was stirring, not even a mouse.\n";
    
    // Display pattern and sub pattern matches
    cout << pattern.GetMatch(text.c_str()) << endl;
    for (int k = 1; k < pattern.NumFound(); k++) {
        cout << pattern.GetSub(text.c_str(), 1) << endl;
    }    
    
    // Set new pattern and ignore case
    pattern.Set("t\\w*e", Regexp::eCompile_ignore_case);
    
    // Find all matches to pattern
    size_t start = 0;
    while (start != string::npos) {
        string match = pattern.GetMatch(text.c_str(), start);
        if (pattern.NumFound() > 0) {
            cout << match << endl;
            start = text.find(match, start) + 1;            
        } else {
            break;
        }
    }
    
    // test for RegexpUtil
    if (RegexpUtil::Exists("my this","^m\\w*"))
      cout << "exits" << endl;
    else
      cout << "not exists" << endl;
   int i;
    
    // Same as above but with cstrings and elinates string return
    start = 0;
    char *txt = new char[text.length() + 1];
    strcpy(txt, text.c_str());
    while (true)
    {
        pattern.GetMatch(txt, start, 0, 0, true);
        if (pattern.NumFound() > 0) {
            const int *rslt = pattern.GetResults(0);
            start = rslt[1];
            for (int i = rslt[0]; i < rslt[1]; i++) {
                cout << txt[i];
            }
            cout << endl;
        } else {
            break;
        }
    }
    delete[] txt;                  
    return 0;
}



/////////////////////////////////////////////////////////////////////////////
//  MAIN


int main(int argc, const char* argv[])
{
	cout << "startup proc" << endl;
    // Execute main application function
    test();
    return 0;
}

