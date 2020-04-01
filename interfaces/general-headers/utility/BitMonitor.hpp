/*
  Author: Shane Neph
  Date:   Tue Nov 22 13:53:28 PST 2016
*/
//
//    BEDOPS
//    Copyright (C) 2011-2020 Shane Neph, Scott Kuehn and Alex Reynolds
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef UTILS_BITMONITOR
#define UTILS_BITMONITOR

#include <algorithm>
#include <array>
#include <bitset>
#include <iterator>
#include <limits>
#include <ostream>
#include <type_traits>

#include "utility/CompilerMath.hpp"

namespace Ext {

  template <std::size_t Total>
  struct BSet {
    static constexpr std::size_t BITS = Total;
    static constexpr std::size_t BASE = std::numeric_limits<unsigned char>::digits;
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

    BSet() : _count(0), _open(0) { for (std::size_t i=0; i<TBYTE; ++i) _charset[i]=CZERO; }

    inline bool any() const { return _count < BITS; }
    inline bool empty() const { return _count == 0; }
    inline std::size_t size() const { return BITS; }
    inline std::size_t get_open() { return (npos != _open) ? _open : find_first_unset(); }

    inline bool set(std::size_t bit) {
      _charset[bit/BASE] |= (1 << (bit%BASE));
      bool has_open = (++_count < BITS);
      if ( bit == _open && has_open )
        _open = (bit+1 < BITS) ? next_unset(bit) : find_first_unset(bit); // could still become npos
      return has_open;
    }

    inline void set_all() {
      for ( auto& c : _charset )
        c = FULL;
      _count = BITS;
      _open = npos;
    }

    inline void unset(std::size_t bit) {
      _charset[bit/BASE] &= ~(1 << (bit%BASE));
      --_count;
      if ( npos == _open )
        _open = bit;
    }

    inline void unset_all() {
      for ( auto& c : _charset )
        c = CZERO;
      _count = 0;
      _open = 0;
    }

    inline std::size_t next_set(std::size_t start) const {
      start += 1;
      std::size_t bin = start/BASE;
      if ( _charset[bin] ) {
        for ( std::size_t i = start%BASE; i < BASE; ++i ) {
          if (_charset[bin] & (1 << i))
            return bin*BASE+i;
          else if ( npos == _open )
            _open = bin*BASE+i;
        } // for
      }

      for ( std::size_t b = bin+1; b < TBYTE; ++b ) {
        if ( _charset[b] ) {
          for ( std::size_t i = 0; i < BASE; ++i ) {
            if ( _charset[b] & (1 << i) )
              return b*BASE+i;
            else if ( npos == _open )
              _open = bin*BASE+i;
          } // for
        }
      } // for
      return npos;
    }

    inline std::size_t find_first_set() {
      if ( _charset[0] & (1 << 0) )
        return 0;
      else if ( npos == _open )
        _open = 0;
      return next_set(0);
    }

    inline std::size_t next_unset(std::size_t start) {
      start += 1;
      std::size_t bin = start/BASE;
      for ( std::size_t i = start%BASE; i < BASE; ++i ) {
        if ( !(_charset[bin] & (1 << i)) ) {
          if ( npos == _open )
            _open = bin*BASE+i;
          return bin*BASE+i;
        }
      } // for

      for ( std::size_t b = bin+1; b < TBYTE; ++b ) {
        if ( _charset[b] != FULL ) {
          for ( std::size_t i = 0; i < BASE; ++i ) {
            if ( !(_charset[b] & (1 << i)) ) {
              if ( npos == _open )
                _open = b*BASE+i;
              return b*BASE+i;
            }
          } // for
        }
      } // for
      return npos;
    }

    inline std::size_t find_first_unset() {
      if ( !(_charset[0] & (1 << 0) ) ) {
        if ( npos == _open )
          _open = 0;
        return 0;
      }
      return next_unset(0);
    }

    template <std::size_t A>
    friend
    std::ostream&
    operator<<(std::ostream& os, const BSet<A>& r);

  private:
    static constexpr std::size_t TBYTE = BITS/BASE;
    static constexpr unsigned char CZERO = (unsigned char)0;
    static constexpr unsigned char FULL = std::numeric_limits<unsigned char>::max();

    std::size_t _count, _open;
    std::array<unsigned char, TBYTE> _charset;
  };

