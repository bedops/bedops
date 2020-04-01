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

#ifndef UTILS_SIMPLE_SMALLOBJ_MEM
#define UTILS_SIMPLE_SMALLOBJ_MEM

#include <functional>
#include <limits>
#include <map>
#include <set>
#include <utility>

#include "utility/BitMonitor.hpp"

namespace Ext {

  //==============
  // PooledMemory
  //==============
  template <typename DataType, std::size_t chunksz = 512, bool CallDestruct = false>
  struct PooledMemory; // atm, chunksz needs to be a power of 8 and at least 64

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
          if ( m == _curr )
            _curr = *(_blocks.begin()); // important if we drop back to 1 block
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

      /* temporary disable -> double destruction is happening at times
           for now, use with types that have trivial destructors
      ~Chunk() {
        if ( CallDestruct ) {
          for ( std::size_t i = 0; i < _tracker.size(); ++i ) {
            type* address = static_cast<type*>(_data+i);
            if ( address ) { address->~type(); }
          } // for
        }
      }
      */

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

#endif // UTILS_SIMPLE_SMALLOBJ_MEM
