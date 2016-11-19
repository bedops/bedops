#ifndef __UTILS_SIMPLE_SMALLOBJ_MEM__
#define __UTILS_SIMPLE_SMALLOBJ_MEM__

#include <algorithm>
#include <array>
#include <bitset>
#include <limits>
#include <map>
#include <ostream>
#include <set>
#include <type_traits>
#include <utility>

namespace Ext {

  template <std::size_t Base, std::size_t L, bool b=(L<=Base)>
  struct IntLogN {
    static constexpr std::size_t value = 1 + IntLogN<Base, L/Base + (L%Base > 0)>::value;
  };

  template <std::size_t Base, std::size_t L>
  struct IntLogN<Base, L, true> {
    static constexpr std::size_t value = 1;
  };

  template <std::size_t V, std::size_t W>
  struct Pow {
    static constexpr std::size_t value = V*Pow<V, W-1>::value;
  };

  template <std::size_t V>
  struct Pow<V,1> {
    static constexpr std::size_t value = V;
  };

  template <std::size_t V>
  struct Pow<V,0> {
    static constexpr std::size_t value = 0;
  };

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
  template <std::size_t N,
            std::size_t StopLevel,
            std::size_t Iter=(N%std::numeric_limits<unsigned char>::digits==0) &&
                             (StopLevel%N==0) &&
                             (Pow<N, IntLogN<N, StopLevel>::value>::value==StopLevel)>
  struct BitMonitor {
    static constexpr std::size_t NVAL = N;
    static constexpr std::size_t MYSZ = Pow<NVAL,Iter>::value;
    static constexpr std::size_t PARENTSZ = Pow<NVAL,Iter-1>::value;
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
      pbin *= NVAL;
      for ( std::size_t idx = pbin; idx < pbin+NVAL; ++idx ) {
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
        cnt = val/NVAL;
        std::size_t pbin = cnt*NVAL;
        _nxt = npos;
        for ( std::size_t idx = pbin; idx < pbin+NVAL; ++idx ) {
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
      cnt /= NVAL;
    }

    inline std::size_t size() const { return _children.size(); }

    template <std::size_t A, std::size_t B, std::size_t C>
    friend
    typename std::enable_if<!BitMonitor<A,B,C>::STOP, std::ostream&>::type
    operator<<(std::ostream& os, const BitMonitor<A,B,C>& r);

    std::size_t _nxt;
    std::bitset<MYSZ> _bset; // all zeroes by default
    BitMonitor<NVAL, StopLevel/NVAL, Iter+1> _children;
  };

  template <std::size_t N, std::size_t Iter>
  struct BitMonitor<N,N,Iter> {
    static constexpr std::size_t NVAL = N;
    static constexpr std::size_t MYSZ = Pow<NVAL,Iter>::value;
    static constexpr std::size_t PARENTSZ = Pow<NVAL,Iter-1>::value;
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
      pbin *= NVAL;
      for ( std::size_t idx = pbin; idx < pbin+NVAL; ++idx ) {
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
      cnt = bit/NVAL;
      std::size_t pbin = cnt*NVAL;
      _nxt = npos;
      for ( std::size_t idx = bit+1; idx < pbin+NVAL; ++idx ) {
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
      cnt = bit/NVAL;
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


  //==============
  // PooledMemory
  //==============
  template <typename DataType, std::size_t chunksz = 512, bool CallDestruct = false>
  struct PooledMemory; // atm, chunksz needs to be >=16 and divisible by 8

  template <typename DataType, std::size_t chunksz, bool CallDestruct>
  struct PooledMemory {
    typedef DataType type;

    PooledMemory() : _curr(new MemChunk()), _cache(nullptr)
     {
       _blocks.insert(_curr);
       _blockstarts.insert(_curr->_data);
       _r.insert(std::make_pair(_curr->_data, _curr));
     }

    template <typename... Args>
    inline
    type* construct(Args... parameters) {
      if ( !_curr->any() ) {
        if ( _cache )
          _curr = _cache;
        else
          _curr = new MemChunk();

        _cache = nullptr;
        _blocks.insert(_curr);
        _blockstarts.insert(_curr->_data);
        _r.insert(std::make_pair(_curr->_data, _curr));
      }
      return _curr->add(parameters...);
    }

    inline
    void release(type* b) {
      if ( 1 == _blockstarts.size() ) {
        _curr->remove(b);
      } else { // possible Chunk removal
        auto iter = _blockstarts.begin();
        MemChunk* m;
        if ( _blockstarts.size() < 10 ) {
          auto miter = _r.begin();
          while ( *iter > b ) { ++iter; ++miter; }
          m = miter->second;
        } else {
          iter = _blockstarts.lower_bound(b);
          m = _r[*iter];
        }
        m->remove(b);
        if ( m->empty() ) {
          _blocks.erase(m);
          _blockstarts.erase(iter);
          _r.erase(*iter);
          if ( !_cache )
            _cache = m;
          else
            delete m;
        }
      }
    }

    ~PooledMemory() {
      for ( auto b : _blocks )
        delete b;
      if ( _cache )
        delete _cache;
    }

  private:
    template <std::size_t basesz, std::size_t nelements>
    struct Chunk {
      Chunk() : _any(true), _cntr(0), _data{} { }

      inline bool any() const { return _any; } // any positions available?
      inline bool empty() const { return _cntr == 0; } // nothing set

      template <typename... Args>
      inline type* add(Args... parameters) {
        std::size_t trackpos = _tracker.get_open();
        type* address = static_cast<type*>(_data+trackpos);
        if ( CallDestruct && address ) { address->~type(); }
        _any &= _tracker.set(trackpos);
        ++_cntr;
        return new(address) type(parameters...);
      }

      inline void remove(type* bt) {
        // lazy deletion (see add())
        _tracker.unset(bt-_data);
        _any = true;
        --_cntr;
      }

      ~Chunk() {
        if ( CallDestruct ) {
          for ( std::size_t i = 0; i < _tracker.size(); ++i ) {
            type* address = static_cast<type*>(_data+i);
            if ( address ) { address->~type(); }
          } // for
        }
      }

      bool _any;
      std::size_t _cntr;
      type _data[nelements];
      //BitMonitor<basesz, nelements> _tracker; // nelements == nbits monitored
      BitMonitor2<basesz, nelements> _tracker; // nelements == nbits monitored
    };

  private:
    static constexpr std::size_t BaseSize = std::numeric_limits<unsigned char>::digits;
    static constexpr std::size_t NBits = Pow<BaseSize, IntLogN<BaseSize, chunksz>::value>::value;
    typedef Chunk<BaseSize, NBits> MemChunk;

    MemChunk* _curr;
    MemChunk* _cache;
    // Important to remember that _blocks, _blockstarts, _r all have the same #elements
    std::set<MemChunk*, std::greater<MemChunk*>> _blocks;
    std::set<type*, std::greater<type*>> _blockstarts;
    std::map<type*, MemChunk*, std::greater<type*>> _r;
  };

} // namespace Ext

#endif // __UTILS_SIMPLE_SMALLOBJ_MEM__
