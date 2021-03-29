#include "zip.h"

/*
template <class A, class B> class zip_t {
  A &a;
  B &b;

  class iterator {
    typename A::iterator apos;
    typename B::iterator bpos;

  public:
    using value_type =
        std::pair<typename A::value_type, typename B::value_type>;

    iterator(typename A::iterator a, typename B::iterator b)
        : apos(a), bpos(b) {}

    iterator operator++() {
      ++apos;
      ++bpos;
      return *this;
    }

    bool operator==(iterator const &other) const { return apos == other.apos; }

    bool operator!=(iterator const &other) const { return apos != other.apos; }

    value_type operator*() { return value_type{*apos, *bpos}; }
  };

public:
  zip_t(A &a, B &b) : a(a), b(b) { assert(a.size() == b.size()); }

  iterator begin() const { return iterator(a.begin(), b.begin()); }
  iterator end() const { return iterator(a.end(), b.end()); }
};

template <class A, class B> zip_t<A, B> zip(A &a, B &b) {
  return zip_t<A, B>(a, b);
}
*/

//////////
// OR : //
//////////

// template <typename T> class Zip {

// public:
//   typedef std::vector<T> container_t;

//   template <typename... Args>
//   Zip(const T &head, const Args &...args)
//       : items_(head.size()), min_(head.size()) {
//     zip_(head, args...);
//   }

//   inline operator container_t() const { return items_; }

//   inline container_t operator()() const { return items_; }

// private:
//   template <typename... Args> void zip_(const T &head, const Args &...tail) {
//     // If the current item's size is less than
//     // the one stored in min_, reset the min_
//     // variable to the item's size
//     if (head.size() < min_)
//       min_ = head.size();

//     for (std::size_t i = 0; i < min_; ++i) {
//       // Use begin iterator and advance it instead
//       // of using the subscript operator adds support
//       // for lists. std::advance has constant complexity
//       // for STL containers whose iterators are
//       // RandomAccessIterators (e.g. vector or deque)
//       typename T::const_iterator itr = head.begin();

//       std::advance(itr, i);

//       items_[i].push_back(*itr);
//     }

//     // Recursive call to zip_(T, Args...)
//     // while the expansion of tail... is not empty
//     // else calls the overload with no parameters
//     return zip_(tail...);
//   }

//   inline void zip_() {
//     // If min_ has shrunk since the
//     // constructor call
//     items_.resize(min_);
//   }

//   /*! Holds the items for iterating. */
// container_t items_;

// /*! The minimum number of values held by all items */
// std::size_t min_;
// }
// ;

// template <typename T, typename... Args>
// typename Zip<T>::container_t zip(const T &head, const Args &...tail) {
//   return Zip<T>(head, tail...);
// }