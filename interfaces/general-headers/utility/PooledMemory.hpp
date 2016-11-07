#ifndef __UTILS_SIMPLE_SMALLOBJ_MEM__
#define __UTILS_SIMPLE_SMALLOBJ_MEM__

#include <algorithm>
#include <array>
#include <bitset>
#include <limits>
#include <map>
#include <ostream>
#include <set>
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
  struct Exp {
    static constexpr std::size_t value = V*Exp<V, W-1>::value;
  };

  template <std::size_t V>
  struct Exp<V,1> {
    static constexpr std::size_t value = V;
  };

  template <std::size_t V>
  struct Exp<V,0> {
    static constexpr std::size_t value = 0;
  };

  namespace Zeros {
    std::size_t NC_ZERO = 0;
    static constexpr std::size_t C_ZERO = 0;
  } // namespace Zeros


  /* forcing N to be a multiple of 8 as less than 8 with a bitset is not that great,
       though it can be done.  Take out (N%...::digits==0) if you want this behavior.
  */
  template <std::size_t N,
            std::size_t StopLevel,
            std::size_t Iter=(N%std::numeric_limits<unsigned char>::digits==0) &&
                             (StopLevel%N==0) &&
                             (Exp<N, IntLogN<N, StopLevel>::value>::value==StopLevel)>
  struct BitMonitor {
    static constexpr std::size_t NVAL = N;
    static constexpr std::size_t MYSZ = Exp<NVAL,Iter>::value;
    static constexpr std::size_t PARENTSZ = Exp<NVAL,Iter-1>::value;
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

    std::size_t size() const { return _children.size(); }

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
    static constexpr std::size_t MYSZ = Exp<NVAL,Iter>::value;
    static constexpr std::size_t PARENTSZ = Exp<NVAL,Iter-1>::value;
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

    std::size_t size() const { return _bset.size(); }

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
    PooledMemory() : _curr(new MemChunk())
     {
       _blocks.insert(_curr);
       _blockstarts.insert(_curr->_data);
       _r.insert(std::make_pair(_curr->_data, _curr));
     }

    template <typename... Args>
    inline
    DataType* construct(Args... parameters) {
      if ( !_curr->any() ) {
        _curr = new MemChunk();
        _blocks.insert(_curr);
        _blockstarts.insert(_curr->_data);
        _r.insert(std::make_pair(_curr->_data, _curr));
      }
      return _curr->add(parameters...);
    }

    inline
    void release(DataType* b) {
      if ( 1 == _blockstarts.size() ) {
        _curr->remove(b);
      } else { // possible Chunk removal
        auto iter = _blockstarts.lower_bound(b);
        MemChunk* m = _r[*iter];
        m->remove(b);
        if ( m->empty() && m->enough() ) {
          _blocks.erase(m);
          _blockstarts.erase(iter);
          _r.erase(*iter);
          delete m;
        }
      }
    }

    ~PooledMemory() {
      for ( auto b : _blocks )
        delete b;
    }

  private:
    template <std::size_t basesz, std::size_t nelements>
    struct Chunk {
      Chunk() : _any(true), _enough(false), _thold(nelements/4), _cntr(0), _data{} { }

      inline bool any() const { return _any; } // any positions available?
      inline bool enough() const { return _enough; } // have we ever contained _thold elements?
      inline bool empty() const { return _cntr == 0; } // nothing set

      template <typename... Args>
      inline DataType* add(Args... parameters) {
        std::size_t trackpos = _tracker.get_open();
        DataType* address = static_cast<DataType*>(_data+trackpos);
        if ( CallDestruct && address ) { address->~DataType(); }
        _any &= _tracker.set(trackpos);
        ++_cntr;
        if ( !_enough && _cntr == _thold ) // once we have added enough elements, PooledMemory might
          _enough = !_enough;              // choose to delete this after all elements are remove()'d
        return new(address) DataType(parameters...);
      }

      inline void remove(DataType* bt) {
        // lazy deletion (see add())
        _tracker.unset(bt-_data);
        _any = true;
        --_cntr;
      }

      ~Chunk() {
        if ( CallDestruct ) {
          for ( std::size_t i = 0; i < _tracker.size(); ++i ) {
            DataType* address = static_cast<DataType*>(_data+i);
            if ( address ) { address->~DataType(); }
          } // for
        }
      }

      bool _any, _enough;
      const std::size_t _thold;
      std::size_t _cntr;
      DataType _data[nelements];
      BitMonitor<basesz, nelements> _tracker; // nelements == nbits monitored
    };

  private:
    static constexpr std::size_t BaseSize = std::numeric_limits<unsigned char>::digits;
    static constexpr std::size_t NBits = Exp<BaseSize, IntLogN<BaseSize, chunksz>::value>::value;
    typedef Chunk<BaseSize, NBits> MemChunk;

    MemChunk* _curr;
    // Important to remember that _blocks, _blockstarts, _r all have the same #elements
    std::set<MemChunk*, std::greater<MemChunk*>> _blocks;
    std::set<DataType*, std::greater<DataType*>> _blockstarts;
    std::map<DataType*, MemChunk*, std::greater<DataType*>> _r;
  };

} // namespace Ext

#endif // __UTILS_SIMPLE_SMALLOBJ_MEM__
