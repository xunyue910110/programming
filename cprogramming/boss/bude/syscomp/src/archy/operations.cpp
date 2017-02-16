#include "archy_config.hpp"
#include "operations.hpp"
#include "fileexception.hpp"
#include "scoped_array.hpp"
#include "throw_exception.hpp"


namespace fs = archy::filesystem;

// ARCHY_POSIX or ARCHY_WINDOWS specify which API to use.
# if !defined( ARCHY_WINDOWS ) && !defined( ARCHY_POSIX )
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#     define ARCHY_WINDOWS
#   else
#     define ARCHY_POSIX
#     if defined(__sun__) || defined(OS_SUN)
#        ifndef _POSIX_PTHREAD_SEMANTICS
#           define _POSIX_PTHREAD_SEMANTICS
#        endif
#     endif
#   endif
# endif

# if defined(ARCHY_WINDOWS)
#   include "windows.h"

// For Windows, the xxxA form of various function names is used to avoid
// inadvertently getting wide forms of the functions. (The undecorated
// forms are actually macros, so can misfire if the user has various
// other macros defined. There was a bug report of this happening.)

# else
#   include "sys/stat.h"
#   include "dirent.h"
#   include "unistd.h"
#   include "fcntl.h"
extern "C" {
	int readdir_r(DIR *dir_pointer, struct dirent *entry, struct dirent **result);
}

# endif

#include <string>
#include <cstdio>
#include <cerrno>
#include <cassert>

//  helpers  -----------------------------------------------------------------//

namespace
{
#ifdef ARCHY_POSIX

  #define ARCHY_HANDLE DIR *
  #define ARCHY_INVALID_HANDLE_VALUE 0
  #define ARCHY_SYSTEM_DIRECTORY_TYPE struct dirent *

  const char *  find_first_file( const char * dir,
    ARCHY_HANDLE & handle, ARCHY_SYSTEM_DIRECTORY_TYPE & dp )
  // Returns: 0 if error, otherwise name
  {
    const char * dummy_first_name = ".";
    bool ret =  ((handle = ::opendir( dir ))== ARCHY_INVALID_HANDLE_VALUE );
    if( !ret ) {
	#if defined(__sun__) || defined(OS_SUN)
		char* Entry = new char[ sizeof(struct dirent) + pathconf(dir, _PC_NAME_MAX) + 1 ];
		dp = (struct dirent *)(Entry);
	#else
		dp = new struct dirent;
	#endif
 //printf("opendir succesfull\n");
    }
    else {
    	dp = 0;
    }
    return ret ? 0 : dummy_first_name;
  }  

  void find_close( ARCHY_HANDLE handle )
  {
    assert( handle != ARCHY_INVALID_HANDLE_VALUE );
    ::closedir( handle );
  }

  const char * find_next_file(
    ARCHY_HANDLE handle, const fs::path & ph, ARCHY_SYSTEM_DIRECTORY_TYPE& dp )
  // Returns: if EOF 0, otherwise name
  // Throws: if system reports error
  {

//  TODO: use readdir_r() if available, so code is multi-thread safe.
//  Fly-in-ointment: not all platforms support readdir_r().

	struct dirent  *dirresult;
	int iRet;
	// 遍历搜索
	readdir_r(handle, dp, &dirresult);
	if( dirresult == 0 ) {
	#if defined(__sun__) || defined(OS_SUN)
		char* Entry = (char*)(dp);
		delete []Entry;
	#else
		delete dp;
	#endif
		dp = 0;
		return 0;
	}
	return dp->d_name;

  }
#else // ARCHY_WINDOWS

# define ARCHY_HANDLE HANDLE
# define ARCHY_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
# define ARCHY_SYSTEM_DIRECTORY_TYPE WIN32_FIND_DATAA

