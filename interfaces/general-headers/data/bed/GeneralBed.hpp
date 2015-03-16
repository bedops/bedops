/*
  Author: Shane Neph
  Date:   Sun Feb  8 09:17:44 PST 2015
*/
//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013, 2014 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef GENERAL_BED_HPP
#define GENERAL_BED_HPP

#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#include "suite/BEDOPS.Constants.hpp"
#include "utility/Formats.hpp"

namespace Bed
{
  class GBed {
    enum { RestColIdx = 4 };

  public:
    typedef double MeasurementType;

    GBed()
      : _chrom(""), _start(1), _end(0) { /* */ }
    GBed(char const* chr, CoordType start, CoordType end)
      : _chrom(chr), _start(start), _end(end) { /* */ }
    GBed(char const* chr, CoordType start, CoordType end, const std::vector<std::string>& rest)
      : _chrom(chr), _start(start), _end(end), _rest(rest) { /* */ }
    explicit GBed(FILE* inputFile) : _chrom(""), _start(1), _end(0)
      {
        static char cbuf[TOKEN_CHR_MAX_LENGTH + 1];
        static char rest[TOKEN_REST_MAX_LENGTH + 1];
        cbuf[0] = '\0';
        rest[0] = '\0';
        std::fscanf(inputFile, "%s\t%" SCNu64 "\t%" SCNu64 "%[^\n]s\n", cbuf, &_start, &_end, rest);
        std::fgetc(inputFile); // chomp newline

        _chrom = cbuf;

        char* pch = std::strtok(rest, "\t");
        while ( pch != NULL ) {
          _rest.push_back(pch);
          pch = std::strtok(NULL, "\t");
        } // while
      }
    explicit GBed(const std::string& inputLine) : _chrom(""), _start(1), _end(0)
      {
        static char cbuf[TOKEN_CHR_MAX_LENGTH + 1];
        static char rest[TOKEN_REST_MAX_LENGTH + 1];
        cbuf[0] = '\0';
        rest[0] = '\0';
        std::sscanf(inputLine.c_str(), "%s\t%" SCNu64 "\t%" SCNu64 "%[^\n]s\n", cbuf, &_start, &_end, rest);

        _chrom = cbuf;

        char* pch = std::strtok(rest, "\t");
        while ( pch != NULL ) {
          _rest.push_back(pch);
          pch = std::strtok(NULL, "\t");
        } // while
      }

    // Basic BED
    char const* chrom() const { return _chrom.c_str(); }
    void chrom(const std::string& chr) { _chrom = chr; }
    void start(CoordType start) { _start = start; }
    CoordType start() const { return _start; }
    void end(CoordType end) { _end = end; }
    CoordType end() const { return _end; }
    CoordType length() const { return _end - _start; }

    // Text and measures
    char const* text(int col) const { return _rest[col-RestColIdx].c_str(); }
    void text(int col, const std::string& s) { _rest[col-RestColIdx] = s; }
    MeasurementType measure(int col) const { return std::atof(_rest[col-RestColIdx].c_str()); }
    void measure(int col, MeasurementType m)
      {
        std::stringstream ss; ss << m;
        _rest[col-RestColIdx] = ss.str();
      }

    // Printing
    void print() const
      {
        std::printf("%s\t%" PRIu64 "\t%" PRIu64, _chrom.c_str(), _start, _end);
        for ( auto i : _rest )
          std::printf("\t%s", i.c_str());
      }
    void println() const
      {
        std::printf("%s\t%" PRIu64 "\t%" PRIu64, _chrom.c_str(), _start, _end);
        for ( auto i : _rest )
          std::printf("\t%s", i.c_str());
        std::printf("\n");
      }

    // Operatives
    GBed& eunion(const GBed& a) {
      if ( overlap(a) ) {
        _start = std::min(_start, a._start);
        _end = std::max(_end, a._end);
      }
      else
        _start = _end = 0;
      return *this;
    }
    GBed& intersection(const GBed& a) {
      if ( overlap(a) ) {
        _start = std::max(_start, a._start);
        _end = std::min(_end, a._end);
      }
      else
        _start = _end = 0;
      return *this;
    }
    inline CoordType overlap(const GBed& a) const {
      if ( _chrom != a._chrom )
        return 0;
      if ( _start >= a._start ) {
        if ( a._end > _start ) {
          if ( a._end > _end )
            return _end - _start;
          return a._end - _start;
        }
        else
          return 0;
      } else {
        if ( _end > a._start ) {
          if ( a._end < _end )
            return a._end - a._start;
          return _end - a._start;
        }
        else
         return 0;
      }
    }

  protected:
    std::string _chrom;
    CoordType _start;
    CoordType _end;
    std::vector<std::string> _rest;
  };

} // namespace Bed

#endif // GENERAL_BED_HPP
