#ifndef DLL_UTIL_HEADER_FILE_ARCHY
#define DLL_UTIL_HEADER_FILE_ARCHY
#include "archy_config.hpp"

USING_STD_NAMESPACE;
namespace archy {

class dll_util
{
public:
	dll_util();
	~dll_util();
	int dll_open(const string& dllfile);
	void* dll_sym(const string& symname);
	void dll_close();
	bool is_valid()const { 
		return m_hDllInstance != 0;
	}
private:
#ifdef WIN32
	HINSTANCE m_hDllInstance;
#else
	void* m_hDllInstance;
#endif	
};

} // namespace archy

#if defined(WIN32)
  // In C++ Builder "DllMain" should be replace with "DllEntryPoint"
  // #pragma argsused
  // int WINAPI DllEntryPoint(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved ){ return 1;}
  
  #define ARCHY_DLL_MAIN int WINAPI DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved ){ return 1;}
#else
  #define ARCHY_DLL_MAIN
#endif

#endif // DLL_UTIL_HEADER_FILE_ARCHY
