#include "dll_util.hpp"
#if defined(WIN32)
  #include <windows.h>
#elif defined(_hpux)
  #include <dl.h>
#else
  #include <dlfcn.h>
#endif

namespace archy {

int dll_util::dll_open(const string& dllfile)
{
#if defined(WIN32)
	m_hDllInstance = ::LoadLibrary(dllfile.c_str());
#elif defined(_hpux)
	m_hDllInstance = ::shl_load(dllfile.c_str(), BIND_DEFERRED | DYNAMIC_PATH, 0L);
#else
	m_hDllInstance = ::dlopen(dllfile.c_str(), RTLD_LAZY);
#endif
	if( m_hDllInstance == 0 ) return -1;
	return 0;
}

void* dll_util::dll_sym(const string& symname)
{
	if( m_hDllInstance != 0 ) {
#if defined(WIN32)
		return ::GetProcAddress(m_hDllInstance, symname.c_str());
#elif defined(_hpux)
		void* pAddr;
		int rtn = shl_findsym((mapped_shl_entry **)&m_hDllInstance, symname.c_str(), TYPE_PROCEDURE, pAddr);
		if( rtn == 0 )
			return pAddr;		
#else
		return ::dlsym(m_hDllInstance, symname.c_str());
#endif
	}
	return 0;
}

void dll_util::dll_close()
{
	if( m_hDllInstance ) {
#if defined(WIN32)
		::FreeLibrary(m_hDllInstance);
#elif defined(_hpux)
		::shl_unload(m_hDllInstance);
#else
		::dlclose(m_hDllInstance);
#endif
		m_hDllInstance = 0;
	}
}

dll_util::dll_util()
{
	m_hDllInstance = 0;
}

dll_util::~dll_util()
{
	if( m_hDllInstance ) {
		dll_util::dll_close();
	}
}


} // namespace archy


