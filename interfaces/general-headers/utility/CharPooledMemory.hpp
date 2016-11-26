/*
  Author: Shane Neph
  Date:   Thu Nov 24 20:16:29 PST 2016
*/
//
//    BEDOPS
//    Copyright (C) 2011-2016 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef __UTILS_SIMPLE_CHARARRAY_MEM__
#define __UTILS_SIMPLE_CHARARRAY_MEM__

#include <cstring>
#include <exception>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <utility>

namespace Ext {

  template <std::size_t Sz>
  struct PooledCharMemory {

    PooledCharMemory() : _curr(new MemChunk()), _cache(nullptr)
      {
        _blocks.insert(_curr);
        _blockstarts.insert(_curr->_data);
        _r.insert(std::make_pair(_curr->_data, _curr));
      }

    inline std::size_t
    nblocks() const { return _blocks.size(); }

    inline char*
    construct(char const* val)
      {
        auto p = _curr->add(val);
        if ( p )
          return p;

        /* current memory block is full */
        if ( _cache ) {
          _cache->clear();
          _curr = _cache;
          _cache = nullptr;
          _blocks.insert(_curr);
          _blockstarts.insert(_curr->_data);
          _r.insert(std::make_pair(_curr->_data, _curr));
        } else {
          for ( auto& i : _blocks ) {
            /* ensure no current block can accept val
               before adding another MemChunk
            */
            if ( i != _curr ) {
              auto q = i->add(val);
              if ( q ) {
                _curr = i;
                return q;
              }
            }
          } // for
          if ( std::strlen(val) > Sz )
            throw std::logic_error("Cannot store a string that large in PooledCharMemory<Sz>");
          _curr = new MemChunk();
          _blocks.insert(_curr);
          _blockstarts.insert(_curr->_data);
          _r.insert(std::make_pair(_curr->_data, _curr));
        }
        return _curr->add(val);
      }

    inline void
    release(char* b)
      {
        if ( 1 == _blockstarts.size() )
          _curr->remove(b);
        else { // possible Chunk removal
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

    ~PooledCharMemory()
      {
        for ( auto b : _blocks )
          delete b;
        if ( _cache )
          delete _cache;
      }

  private:
    template <std::size_t nelements>
    struct CharChunk {
      static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

      CharChunk() : _cc(0), _cntr(0), _data{}
        {
          _data[nelements] = '\0';
          /* leave _open empty until we've exhausted first pass of nelements */
        }

      inline bool
      empty() const
        {
          return _cntr == 0;
        }

      inline char*
      add(char const* c)
        {
          /* not protecting against c == nullptr in private, nested class */
          if ( _cc == nelements ) {
            auto a = find_open(c);
            if ( a )
              ++_cntr;
            return a;
          }

          std::size_t i = 0, start = _cc;
          bool ok = false;
          while ( start < nelements ) {
            _data[start++] = c[i];
            if ( c[i++] == '\0' ) {
              ok = true;
              break;
            }
          } // while

          if ( start == nelements && c[i] == '\0' ) {
            _data[start] = '\0'; // _data has nelements+1
            ok = true;
          }

          if ( ok ) {
            std::swap(start, _cc);
            ++_cntr;
            return _data+start;
          }
          return nullptr;
        }

      inline void
      remove(char const* c)
        {
          _dirty.push_back(c - _data);
          --_cntr;
        }

      inline void
      clear()
        {
          _cntr = 0;
          _cc = 0;
          _open.clear();
          _dirty.clear();
          _data[nelements] = '\0';
          _open.insert(std::make_pair(nelements, 0));
        }

    private:
      inline char*
      find_open(char const* c)
        {
          std::size_t sz = std::strlen(c);
          auto nxt = _open.upper_bound(sz); // upper_bound -> +1 dealt with
          if ( nxt != _open.end() ) {
            std::strcpy(_data+nxt->second, c);
            std::size_t newlength = (nxt->first-sz);
            if ( newlength > 1 ) // if == 1, can only hold '\0'
              _open.insert(std::make_pair(newlength-1, nxt->second+sz+1));
            _open.erase(nxt);
            return _data+nxt->second;
          }
          return clean(c, sz);
        }

      char*
      clean(char const* c, std::size_t need)
        {
          // potentially very slow; try to find first location that fits c
          for ( auto iter = _dirty.begin(); iter != _dirty.end(); iter = _dirty.erase(iter) ) {
            std::size_t pos = *iter;
            std::size_t lng = std::strlen(_data+pos);
            if ( lng >= need ) {
              std::strcpy(_data+pos, c);
              if ( lng-need > 1 ) // if == 1, can only hold '\0'
                _open.insert(std::make_pair(lng-need-1, pos+need+1));
              _dirty.erase(iter);
              return _data+pos;
            }
            else
              _open.insert(std::make_pair(lng, pos));
          } // for
          _dirty.clear();
          return nullptr;
        }

      inline std::size_t
      max() const /* not necessarily the real max if !_dirty.empty() */
        {
          if ( nelements > _cc || _dirty.empty() )
            return nelements - _cc; // if 2nd case only -> perfectly full _data
          return _open.rbegin()->first;
        }

      friend struct PooledCharMemory<nelements>;

      std::size_t _cc;
      std::size_t _cntr;
      char _data[nelements+1];
      std::multimap<std::size_t, std::size_t> _open; // size, position
      std::list<std::size_t> _dirty; // position
    };

    typedef CharChunk<Sz> MemChunk;

    MemChunk* _curr;
    MemChunk* _cache;
    // Important to remember that _blocks, _blockstarts, _r all have the same #elements
    std::set<MemChunk*, std::greater<MemChunk*>> _blocks;
    std::set<char*, std::greater<char*>> _blockstarts;
    std::map<char*, MemChunk*, std::greater<char*>> _r;
  };

} // namespace Ext

#endif // __UTILS_SIMPLE_CHARARRAY_MEM__
