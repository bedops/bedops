/*
  Author:  Shane Neph & Alex Reynolds
  Date:    Mon Jan 23 06:29:10 PST 2012
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

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "algorithm/bed/FindBedRange.hpp"
#include "algorithm/visitors/helpers/ProcessBedVisitorRow.hpp"
//#include "data/bed/BedTypes.hpp"
#include "data/bed/BedCheckIterator.hpp"
#include "suite/BEDOPS.Constants.hpp"
#include "suite/BEDOPS.Version.hpp"
#include "utility/Exception.hpp"

namespace {

  const std::string prognm = "bedextract";
  const std::string citation = BEDOPS::citation();
  const std::string version = BEDOPS::version();
  const std::string authors = "Shane Neph & Alex Reynolds";
  constexpr std::size_t PoolSz = 2*8;

  //======
  // Help
  //======
  struct Help {};

  //=========
  // Version
  //=========
  struct Version {};

  //=======
  // Cache
  //=======
  template <typename ValueType>
  struct Cache {
    Cache() : empty_(true)
      { }

    template <typename T>
    void operator()(const T* t) {
      empty_ = false;
    }

    bool Empty() const { return empty_; }
    void Clear() const { empty_ = true; }

  private:
    bool empty_;
  };

  using Bed::extract_details::TargetBedType;
  using Bed::extract_details::QueryBedType;
  using Bed::ByteOffset;
  typedef Bed::CoordType CoordType;

  //=======
  // Input
  //=======
  struct Input {

    enum OpMode { TWOFILE, ONECHROME, CHROMELIST };

    Input(int argc, char** argv) : m_(TWOFILE), f1_(NULL), f2_(NULL), f2Name_(""), chrom_("") {
      for ( int i = 1; i < argc; ++i ) {
        if ( std::string(argv[i]) == "--help" )
          throw(Help());
        else if ( std::string(argv[i]) == "--version" )
          throw(Version());
      } // for

      int argcntr = 1;
      if ( argc != 3 ) {
        if ( 1 == argc )
          throw(std::string("")); // no input args
        throw(std::string("Wrong # args"));
      }


      if ( std::string(argv[argcntr]) == "--list-chr" || std::string(argv[argcntr]) == "--listchr" ) {
        // still silently support --listchr
        m_ = CHROMELIST;
        std::string fn = argv[++argcntr];
        if ( fn == "-" )
          throw("No stdin support for first file given to " + prognm);
        f1_ = std::fopen(fn.c_str(), "rb");
        if ( f1_ == NULL )
          throw("Unable to find file: " + fn);
        else if ( starch::Starch::isStarch(f1_) )
          throw("Starch format is not supported with first file given to " + prognm + std::string("\nProblem file: ") + fn);
      } else {
        m_ = TWOFILE;
        std::string fn = argv[argcntr];
        if ( fn == "-" )
          throw("No stdin support for first file given to " + prognm + ".");
        f1_ = std::fopen(fn.c_str(), "rb");
        if ( f1_ == NULL ) {
          if ( fn.find("--") == 0 )
            throw("Unrecognized option: " + fn + " given to " + prognm + "."); 
          m_ = ONECHROME;
          chrom_ = argv[argcntr];
          fn = argv[++argcntr];
          if ( fn == "-" )
            throw("No stdin support for first file given to " + prognm +
                  ". Using <chromosome> = " + chrom_ + " since " + chrom_ + " is no file.");
          f1_ = std::fopen(fn.c_str(), "rb");
          if ( f1_ == NULL )
            throw("Unable to find file: " + std::string(argv[argcntr]));
        } else {
          ++argcntr;
          if ( std::string(argv[argcntr]) == "-" )
            f2_ = stdin, f2Name_ = "stdin";
          else
            f2_ = std::fopen(argv[argcntr], "rb"), f2Name_ = argv[argcntr];
          if ( f2_ == NULL )
            throw("Unable to find file: " + f2Name_);
        }

        if ( starch::Starch::isStarch(f1_) )
          throw("Starch format is not supported with first file given to " + prognm + std::string("\nProblem file: ") + fn);
      }
    }

    OpMode Mode() const { return m_; }
    FILE* File1() const { return f1_; }
    FILE* File2() const { return f2_; }
    std::string File2Name() const { return f2Name_; }
    std::string Chrome() const { return(chrom_); }

    ~Input() {
      if ( f1_ != NULL )
        std::fclose(f1_);
      if ( f2_ != NULL && f2_ != stdin )
        std::fclose(f2_);
    }

  private:
    OpMode m_;
    FILE* f1_;
    FILE* f2_;
    std::string f2Name_;
    std::string chrom_;
  };


  //=========
  // usage()
  //=========
  std::string usage() {
     std::string msg = "\n    Every input file must be sorted per sort-bed.\n\n";
     msg += " USAGE:\n";
     msg += "   0) --help or --version           Print requested info and exit successfully\n";
     msg += "   1) --list-chr <input.bed>        Print all unique chromosome names found in <input.bed>\n";
     msg += "   2) <chromosome> <input.bed>      Retrieve all rows for chr8 with:  bedextract chr8 <input.bed>\n";
     msg += "   3) <query.bed> <target>          Grab elements from the <query.bed> that overlap elements in <target>. Same as\n";
     msg += "                                     `bedops -e 1 <query.bed> <target>`, except that this option fails silently\n";
     msg += "                                      if <query.bed> contains fully-nested BED elements.  If no fully-nested\n";
     msg += "                                      element exists, bedextract can vastly improve upon the performance of bedops.\n";
     msg += "                                      <target> may be a BED or Starch file (with or without fully-nested elements).\n";
     msg += "                                      Using '-' for <target> indicates input (in BED format) comes from stdin.\n";
     return(msg);
  }

  //===============
  // skipHeaders()
  //===============
  bool skipHeaders(FILE* f) {
    // move beyond any silly headers
    std::rewind(f);
    ByteOffset no_header = std::ftell(f);

    bool firstword = true, firstletter = true, isheader = false;
    std::string word;
    char c;
    while ( (c = static_cast<char>(std::fgetc(f))) != EOF ) {
      if ( c == '\n' ) {
        if ( isheader || word == "browser" || word == "track" ) { // 'browser' may be only word on line
          word = "";
          firstword = true;
          firstletter = true;
          isheader = false;
          no_header = std::ftell(f);
        } else {
          break;
        }
      } else if ( isheader ) {
        continue;
      } else {
        if ( c == ' ' || c == '\t' ) { // know it's not a newline; first word completed
          firstword = false;
          if ( word == "browser" || word == "track" ) // UCSC BED headers
            isheader = true;
          else
            break;
        }
        if ( firstword )
          word += c;
        if ( firstletter && (c == '@' || c == '#') ) // SAM and VCF format headers
          isheader = true;
        firstletter = false;
      }
    } // while
    std::fseek(f, no_header, SEEK_SET);
    return(c == EOF);
  }

  //=============
  // get_pool()
  //============
  template <typename BedTypePtr>
  Ext::PooledMemory<typename std::remove_pointer<BedTypePtr>::type, PoolSz>&
  get_pool() {
    static Ext::PooledMemory<typename std::remove_pointer<BedTypePtr>::type, PoolSz> pool;
    return pool;
  }

  //==========
  // doWork()
  //==========
  void doWork(const Input& input) {
    // execute the mode requested by the user
    Input::OpMode mode = input.Mode();
    FILE* f = input.File1();
    bool at_end = skipHeaders(f);
    if ( at_end )
      return;
    ByteOffset no_header = std::ftell(f);

    if ( mode == Input::TWOFILE ) { // find elements of file1 that overlap specified ranges of file2
      Visitors::BedHelpers::Println printer;
      if ( input.File2Name() == "stdin" ) {
        auto& mem = get_pool<TargetBedType*>();
        Bed::bed_check_iterator<TargetBedType*, PoolSz> titer(std::cin, input.File2Name(), mem), teof;
        find_bed_range(f, titer, teof, printer);
      } else {
        std::ifstream* infile = new std::ifstream(input.File2Name().c_str());
        auto& mem = get_pool<TargetBedType*>();
        Bed::bed_check_iterator<TargetBedType*, PoolSz> titer(*infile, input.File2Name(), mem), teof;
        find_bed_range(f, titer, teof, printer);
        delete infile;
      }
    } else if ( mode == Input::CHROMELIST ) {
      bool done = false, first = true;
      Cache<TargetBedType> cache;
      std::vector<TargetBedType*> v;
      std::pair<bool, ByteOffset> lbound;

      std::fseek(f, 0, SEEK_END);  // apparently dangerous on some platforms in binary mode -> padded nulls;
      const ByteOffset at_end = std::ftell(f); // I'll assume msft is the problem until I know better
      std::fseek(f, no_header, SEEK_SET);

      while ( !done ) {
        TargetBedType* bt = new TargetBedType(f); // find_bed_range cleans up for us
        bt->start(std::numeric_limits<CoordType>::max()-1);
        bt->end(bt->start()+1);

        if ( first ) {
          printf("%s\n", bt->chrom());
          first = !first;
        }

        v.push_back(bt);
        lbound = find_bed_range(f, v.begin(), v.end(), cache);
        v.clear(); // bt already deleted
        if ( !cache.Empty() )
          throw(std::string("program error: bed coordinates beyond designed range\n"));
        if ( lbound.first && lbound.second != at_end ) {
          std::fseek(f, lbound.second, SEEK_SET);
          ByteOffset b = std::ftell(f);
          auto q = new QueryBedType(f);
          printf("%s\n", q->chrom());
          delete q;
          std::fseek(f, b, SEEK_SET);
        }
        else
          break;
      } // while
    } else if ( mode == Input::ONECHROME ) {
      std::vector<TargetBedType*> v;
      TargetBedType* bt = new TargetBedType;
      std::string tmp = input.Chrome();
      bt->chrom(tmp.c_str());
      bt->start(0);
      bt->end(std::numeric_limits<CoordType>::max());
      v.push_back(bt);
      Visitors::BedHelpers::Println printer;
      find_bed_range(f, v.begin(), v.end(), printer); // routine deletes bt for us
    } else
      throw(std::string("program error : unrecognized mode"));
  }

} // unnamed


//========
// main()
//========
int main(int argc, char** argv) {
  bool isok = false;
  try {
    Input input(argc, argv);
    doWork(input);
    isok = true;
  } catch(const Help& h) {
    std::cout << prognm << std::endl;
    std::cout << "  citation: " + citation << std::endl;
    std::cout << "  version:  " + version << std::endl;
    std::cout << "  authors:  " + authors << std::endl;
    std::cout << usage() << std::endl;
    isok = true;
  } catch(const Version& v) {
    std::cout << prognm << std::endl;
    std::cout << "  citation: " + citation << std::endl;
    std::cout << "  version:  " + version << std::endl;
    std::cout << "  authors:  " + authors << std::endl;
    isok = true;
  } catch(std::string& msg) {
    std::cerr << prognm << std::endl;
    std::cerr << "  citation: " + citation << std::endl;
    std::cerr << "  version:  " + version << std::endl;
    std::cerr << "  authors:  " + authors << std::endl;
    std::cerr << usage() << std::endl;
    std::cerr << msg << std::endl;
  } catch(std::exception& e) {
    std::cerr << usage() << std::endl;
    std::cerr << e.what() << std::endl;
  } catch(...) {
    std::cerr << usage() << std::endl;
    std::cerr << "Unknown Error.  Aborting" << std::endl;
  }
  return(isok ? EXIT_SUCCESS : EXIT_FAILURE);
}