  template <std::size_t A>
  std::ostream&
  operator<<(std::ostream& os, const BSet<A>& r) {
    std::copy(r._charset.begin(), r._charset.end(), std::ostream_iterator<std::bitset<8>>(os, " "));
    os << std::endl;
    return os;
  }

  template <std::size_t Base,
            std::size_t Total, // undefined when Check==false
            bool Check=(Base==std::numeric_limits<unsigned char>::digits) &&
                       (Total%Base==0) &&
                       (Pow<Base, IntLogN<Base, Total>::value>::value==Total)>
  struct BitMonitor2;

  template <std::size_t Base,
            std::size_t Total>
  struct BitMonitor2<Base, Total, true> {
    static constexpr std::size_t BASE = std::numeric_limits<unsigned char>::digits;
    static constexpr std::size_t TOTAL = Total + BitMonitor2<Base, Total/Base>::TOTAL;
    static constexpr std::size_t BITS = Total;
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

    BitMonitor2() : _nxt(0) { for( std::size_t i=0; i<TBYTE; ++i) _charset[i]=CZERO; }
    inline bool any() const { return _charset[LAST_BYTE] != FULL; }
    inline std::size_t get_open() const { return _nxt; }
    inline std::size_t size() const { return BITS; }

    inline bool set(std::size_t bit) {
      std::size_t bin = bit/BASE;
      _charset[bin] |= (1 << (bit%BASE));
      if ( _charset[bin] != FULL ) {
        if ( bit == _nxt ) {
          for ( std::size_t i = 0; i < BASE; ++i ) {
            if ( !(_charset[bin] & (1 << i)) ) {
              _nxt = BASE*bin+i;
              break;
            }
          } // for
        }
        return true;
      }

      std::size_t idx = BITS;
      std::size_t val = bin; // val == bit/BASE
      bin = BITS;
      std::size_t parent = (bin + val);
      std::size_t pbyte = parent/BASE;
      while ( parent < TOTAL ) {
        _charset[pbyte] |= (1 << (val%BASE));
        if ( _charset[pbyte] != FULL ) {
          if ( _nxt != bit )
            return true;
          _nxt = npos;
          for ( std::size_t i = 0; i < BASE; ++i ) {
            if ( !(_charset[pbyte] & (1 << i)) ) {
              _nxt = find_open_child(pbyte*BASE+i);
              break;
            }
          } // for
          return true;
        }
        idx /= BASE;
        bin += idx;
        val /= BASE;
        parent = bin + val;
        pbyte = parent/BASE;
      } // while
      return false;
    }

    inline void set_all() {
      for ( auto& c : _charset )
        c = FULL;
      _nxt = npos;
    }

    inline void unset(std::size_t bit) {
      std::size_t bin = bit/BASE;
      bool pstatus = _charset[bin] != FULL;
      _charset[bin] &= ~(1 << (bit%BASE));
      _nxt = bit;
      if ( pstatus ) // parent wasn't FULL even before unsetting bit
        return;

      // unset parent from FULL
      std::size_t parent_bit = BITS + bin; // bin=bit/BASE
      _charset[parent_bit/BASE] &= ~(1 << (parent_bit%BASE));

      std::size_t val = BITS/BASE;
      std::size_t idx = BITS + val;
      bin /= BASE;
      while ( !pstatus && (parent_bit = idx + bin) < TOTAL ) {
        if ( _charset[parent_bit/BASE] != FULL )
          pstatus = true;
        _charset[parent_bit/BASE] &= ~(1 << (parent_bit%BASE));
        val /= BASE;
        idx += val;
        bin /= BASE;
      } // while
    }

    inline void unset_all() {
      for ( auto& c : _charset )
        c = CZERO;
      _nxt = 0;
    }

    inline std::size_t next_set(std::size_t start, std::size_t end = BITS, bool include_start = false) const {
      std::size_t beg = (start + !include_start);
      std::size_t bin = beg/BASE;
      for ( std::size_t i = beg%BASE; i < BASE; ++i ) {
        if (_charset[bin] & (1 << i))
          return bin*BASE+i;
      } // for

      for ( std::size_t b = bin+1; b <= end/BASE; ++b ) {
        if ( _charset[b] ) {
          for ( std::size_t i = 0; i < BASE; ++i ) {
            if ( _charset[b] & (1 << i) )
              return b*BASE+i;
          } // for
        }
      } // for
      return npos;
    }