  const char *  find_first_file( const char * dir,
    ARCHY_HANDLE & handle, ARCHY_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if error, otherwise name
  {
//    std::cout << "find_first_file " << dir << std::endl;
    std::string dirpath( std::string(dir) + "/*" );
    return ( (handle = ::FindFirstFileA( dirpath.c_str(), &data ))
      == ARCHY_INVALID_HANDLE_VALUE ) ? 0 : data.cFileName;
  }  

  void find_close( ARCHY_HANDLE handle )
  {
//    std::cout << "find_close" << std::endl;
    assert( handle != ARCHY_INVALID_HANDLE_VALUE );
    ::FindClose( handle );
  }

  const char * find_next_file(
    ARCHY_HANDLE handle, const fs::path & ph,
    ARCHY_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if EOF, otherwise name
  // Throws: if system reports error
  {
    if ( ::FindNextFileA( handle, &data ) == 0 )
    {
      if ( ::GetLastError() != ERROR_NO_MORE_FILES )
      {
        archy::throw_exception( fs::filesystem_error(
          "archy::filesystem::directory_iterator::operator++",
          ph.branch_path(), fs::detail::system_error_code() ) );
      }
      else { return 0; } // end reached
     }
    return data.cFileName;
  }

#endif

  fs::directory_iterator end_itr;

  bool is_empty_directory( const fs::path & dir_path )
  {
    return fs::directory_iterator(dir_path) == end_itr;
  }

  unsigned long remove_all_aux( const fs::path & ph )
  {
    unsigned long count = 1;
    if ( fs::is_directory( ph ) )
    {
      for ( fs::directory_iterator itr( ph );
            itr != end_itr; ++itr )
      {
        count += remove_all_aux( *itr );
      }
    }
    fs::remove( ph );
    return count;
  }

} // unnamed namespace

namespace archy
{
  namespace filesystem
  {

//  dir_itr_imp  -------------------------------------------------------------// 

    class directory_iterator::dir_itr_imp
    {
    public:
      path              entry_path;
      ARCHY_HANDLE      handle;
      ARCHY_SYSTEM_DIRECTORY_TYPE scratch;
      ~dir_itr_imp()
      {
        if ( handle != ARCHY_INVALID_HANDLE_VALUE ) find_close( handle );
      }
    };

//  directory_iterator implementation  ---------------------------------------//

    // default ctor creates the "end" iterator (by letting base default to 0)
    directory_iterator::directory_iterator():m_imp(0) {}
	directory_iterator::~directory_iterator(){delete m_imp;	}

    directory_iterator::directory_iterator( const path & dir_path )
    {
      m_imp = new dir_itr_imp ;
      const char * name = 0;  // initialization quiets compiler warnings
      if ( dir_path.empty() )
       m_imp->handle = ARCHY_INVALID_HANDLE_VALUE;
      else
        name = find_first_file( dir_path.native_directory_string().c_str(),
          m_imp->handle, m_imp->scratch );  // sets handle

      if ( m_imp->handle != ARCHY_INVALID_HANDLE_VALUE )
      {
        m_imp->entry_path = dir_path;
        m_imp->entry_path.m_path_append( name, path::nocheck );
        while ( m_imp != 0
             && ( m_imp->entry_path.leaf() == "."
              || m_imp->entry_path.leaf() == ".." ) )
          { operator++(); }
      }
      else
      {
        archy::throw_exception( filesystem_error(  
          "archy::filesystem::directory_iterator constructor",
          dir_path, fs::detail::system_error_code() ) );
      }  
    }

    path & directory_iterator::m_deref() const
    {
      assert( m_imp != 0 ); // fails if dereference end iterator
      return m_imp->entry_path;
    }

    void directory_iterator::m_inc()
    {
      assert( m_imp != 0 ); // fails on increment end iterator
      assert( m_imp->handle != ARCHY_INVALID_HANDLE_VALUE ); // reality check

      const char * name;
      if ( (name = find_next_file( m_imp->handle,
        m_imp->entry_path, m_imp->scratch )) != 0 )
      {
        m_imp->entry_path.m_replace_leaf( name );
      }
      else
      {
      	delete m_imp;
      	m_imp = 0;
      }
    }

//  free functions  ----------------------------------------------------------//

    bool exists( const path & ph )
    {
#   ifdef ARCHY_POSIX
      struct stat path_stat;
      return ::stat( ph.string().c_str(), &path_stat ) == 0;  
#   else
      return ::GetFileAttributesA( ph.string().c_str() ) != 0xFFFFFFFF;
#   endif
    }

    bool is_directory( const path & ph )
    {
#   ifdef ARCHY_POSIX
      struct stat path_stat;
      if ( ::stat( ph.native_directory_string().c_str(), &path_stat ) != 0 )
        archy::throw_exception( filesystem_error(
          "archy::filesystem::is_directory",
          ph, fs::detail::system_error_code() ) );
      return S_ISDIR( path_stat.st_mode );
#   else
      DWORD attributes = ::GetFileAttributesA( ph.native_directory_string().c_str() );
      if ( attributes == 0xFFFFFFFF )
        archy::throw_exception( filesystem_error(
          "archy::filesystem::is_directory",
          ph, fs::detail::system_error_code() ) );
      return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#   endif
    }

    bool _is_empty( const path & ph )
    {
#   ifdef ARCHY_POSIX
      struct stat path_stat;
      if ( ::stat( ph.string().c_str(), &path_stat ) != 0 )
        archy::throw_exception( filesystem_error(
          "archy::filesystem::is_empty",
          ph, fs::detail::system_error_code() ) );
      
      return S_ISDIR( path_stat.st_mode )
        ? is_empty_directory( ph )
        : path_stat.st_size == 0;
#   else
      WIN32_FILE_ATTRIBUTE_DATA fad;
      if ( !::GetFileAttributesExA( ph.string().c_str(),
        ::GetFileExInfoStandard, &fad ) )
        archy::throw_exception( filesystem_error(
          "archy::filesystem::is_empty",
          ph, fs::detail::system_error_code() ) );
      
      return ( fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        ? is_empty_directory( ph )
        :( !fad.nFileSizeHigh && !fad.nFileSizeLow );
#   endif
    }

//  create_directories (contributed by Vladimir Prus)  -----------------------//

     void create_directories(const path& ph)
     {
         if (ph.empty() || exists(ph)) return;

         // First create branch, by calling ourself recursively
         create_directories(ph.branch_path());
         // Now that parent's path exists, create the directory
         create_directory(ph);
     }

    void create_directory( const path & dir_path )
    {
#   ifdef ARCHY_POSIX
      if ( ::mkdir( dir_path.native_directory_string().c_str(),
        S_IRWXU|S_IRWXG|S_IRWXO ) != 0 )
#   else
      if ( !::CreateDirectoryA( dir_path.native_directory_string().c_str(), 0 ) )
#   endif
        archy::throw_exception( filesystem_error(
          "archy::filesystem::create_directory",
          dir_path, fs::detail::system_error_code() ) );
    }

    bool remove( const path & ph )
    {
      if ( exists( ph ) )
      {
#   ifdef ARCHY_POSIX
        if ( ::remove( ph.string().c_str() ) != 0 )
        {
#   else
        if ( is_directory( ph ) )
        {
          if ( !::RemoveDirectoryA( ph.string().c_str() ) )
            archy::throw_exception( filesystem_error(
              "archy::filesystem::remove",
              ph, fs::detail::system_error_code() ) );
        }
        else
        {
          if ( !::DeleteFileA( ph.string().c_str() ) )
#   endif
            archy::throw_exception( filesystem_error(
              "archy::filesystem::remove",
              ph, fs::detail::system_error_code() ) );
        }
        return true;
      }
      return false;
    }

    unsigned long remove_all( const path & ph )
    {
      return exists( ph ) ? remove_all_aux( ph ) : 0;
    }

	void move_file(const path & from_file_ph,
                const path & to_file_ph )
	{
#   ifdef ARCHY_POSIX
		link(from_file_ph.string().c_str(), to_file_ph.string().c_str());
		unlink(from_file_ph.string().c_str());
#   else
		::MoveFileA(from_file_ph.string().c_str(), to_file_ph.string().c_str());
#   endif
	}
    void rename( const path & old_path,
                 const path & new_path )
    {
#   ifdef ARCHY_POSIX
      if ( exists( new_path ) // POSIX is too permissive so must check
        || ::rename( old_path.string().c_str(), new_path.string().c_str() ) != 0 )
#   else
      if ( !::MoveFileA( old_path.string().c_str(),  new_path.string().c_str()) )
#   endif
        archy::throw_exception( filesystem_error(
          "archy::filesystem::rename",
          old_path, new_path, fs::detail::system_error_code() ) );
    }

    void copy_file( const path & from_file_ph,
                    const path & to_file_ph )
    {
#   ifdef ARCHY_POSIX
      // TODO: Ask POSIX experts if this is the best way to copy a file

      const size_t buf_sz = 32768;
      archy::scoped_array<char> buf( new char [buf_sz] );
      int infile, outfile=0;  // init quiets compiler warning

      if ( (infile = ::open( from_file_ph.string().c_str(),
                             O_RDONLY )) < 0
        || (outfile = ::open( to_file_ph.string().c_str(),
                              O_WRONLY | O_CREAT | O_EXCL,
                              S_IRWXU|S_IRWXG|S_IRWXO )) < 0 )
      {
        if ( infile != 0 ) ::close( infile );
        archy::throw_exception( filesystem_error(
          "archy::filesystem::copy_file",
          from_file_ph, to_file_ph, fs::detail::system_error_code() ) );
      }

      ssize_t sz;
      while ( (sz = ::read( infile, buf.get(), buf_sz )) > 0
        && (sz = ::write( outfile, buf.get(), sz )) > 0 ) {}

      ::close( infile );
      ::close( outfile );

      if ( sz != 0 )
#   else
      if ( !::CopyFileA( from_file_ph.string().c_str(),
                      to_file_ph.string().c_str(), /*fail_if_exists=*/true ) )
#   endif
        archy::throw_exception( filesystem_error(
          "archy::filesystem::copy_file",
          from_file_ph, to_file_ph, fs::detail::system_error_code() ) );
    }

    path current_path()
    {
#   ifdef ARCHY_POSIX
      long path_max = ::pathconf( ".", _PC_PATH_MAX );
      if ( path_max < 1 )
        archy::throw_exception(
          filesystem_error( "archy::filesystem::current_path",
            "_PC_PATH_MAX < 1" ) );
      archy::scoped_array<char>
        buf( new char[static_cast<size_t>(path_max)] );
      if ( ::getcwd( buf.get(), static_cast<size_t>(path_max) ) == 0 )
#   else
      DWORD sz;
      if ( (sz = ::GetCurrentDirectoryA( 0, static_cast<char*>(0) )) == 0 )
        archy::throw_exception(
          filesystem_error( "archy::filesystem::current_path",
            "size is 0" ) );
      archy::scoped_array<char> buf( new char[sz] );
      if ( ::GetCurrentDirectoryA( sz, buf.get() ) == 0 )
#   endif
        archy::throw_exception(
          filesystem_error( "archy::filesystem::current_path", path(),
            fs::detail::system_error_code() ) );
      return path( buf.get(), native );
    }

    const path & initial_path()
    {
      static path init_path;
      if ( init_path.empty() ) init_path = current_path();
      return init_path;
    }

    path system_complete( const path & ph )
    {
#   ifdef ARCHY_WINDOWS
      if ( ph.empty() ) return ph;
      char buf[MAX_PATH];
      char * pfn;
      size_t len = ::GetFullPathNameA( ph.string().c_str(),
                                            sizeof(buf) , buf, &pfn );
      if ( !len )
        { archy::throw_exception(
            filesystem_error( "archy::filesystem::system_complete",
              ph, "size is 0" ) ); }
      buf[len] = '\0';
      return path( buf, native );
#   else
      return (ph.empty() || ph.is_complete())
        ? ph : current_path() / ph;
#   endif
    }
    
    path complete( const path & ph, const path & base )
    {
      assert( base.is_complete()
        && (ph.is_complete() || !ph.has_root_name()) ); // precondition
#   ifdef ARCHY_WINDOWS
      if (ph.empty() || ph.is_complete()) return ph;
      if ( !ph.has_root_name() )
        return ph.has_root_directory()
          ? path( base.root_name(), native ) / ph
          : base / ph;
      return base / ph;
#   else
      return (ph.empty() || ph.is_complete()) ? ph : base / ph;
#   endif
    }
  } // namespace filesystem
} // namespace archy

