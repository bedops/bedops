//=========
// File:    IteratorPair.hpp
// Author:  Shane Neph & Scott Kuehn
// Date:    Tue Aug 28 09:36:24 PDT 2007
// Project: Utility
// ID: $Id:$
//=========


// Macro Guard
#ifndef PAIRED_ITERATOR_WRAP_HPP
#define PAIRED_ITERATOR_WRAP_HPP

// Files
#include <cstddef>
#include <iterator>
#include <utility>


namespace Ext {

  namespace details {    
    template <class Iter>
    struct number_iter; // undefined, including input/output_iterator_tag

    template <>
    struct number_iter<std::forward_iterator_tag> {
      enum { value = 2 };
    };

    template <>
    struct number_iter<std::bidirectional_iterator_tag> {
      enum { value = 3 };
    };

    template <>
    struct number_iter<std::random_access_iterator_tag> {
      enum { value = 4 };
    };

    template <int I>
    struct get_category; // undefined

    template <>
    struct get_category< number_iter<std::forward_iterator_tag>::value > {
      typedef std::forward_iterator_tag iterator_category;
    };

    template <>
    struct get_category< number_iter<std::bidirectional_iterator_tag>::value > {
      typedef std::bidirectional_iterator_tag iterator_category;
    };

    template <>
    struct get_category< number_iter<std::random_access_iterator_tag>::value > {
      typedef std::random_access_iterator_tag iterator_category;
    };
   
    template <typename IterCat1, typename IterCat2>
    struct lesser_category {
      enum { value = (number_iter<IterCat1>::value <= number_iter<IterCat2>::value) ?
                                                      number_iter<IterCat1>::value :
                                                      number_iter<IterCat2>::value };
      typedef typename get_category<value>::iterator_category iterator_category;
    };

    template <typename IterCat1, typename IterCat2>
    struct greater_category {
      enum { value = (number_iter<IterCat1>::value >= number_iter<IterCat2>::value) ?
                                                      number_iter<IterCat1>::value :
                                                      number_iter<IterCat2>::value };
      typedef typename get_category<value>::iterator_category iterator_category;
    };

  } // details




  //=================
  // paired_iterator
  //=================
  template <typename IterType1, typename IterType2>
  struct paired_iterator {
  
    typedef typename details::lesser_category<typename IterType1::iterator_category,
                                              typename IterType2::iterator_category>::iterator_category iterator_category;
    typedef std::pair<IterType1, IterType2> value_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type*    pointer;
    typedef value_type&    reference;
  
  
  
    paired_iterator(IterType1 iter1, IterType2 iter2)
      : piter_(iter1, iter2)
      { /* */ }

    paired_iterator(const std::pair<IterType1, IterType2>& p)
      : piter_(p.first, p.second)
      { /* */ }
  
    paired_iterator()
      : piter_()
      { /* */ }
  
    reference operator*() { return piter_; }
    pointer operator->() { return &(operator*()); }
  
    paired_iterator& operator++() {
      ++piter_.first, ++piter_.second;
      return(*this);
    }
  
    paired_iterator operator++(int) {
      paired_iterator p = *this;
      ++piter_.first, ++piter_.second;
      return(p);
    }
  
    IterType1 iter1() const {
      return(piter_.first);
    }
  
    IterType2 iter2() const {
      return(piter_.second);
    }
  
    IterType1 iter1() {
      return(piter_.first);
    }
  
    IterType2 iter2() {
      return(piter_.second);
    }
  
    template <typename I1, typename I2>
    friend bool operator==(const paired_iterator<I1, I2>& a, const paired_iterator<I1, I2>& b) {
      return(a.piter_.first == b.piter_.first && a.piter_.second == b.piter_.second);
    }
  
    template <typename I1, typename I2>
    friend bool operator!=(const paired_iterator<I1, I2>& a, const paired_iterator<I1, I2>& b) {
      return(!(a == b));
    }
  
  private:
    std::pair<IterType1, IterType2> piter_;
  };

} // namespace Ext


#endif // PAIRED_ITERATOR_WRAP_HPP
