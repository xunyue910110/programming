#ifndef ARCHY_ITERATOR_HPP_HEADER
#define ARCHY_ITERATOR_HPP_HEADER

#include <stddef.h>           // std::ptrdiff_t

namespace archy
{
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

  template <class Category, class T,
    class Distance = ptrdiff_t,
    class Pointer = T*, class Reference = T&>
  struct iterator
  {
    typedef T         value_type;
    typedef Distance  difference_type;
    typedef Pointer   pointer;
    typedef Reference reference;
    typedef Category  iterator_category;
  };
  
} // namespace archy

#endif //ARCHY_ITERATOR_HPP_HEADER
