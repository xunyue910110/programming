#ifndef BOOST_FILESYSTEM_DIRECTORY_HPP
#define BOOST_FILESYSTEM_DIRECTORY_HPP

#include "archy_config.hpp"
#include "path.hpp"
#include "iterator.hpp"

#include <string>

//----------------------------------------------------------------------------//

namespace archy
{
  namespace filesystem
  {

//  query functions  ---------------------------------------------------------//

    bool exists( const path & ph );

    bool is_directory( const path & ph );

    // VC++ 7.0 and earlier has a serious namespace bug that causes a clash
    // between boost::filesystem::is_empty and the unrelated type trait
    // boost::is_empty. The workaround for those who must use broken versions
    // of VC++ is to use the function _is_empty. All others should use the
    // correct is_empty name.
    bool _is_empty( const path & ph ); // deprecated

#   if !defined( BOOST_MSVC ) || BOOST_MSVC > 1300
    inline bool is_empty( const path & ph ) { return _is_empty( ph ); }
#   endif

//  operations  --------------------------------------------------------------//

    void create_directory( const path & directory_ph );
//  create_directories (contributed by Vladimir Prus)  -----------------------//


    /** Creates directory 'ph' and all necessary parent directories.
        @post exists(directory_ph) && is_directory(directory_ph) && is_empty(directory_ph)
     */
    void create_directories(const path& ph);

    bool remove( const path & ph );
    unsigned long remove_all( const path & ph );

    void rename( const path & from_path,
                 const path & to_path );

    void copy_file( const path & from_file_ph,
                    const path & to_file_ph );
	
	void move_file(const path & from_file_ph,
                    const path & to_file_ph );

    path          current_path();
    const path &  initial_path();

    path          system_complete( const path & ph );
    path          complete( const path & ph, const path & base = initial_path() );

//  directory_iterator  ------------------------------------------------------//

    class directory_iterator
      : public archy::iterator< std::input_iterator_tag, path >
    {
    private:
      typedef directory_iterator self;
    public:
      directory_iterator();  // creates the "end" iterator
	  ~directory_iterator();
      explicit directory_iterator( const path & p );

      reference operator*() const { return m_deref(); }
      pointer   operator->() const { return &m_deref(); }
      self &    operator++() { m_inc(); return *this; }

      bool operator==( const self & y )
        { return m_imp == y.m_imp; }
      bool operator!=( const self & y )
        { return !(m_imp == y.m_imp); }

      struct path_proxy // allows *i++ to work, as required by std
      {
        path pv;
        explicit path_proxy( const path & p ) : pv(p) {}
        path operator*() const { return pv; }
      };

      path_proxy operator++(int)
      {
        path_proxy pp( m_deref() );
        ++*this;
        return pp;
      }

    private:
      class dir_itr_imp;
      dir_itr_imp*  m_imp;
      path  &   m_deref() const;
      void       m_inc();
    };

  } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_DIRECTORY_HPP