    inline std::size_t next_unset(std::size_t start, bool include_start = false) const {
      std::size_t beg = (start + !include_start);
      std::size_t bin = beg/BASE;
      for ( std::size_t i = beg%BASE; i < BASE; ++i ) {
        if ( !(_charset[bin] & (1 << i)) )
          return bin*BASE+i;
      } // for

      for ( std::size_t b = bin+1; b <= BITS/BASE; ++b ) {
        if ( _charset[b] != FULL ) {
          for ( std::size_t i = 0; i < BASE; ++i ) {
            if ( !(_charset[b] & (1 << i)) )
              return b*BASE+i;
          } // for
        }
      } // for
      return npos;
    }

  private:
    inline std::size_t find_open_child(std::size_t p) {
      while ( BITS <= p ) {
        std::size_t isum = BITS;
        std::size_t inow = BITS;
        std::size_t itrail = 0, ptrail = 0;
        while ( isum < p ) {
          inow /= BASE;
          ptrail = itrail;
          itrail = isum;
          isum += inow;
        } // while
        if ( isum > p ) { isum = itrail; itrail = ptrail; }
        p -= isum;
        p *= BASE;
        p += itrail;
        std::size_t pbyte = p/BASE;
        for ( std::size_t i = 0; i < BASE; ++i ) {
          if ( !(_charset[pbyte] & (1 << i)) ) {
            p += i;
            break;
          }
        } // for
      } // while
      return p;
    }

    template <std::size_t A, std::size_t B, bool C>
    friend
    typename std::enable_if<C, std::ostream&>::type
    operator<<(std::ostream& os, const BitMonitor2<A,B,C>& r);

  private:
    static constexpr unsigned char FULL = std::numeric_limits<unsigned char>::max();
    static constexpr unsigned char CZERO = (unsigned char)0;
    static constexpr std::size_t TBYTE = TOTAL/BASE;
    static constexpr std::size_t LAST_BYTE = TBYTE-1;
    std::size_t _nxt;
    std::array<unsigned char, TBYTE> _charset;
  };

  template <std::size_t Base>
  struct BitMonitor2<Base, Base, true> {
    static constexpr std::size_t BASE = Base;
    static constexpr std::size_t TOTAL = Base;
  };

  template <std::size_t A, std::size_t B, bool C>
  typename std::enable_if<C, std::ostream&>::type
  operator<<(std::ostream& os, const BitMonitor2<A,B,C>& r) {
    std::copy(r._charset.begin(), r._charset.end(), std::ostream_iterator<std::bitset<8>>(os, " "));
    os << std::endl;
    return os;
  }

  namespace Zeros {
    std::size_t NC_ZERO = 0;
    static constexpr std::size_t C_ZERO = 0;
  } // namespace Zeros

  /* forcing N to be a power of 8 as less than 8 with a bitset is not that great,
       though it can be done by modifying Iter requirements.
  */
  template <std::size_t Base,
            std::size_t StopLevel,
            std::size_t Iter=(Base%std::numeric_limits<unsigned char>::digits==0) &&
                             (StopLevel%Base==0) &&
                             (Pow<Base, IntLogN<Base, StopLevel>::value>::value==StopLevel)>
  struct BitMonitor {
    static constexpr std::size_t BASE = Base;
    static constexpr std::size_t MYSZ = Pow<BASE,Iter>::value;
    static constexpr std::size_t PARENTSZ = Pow<BASE,Iter-1>::value;
    static constexpr std::size_t TOTALSZ = MYSZ+PARENTSZ;
    static constexpr std::size_t LEVEL = Iter;
    static constexpr bool        STOP = false;
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

    BitMonitor() : _nxt(0) {}

    inline bool any() const { return _nxt != npos; }
    inline std::size_t get_open() {
      // assume any() is true, but perhaps not _children.any()
      if ( !_children.any() )
        _children.find_open(_nxt); // ignore rtn value
      return _children.get_open();
    }

    std::size_t find_open(std::size_t pbin) {
      // don't call this - use get_open()
      pbin *= BASE;
      for ( std::size_t idx = pbin; idx < pbin+BASE; ++idx ) {
        if ( !_bset[idx] ) {
          _nxt = idx;
          return _nxt;
        }
      } // for
      return npos; // something bad just happened
    }

    // calling program can simply check return value here instead of any()
    inline bool set(std::size_t bit, std::size_t& cnt=Zeros::NC_ZERO) {
      std::size_t val = 0;
      if ( !_children.set(bit, val) ) {
        _bset[val] = true;
        cnt = val/BASE;
        std::size_t pbin = cnt*BASE;
        _nxt = npos;
        for ( std::size_t idx = pbin; idx < pbin+BASE; ++idx ) {
          if ( !_bset[idx] ) {
            _nxt = idx;
            break;
          }
        } // for
        return _nxt != npos;
      }
      return true;
    }

    inline void unset(std::size_t bit, std::size_t& cnt=Zeros::NC_ZERO) {
      _children.unset(bit, cnt);
      _bset[cnt] = false;
      _nxt = cnt;
      cnt /= BASE;
    }

    inline std::size_t size() const { return _children.size(); }

    template <std::size_t A, std::size_t B, std::size_t C>
    friend
    typename std::enable_if<!BitMonitor<A,B,C>::STOP, std::ostream&>::type
    operator<<(std::ostream& os, const BitMonitor<A,B,C>& r);

    std::size_t _nxt;
    std::bitset<MYSZ> _bset; // all zeroes by default
    BitMonitor<BASE, StopLevel/BASE, Iter+1> _children;
  };

  template <std::size_t Base, std::size_t Iter>
  struct BitMonitor<Base,Base,Iter> {
    static constexpr std::size_t BASE = Base;
    static constexpr std::size_t MYSZ = Pow<BASE,Iter>::value;
    static constexpr std::size_t PARENTSZ = Pow<BASE,Iter-1>::value;
    static constexpr std::size_t TOTALSZ = MYSZ+PARENTSZ;
    static constexpr std::size_t LEVEL = Iter;
    static constexpr bool        STOP = true;
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

    BitMonitor() : _nxt(0) {}

    inline bool any() const { return _nxt != npos; }

    inline std::size_t get_open() {
      return _nxt;
    }

    std::size_t find_open(std::size_t pbin) {
      pbin *= BASE;
      for ( std::size_t idx = pbin; idx < pbin+BASE; ++idx ) {
        if ( !_bset[idx] ) {
          _nxt = idx;
          return _nxt;
        }
      } // for
      return npos; // something bad just happened
    }

    // calling program can simply check return value here instead of any()
    inline bool set(std::size_t bit, std::size_t& cnt) {
      _bset[bit] = true;
      Zeros::NC_ZERO = Zeros::C_ZERO;

      // make pbin the group that the parent class is monitoring
      cnt = bit/BASE;
      std::size_t pbin = cnt*BASE;
      _nxt = npos;
      for ( std::size_t idx = bit+1; idx < pbin+BASE; ++idx ) {
        if ( !_bset[idx] ) {
          _nxt = idx;
          return _nxt != npos;
        }
      } // for
      for ( std::size_t idx = pbin; idx < bit; ++idx ) {
        if ( !_bset[idx] ) {
          _nxt = idx;
          return _nxt != npos;
        }
      }
      return _nxt != npos;
    }

    inline void unset(std::size_t bit, std::size_t& cnt) {
      _bset[bit] = false;
      Zeros::NC_ZERO = Zeros::C_ZERO;
      cnt = bit/BASE;
      if ( _nxt == npos ) { _nxt = bit; } 
    }

    inline std::size_t size() const { return _bset.size(); }

    template <std::size_t A, std::size_t B, std::size_t C>
    friend
    typename std::enable_if<BitMonitor<A,B,C>::STOP, std::ostream&>::type
    operator<<(std::ostream& os, const BitMonitor<A,B,C>& r);

    std::size_t _nxt;
    std::bitset<MYSZ> _bset; // initialized all-zeroes
  };

  template <std::size_t N, std::size_t M>
  struct BitMonitor<N,M,0>; // undefined
  template <std::size_t N>
  struct BitMonitor<N,N,0>; // undefined


  template <std::size_t A, std::size_t B, std::size_t C>
  typename std::enable_if<!BitMonitor<A,B,C>::STOP, std::ostream&>::type
  operator<<(std::ostream& os, const BitMonitor<A,B,C>& r) {
    os << r._bset << std::endl;
    os << r._children;
    return os;
  }

  template <std::size_t A, std::size_t B, std::size_t C>
  typename std::enable_if<BitMonitor<A,B,C>::STOP, std::ostream&>::type
  operator<<(std::ostream& os, const BitMonitor<A,B,C>& r) {
    os << r._bset;
    return os;
  }

} // namespace Ext

#endif // UTILS_BITMONITOR__
