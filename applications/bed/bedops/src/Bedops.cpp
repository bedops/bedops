/*
  Author:  Shane Neph & Scott Kuehn
  Date:    Fri Aug 13 15:00:25 PDT 2010
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
#include <cstdlib>
#include <cstring>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "algorithm/visitors/helpers/ProcessBedVisitorRow.hpp"
#include "data/bed/AllocateIterator_BED_starch.hpp"
#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedCompare.hpp"
#include "data/bed/BedTypes.hpp"
#include "suite/BEDOPS.Constants.hpp"
#include "suite/BEDOPS.Version.hpp"
#include "utility/Exception.hpp"
#include "utility/FPWrap.hpp"
#include "utility/PooledMemory.hpp"
#include "utility/Typify.hpp"

#include "BedPadReader.hpp"
#include "Input.hpp"



namespace { // unnamed

  typedef std::pair<Bed::CoordType, Bed::CoordType> PType;

  // not a valid coordinate range
  static const PType NADA_NOTHING = std::make_pair(1, 0);

  constexpr std::size_t PoolSz = 512; // could be many input files though all will share through get_pool()
  constexpr bool NODESTRUCT = false;
  Ext::PooledMemory<Bed::B3Rest, PoolSz, NODESTRUCT> memRest;
  Ext::PooledMemory<Bed::B3NoRest, PoolSz, NODESTRUCT> memNoRest;

  inline
  void Remove(Bed::B3Rest* p) {
    memRest.release(p);
  }

  inline
  Bed::B3Rest* CopyCreate(Bed::B3Rest const* p) {
    return memRest.construct(*p);
  }

  inline
  void Remove(Bed::B3NoRest* p) {
    memNoRest.release(p);
  }

  inline
  Bed::B3NoRest* CopyCreate(Bed::B3NoRest const* p) {
    return memNoRest.construct(*p);
  }

  auto
  get_pool_details(Ext::Type2Type<Bed::B3Rest*>) -> decltype(memRest)&
    { return memRest; }

  auto
  get_pool_details(Ext::Type2Type<Bed::B3NoRest*>) -> decltype(memNoRest)&
    { return memNoRest; }

  template <typename BedTypePtr>
  auto
  get_pool() -> decltype( get_pool_details(Ext::Type2Type<BedTypePtr>()) )
    { return get_pool_details(Ext::Type2Type<BedTypePtr>()); }
} // unnamed



namespace BedOperations {

  const std::string prognm = "bedops";
  const std::string citation = BEDOPS::citation();
  const std::string version = BEDOPS::version();
  const std::string authors = "Shane Neph & Scott Kuehn";

  void doWork(const Input& input);
} // namespace BedOperations



//========
// main()
//========
int main(int argc, char** argv) {
  try {
    // Check inputs; initialize variables
    BedOperations::Input input(argc, argv);
    BedOperations::doWork(input);
    return(EXIT_SUCCESS);
  } catch(BedOperations::HelpException& h) {
    std::cout << BedOperations::prognm << std::endl;
    std::cout << "  citation: " + BedOperations::citation << std::endl;
    std::cout << "  version:  " + BedOperations::version << std::endl;
    std::cout << "  authors:  " + BedOperations::authors << std::endl;
    std::cout << BedOperations::Usage() << std::endl;
    return(EXIT_SUCCESS);
  } catch(BedOperations::ExtendedHelpException& e) {
    try {
      std::cout << BedOperations::prognm << std::endl;
      std::cout << "  citation: " + BedOperations::citation << std::endl;
      std::cout << "  version:  " + BedOperations::version << std::endl;
      std::cout << "  authors:  " + BedOperations::authors << std::endl;
      std::cout << BedOperations::DetailedUsage(e.m_) << std::endl;
    } catch(const std::exception& s) {
      std::cerr << s.what() << std::endl;
    }
    return(EXIT_SUCCESS);
  } catch(BedOperations::Version& v) {
    std::cout << BedOperations::prognm << std::endl;
    std::cout << "  citation: " + BedOperations::citation << std::endl;
    std::cout << "  version:  " + BedOperations::version << std::endl;
    std::cout << "  authors:  " + BedOperations::authors << std::endl;
    return(EXIT_SUCCESS);
  } catch(BedOperations::NoInput& ni) {
    std::cout << BedOperations::prognm << std::endl;
    std::cout << "  citation: " + BedOperations::citation << std::endl;
    std::cout << "  version:  " + BedOperations::version << std::endl;
    std::cout << "  authors:  " + BedOperations::authors << std::endl;
    std::cerr << BedOperations::Usage() << std::endl;
  } catch(std::string& s) {
    std::cerr << "May use bedops --help for more help.\n" << std::endl;
    std::cerr << "Error: " << s << std::endl;
  } catch(const std::exception& stx) {
    std::cerr << "May use bedops --help for more help.\n" << std::endl;
    std::cerr << "Error: " << stx.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown Error.  Aborting" << std::endl;
  }
  return(EXIT_FAILURE);
}



// Function implementations
namespace BedOperations {

//=========
// clean()
//=========
template <typename Cont>
void clean(Cont& c) {
  for ( typename Cont::iterator i = c.begin(); i != c.end(); ++i ) {
    if ( *i )
      delete *i;
  } // for
}

//==========
// record()
//==========
template <typename BedType>
inline void record(BedType* b) {
  static Visitors::BedHelpers::Println printer;
  printer.operator()(b);
}

//===================
// GetType<BedFiles> : get underlying BedType
//===================
template <typename BedFiles>
struct GetType {
  typedef typename NoPtr<typename BedFiles::value_type>::Type::BedType BedType;

  /*
    A c++ priority queue returns the greatest element first.  This is done, by default
      using std::less<>.  If you want the least element, you have to use std::greater<>.
    In our case, the analogs are Bed::GenomicAddressCompare for std::less<> (max element
      first), and Bed::InvertGenomicAddressCompare for std::greater<> (min element first).
  */
  typedef std::priority_queue<
                  BedType*, std::vector<BedType*>,
                  Bed::InvertGenomicAddressCompare<BedType, BedType>
                             > IPQ;

  typedef std::priority_queue<
                  BedType*, std::vector<BedType*>,
                  Bed::GenomicAddressCompare<BedType, BedType>
                             > PQ;

  template <typename BedType>
  struct InvertedBedWithFileIdx : public Bed::InvertGenomicRestAddressCompare<BedType, BedType> {
    typedef Bed::InvertGenomicRestAddressCompare<BedType, BedType> Base;
    inline bool operator()(std::pair<BedType*, int>& p1, std::pair<BedType*, int>& p2) {
      return Base::operator()(p1.first, p2.first);
    }
  };

  // use when BedType has full_rest() and you need to keep track of which file it came from
  typedef std::priority_queue<
                  std::pair<BedType*, int>, std::vector<std::pair<BedType*, int>>,
                  InvertedBedWithFileIdx<BedType>
                             > IPQ_Rest;
};

//=============
// IsSame<T,U>
//=============
template <typename T, typename U>
struct IsSame {
  static const bool same = false;
};

template <typename T>
struct IsSame<T,T> {
  static const bool same = true;
};

// Forward declarations
template <typename BedFiles>
void selectWork(const Input&, BedFiles&);

template <typename RefFile, typename NonRefFiles>
void selectWork(const Input&, RefFile&, NonRefFiles&);


//=================
// createWork<T,U>
//=================
// This structure is specialized for cases where bed_check_iterator<T*> should
//  be used in lieu of allocate_iterator_starch_bed<T*>.  Further specializations
//  include cases where there is a reference file of a different BED type
//  compared to all others.  Specifically, element-of and non-element-of need
//  B3Rest for the reference file and B3NoRest for the remaining files.  While
//  this complicates code a bit (could use B3Rest for everything), efficiency
//  gains are substantial when we can use Bed3NoRest.
template <typename IterType1, typename IterType2 = IterType1>
struct createWork;


//=================
// createWork<T,U>
//=================
template <typename IterType1, typename IterType2>
struct createWork {
  static void run(const Input& input) {
    typedef Ext::FPWrap<Ext::InvalidFile> FPType;
    typedef BedPadReader<IterType1> BedReaderType1;
    typedef BedPadReader<IterType2> BedReaderType2;
    typedef std::vector<BedReaderType2*> BedReaderContainer;

    FPType* nextFilePtr = static_cast<FPType*>(0);
    std::vector<FPType*> filePointers;
    BedReaderType1* refFile = static_cast<BedReaderType1*>(0);
    BedReaderContainer bedFiles;
    auto& mem1 = get_pool<typename IterType1::value_type>();
    auto& mem2 = get_pool<typename IterType2::value_type>();

    for ( int i = 0; i < input.NumberFiles(); ++i ) {
      nextFilePtr = new FPType(input.GetFileName(i));
      filePointers.push_back(nextFilePtr);
      if ( 0 == i ) {
        IterType1 fileI(*nextFilePtr, mem1, input.Chrom());
        refFile = new BedReaderType1(fileI, 0, 0); // never pad sole reference file
      } else {
        IterType2 t(*nextFilePtr, mem2, input.Chrom());
        bedFiles.push_back(new BedReaderType2(t, input.GetLeftPad(), input.GetRightPad()));
      }
    } // for

    try {
      selectWork(input, *refFile, bedFiles);
      if ( refFile )
        delete refFile;
      clean(filePointers);
      clean(bedFiles);
    } catch(...) {
      if ( refFile )
        delete refFile;
      clean(filePointers);
      clean(bedFiles);
      throw;
    }
  }
};

//================================
// createWork<T,T> specialization
//================================
template <typename IterType>
struct createWork<IterType, IterType> {
  static void run(const Input& input) {
    typedef Ext::FPWrap<Ext::InvalidFile> FPType;
    typedef BedPadReader<IterType> BedReaderType;
    typedef std::vector<BedReaderType*> BedReaderContainer;

    FPType* nextFilePtr = static_cast<FPType*>(0);
    std::vector<FPType*> filePointers;
    BedReaderContainer bedFiles;
    auto& mem = get_pool<typename IterType::value_type>();
    for ( int i = 0; i < input.NumberFiles(); ++i ) {
      nextFilePtr = new FPType(input.GetFileName(i));
      filePointers.push_back(nextFilePtr);
      IterType t(*nextFilePtr, mem, input.Chrom());
      bedFiles.push_back(new BedReaderType(t, input.GetLeftPad(), input.GetRightPad()));
    } // for

    try {
      selectWork(input, bedFiles);
      clean(filePointers);
      clean(bedFiles);
    } catch(...) {
      clean(filePointers);
      clean(bedFiles);
      throw;
    }
  }
};

//=================
// createWork<T,U> : specialization for error checking
//=================
template <typename BedType1, typename BedType2, std::size_t Sz>
struct createWork< Bed::bed_check_iterator<BedType1*, Sz>, Bed::bed_check_iterator<BedType2*, Sz> > {
  static void run(const Input& input) {
    typedef std::ifstream* StreamPtr;
    typedef Bed::bed_check_iterator<BedType1*, Sz> IterType1;
    typedef Bed::bed_check_iterator<BedType2*, Sz> IterType2;
    typedef BedPadReader<IterType1> BedReaderType1;
    typedef BedPadReader<IterType2> BedReaderType2;
    typedef std::vector<BedReaderType2*> BedReaderContainer;
    typedef typename Bed::bed_check_iterator<BedType1*, Sz>::Exception Error;

    StreamPtr nextFilePtr = static_cast<StreamPtr>(0);
    std::vector<StreamPtr> filePointers;
    BedReaderType1* refFile = static_cast<BedReaderType1*>(0);
    BedReaderContainer bedFiles;
    auto& mem1 = get_pool<BedType1*>();
    auto& mem2 = get_pool<BedType2*>();

    for ( int i = 0; i < input.NumberFiles(); ++i ) {
      // Create file handle iterators
      bool isStdin = (input.GetFileName(i) == "-");
      if ( !isStdin ) {
        nextFilePtr = new std::ifstream(input.GetFileName(i).c_str());
        filePointers.push_back(nextFilePtr);
      }

      if ( 0 == i ) {
        if ( isStdin ) {
          IterType1 fileI(std::cin, "stdin", mem1, input.Chrom());
          refFile = new BedReaderType1(fileI, 0, 0); // never pad sole Reference File
        } else {
          if ( !nextFilePtr || !(*nextFilePtr) )
            throw(Error("Unable to find file: " + input.GetFileName(i)));
          IterType1 fileI(*nextFilePtr, input.GetFileName(i), mem1, input.Chrom());
          refFile = new BedReaderType1(fileI, 0, 0); // never pad sole Reference File
        }
      } else {
        if ( isStdin ) {
          IterType2 fileI(std::cin, "stdin", mem2, input.Chrom());
          bedFiles.push_back(new BedReaderType2(fileI, input.GetLeftPad(), input.GetRightPad()));
        } else {
          if ( !nextFilePtr || !(*nextFilePtr) )
            throw(Error("Unable to find file: " + input.GetFileName(i)));
          IterType2 fileI(*nextFilePtr, input.GetFileName(i), mem2, input.Chrom());
          bedFiles.push_back(new BedReaderType2(fileI, input.GetLeftPad(), input.GetRightPad()));
        }
      }
    } // for

    try {
      selectWork(input, *refFile, bedFiles);
      refFile->CleanAll(); // read file all the way through for error-checking
      for ( std::size_t i = 0; i < bedFiles.size(); ++i )
        bedFiles[i]->CleanAll(); // read file all the way through for error-checking
      if ( refFile )
        delete refFile;
      clean(filePointers);
      clean(bedFiles);
    } catch(...) {
      clean(filePointers);
      clean(bedFiles);
      throw;
    }
  }
};

//=================
// createWork<T,T> : specialization for error checking
//=================
template <typename BedType, std::size_t Sz>
struct createWork< Bed::bed_check_iterator<BedType*, Sz>, Bed::bed_check_iterator<BedType*, Sz> > {
  static void run(const Input& input) {
    typedef std::ifstream* StreamPtr;
    typedef Bed::bed_check_iterator<BedType*, Sz> IterType;
    typedef BedPadReader<IterType> BedReaderType;
    typedef std::vector<BedReaderType*> BedReaderContainer;
    typedef typename Bed::bed_check_iterator<BedType*, Sz>::Exception Error;

    StreamPtr nextFilePtr = static_cast<StreamPtr>(0);
    std::vector<StreamPtr> filePointers;
    BedReaderContainer bedFiles;
    auto& mem = get_pool<BedType*>();

    for ( int i = 0; i < input.NumberFiles(); ++i ) {
      // Create file handle iterators
      bool isStdin = (input.GetFileName(i) == "-");
      if ( isStdin ) {
        IterType fileI(std::cin, "stdin", mem, input.Chrom());
        bedFiles.push_back(new BedReaderType(fileI, input.GetLeftPad(), input.GetRightPad()));
      } else {
        nextFilePtr = new std::ifstream(input.GetFileName(i).c_str());
        filePointers.push_back(nextFilePtr);
        if ( !nextFilePtr || !(*nextFilePtr) )
          throw(Error("Unable to find file: " + input.GetFileName(i)));
        IterType fileI(*nextFilePtr, input.GetFileName(i), mem, input.Chrom());
        bedFiles.push_back(new BedReaderType(fileI, input.GetLeftPad(), input.GetRightPad()));
      }
    } // for

    try {
      selectWork(input, bedFiles);
      for ( std::size_t i = 0; i < bedFiles.size(); ++i )
        bedFiles[i]->CleanAll(); // read file all the way through for error-checking
      clean(filePointers);
      clean(bedFiles);
    } catch(...) {
      clean(filePointers);
      clean(bedFiles);
      throw;
    }
  }
};

//==========
// doWork()
//==========
void doWork(const Input& input) {
  ModeType mode = input.GetModeType();
  const bool errorCheck = input.ErrorCheck();
  if ( mode == UNIONALL ) { // Keep all columns in all files
    typedef Bed::B3Rest BedType;
    if ( errorCheck )
      createWork< Bed::bed_check_iterator<BedType*, PoolSz> >::run(input);
    else
      createWork< Bed::allocate_iterator_starch_bed<BedType*, PoolSz> >::run(input);
  }
  else if ( mode == ELEMENTOF || mode == NOTELEMENTOF ) { // Keep all columns only in first file
    typedef Bed::B3Rest BedType1;
    typedef Bed::B3NoRest BedType2;
    if ( errorCheck )
      createWork< Bed::bed_check_iterator<BedType1*, PoolSz>,
                  Bed::bed_check_iterator<BedType2*, PoolSz> >::run(input);
    else
      createWork< Bed::allocate_iterator_starch_bed<BedType1*, PoolSz>,
                  Bed::allocate_iterator_starch_bed<BedType2*, PoolSz> >::run(input);
  }
  else { // Only use 3 columns
    typedef Bed::B3NoRest BedType;
    if ( errorCheck )
      createWork< Bed::bed_check_iterator<BedType*, PoolSz> >::run(input);
    else
      createWork< Bed::allocate_iterator_starch_bed<BedType*, PoolSz> >::run(input);
  }
}

//==========
// doChop()
//==========
template <typename BedFiles>
typename GetType<BedFiles>::BedType* nextMergeAllLines(int start, int end, BedFiles&);

template <typename BedFiles>
void doChop(BedFiles& bedFiles, Bed::CoordType chunkSize, Bed::CoordType stagger, bool excludeEndShort) {
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  BedType c;
  BedType* r = zero;
  bool done = false;
  while ( !done ) {
    r = nextMergeAllLines(0, bedFiles.size(), bedFiles);
    if ( !r )
      break;
    for ( auto i = r->start(); i < r->end(); ) {
      if ( 0 != std::strcmp(c.chrom(), r->chrom()) )
        c.chrom(r->chrom());
      c.start(i);
      c.end(i+chunkSize);
      if ( c.end() > r->end() ) {
        if ( excludeEndShort ) // final section < chunkSize
          break;
        c.end(r->end());
      }
      record(&c);

      if ( 0 == stagger )
        i += chunkSize;
      else
        i += stagger;
    } // for
    Remove(r);
  } // while
}

//================
// doComplement()
//================
template <typename BedFiles>
std::pair<bool, typename GetType<BedFiles>::BedType*> nextComplementLine(BedFiles&, bool);

template <typename BedFiles>
void doComplement(BedFiles& bedFiles, bool fullLeft) {
  typedef typename GetType<BedFiles>::BedType BedType;
  bool done = false;
  std::pair<bool, BedType*> nextline;
  while ( !done ) {
    nextline = nextComplementLine(bedFiles, fullLeft);
    if ( !nextline.second )
      break;
    else if ( !nextline.first ) {
      record(nextline.second);
      Remove(nextline.second);
    }
  } // while
}

//================
// doDifference()
//================
template <typename BedFiles>
std::pair<bool, typename GetType<BedFiles>::BedType*>
  nextDifferenceLine(BedFiles&,
                     typename GetType<BedFiles>::BedType*&,
                     typename GetType<BedFiles>::BedType*&);

template <typename BedFiles>
void doDifference(BedFiles& bedFiles) {
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  const int refIdx = 0;
  const int noRefIdx = 1;

  bool done = false;
  std::pair<bool, BedType*> nextline = std::make_pair(false, zero);
  BedType* nextRefMerge = zero;
  BedType* nextNonRefMerge = nextMergeAllLines(noRefIdx, bedFiles.size(), bedFiles);
  while ( !done ) {
    nextline = nextDifferenceLine(bedFiles, nextRefMerge, nextNonRefMerge);
    if ( !nextline.second )
      break;
    else if ( !nextline.first ) {
      record(nextline.second);
      if ( nextRefMerge == nextline.second ) {
        bedFiles[refIdx]->Remove(nextRefMerge);
        nextRefMerge = zero;
      }
      else // begotten from CopyCreate() in nextDifferenceLine
        Remove(nextline.second);
    }
  } // while
}

//===============
// doElementOf()
//===============
template <typename BedFile>
typename BedFile::BedType* getNextFileLine(BedFile&);

template <typename RefFile, typename NonRefFiles>
std::pair<bool, typename RefFile::BedType*>
    nextElementOfLine(typename RefFile::BedType*&, RefFile&, NonRefFiles&,
                      std::deque<typename GetType<NonRefFiles>::BedType*>&,
                      double, bool, bool);

template <typename RefFile, typename NonRefFiles>
void doElementOf(RefFile& refFile, NonRefFiles& nonRefBedFiles, double thres, bool usePerc, bool invert) {
  typedef typename RefFile::BedType RefBedType;
  typedef typename GetType<NonRefFiles>::BedType NonRefBedType;
  static RefBedType* const zero = static_cast<RefBedType*>(0);

  std::deque<NonRefBedType*> q;
  std::pair<bool, RefBedType*> r;
  RefBedType* nextRef = zero;
  NonRefBedType* tmp = nextMergeAllLines(0, nonRefBedFiles.size(), nonRefBedFiles);
  if ( tmp )
    q.push_back(tmp);
  bool done = false;
  while ( !done ) {
    std::pair<bool, RefBedType*> r = nextElementOfLine(nextRef, refFile, nonRefBedFiles, q, thres, usePerc, invert);
    if ( !nextRef )
      break;
    else if ( !r.first && r.second )
      record(nextRef);

    Remove(nextRef);
    nextRef = getNextFileLine(refFile); // unmerged; may be zero
  } // while

  while ( !q.empty() ) {
    Remove(q.front());
    q.pop_front();
  } // while
}

//==================
// doIntersection()
//==================
template <typename BedFiles>
typename GetType<BedFiles>::BedType* nextIntersectLine(BedFiles&);

template <typename BedFiles>
void doIntersection(BedFiles& bedFiles) {
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  BedType* next = zero;
  bool done = false;
  while ( !done ) {
    next = nextIntersectLine(bedFiles);
    if ( !next )
      break;
    record(next);
    Remove(next);
  } // while
}

//===========
// doMerge()
//===========
template <typename BedFiles>
void doMerge(BedFiles& bedFiles) {
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  BedType* r = zero;
  bool done = false;
  while ( !done ) {
    r = nextMergeAllLines(0, bedFiles.size(), bedFiles);
    if ( !r )
      break;
    record(r);
    Remove(r);
    r = zero;
  } // while
}

//================
// doPartitions()
//================
template <typename BedFiles, typename PQueue>
void nextPartitionGroup(BedFiles& bedFiles, PQueue& pq);

template <typename BedFiles>
void doPartitions(BedFiles& bedFiles) {
  typedef typename GetType<BedFiles>::BedType BedType;
  const bool done = false;
  typename GetType<BedFiles>::IPQ pq;

  BedType* z = static_cast<BedType*>(0);
  while ( !done ) {
    nextPartitionGroup(bedFiles, pq);
    if ( pq.empty() )
      break;

    BedType* mn = pq.top(); // minimum element
    pq.pop();
    if ( pq.empty() ) {
      record(mn);
      Remove(mn);
    } else {
      BedType* lcl = CopyCreate(mn);
      BedType* curr = mn;
      while ( !pq.empty() ) {
        BedType* ct = pq.top(); // guaranteed to overlap mn, and not extend beyond mn
        pq.pop();
        if ( curr->end() <= ct->start() ) {
          record(curr);
          Remove(curr);
          curr = ct;
        } else if ( ct->start() == curr->start() ) {
          if ( ct->end() == curr->end() ) { // dups
            Remove(ct);
          } else { // ct->end() > curr->end()
            ct->start(curr->end());
            pq.push(ct);
            while ( (z = pq.top())->start() == curr->start() ) {
              pq.pop();
              z->start(curr->end()); // z->end() > curr->end()
              pq.push(z);
            } // while
            ct = pq.top(); // may not be an element whose start() we just redefined
                           // not popping on purpose
            lcl->start(curr->start());
            lcl->end(ct->start());
            record(lcl);

            if ( curr->end() != ct->start() ) {
              // ct could be fully nested in curr
              // order could change after next modification.
              curr->start(ct->start());
              pq.push(curr);
            } else { // curr is no longer useful
              Remove(curr);
            }
            curr = pq.top();
            pq.pop();
          }
        } else { // overlap and curr->start() < ct->start()
          lcl->start(curr->start());
          lcl->end(ct->start());
          record(lcl);
          // ct could be fully nested in curr, and
          //  order could change after next modification.
          curr->start(ct->start());
          pq.push(curr);
          pq.push(ct);
          curr = pq.top();
          pq.pop();
        }
      } // while
      Remove(lcl);
      record(curr);
      Remove(curr);
    }
  } // while
}

//=========================
// doSymmetricDifference()
//=========================
template <typename BedFiles>
std::pair<bool, typename GetType<BedFiles>::BedType*> nextSymmetricDiffLine(BedFiles&);

template <typename BedType>
BedType* mergeOverlap(const BedType*, const BedType*);

template <typename BedFiles>
void doSymmetricDifference(BedFiles& bedFiles) {
  /* must cache last result to ensure all overlaps are caught */
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  bool done = false, first = true;
  std::pair<bool, BedType*> nextDiff = std::make_pair(false, zero);
  BedType* toRecord = zero;
  BedType* overlap = zero;
  while ( !done ) {
    nextDiff = nextSymmetricDiffLine(bedFiles);
    if ( !nextDiff.second ) {
      if ( !first ) {
        record(toRecord);
        Remove(toRecord);
        toRecord = zero;
      }
      break;
    }
    else if ( nextDiff.first )
      nextDiff = std::make_pair(false, zero);
    else {
      if ( !toRecord || std::strcmp(toRecord->chrom(), nextDiff.second->chrom()) != 0 ) {
        if ( !first && toRecord ) {
          record(toRecord);
          Remove(toRecord);
        }
        toRecord = nextDiff.second;
      }
      else {
        overlap = mergeOverlap(nextDiff.second, toRecord);
        if ( !overlap ) {
          if ( !first ) {
            record(toRecord);
            Remove(toRecord);
          }
          toRecord = nextDiff.second;
        }
        else {
          Remove(toRecord);
          Remove(nextDiff.second);
          toRecord = overlap;
        }
      }
      first = false;
    }
  } // while

  if ( toRecord )
    Remove(toRecord);
}

//=================================
// doUnionAll() and doUnionAllPQ()
//=================================
template <typename BedFiles>
typename std::enable_if<GetType<BedFiles>::BedType::UseRest, typename GetType<BedFiles>::BedType>::type*
nextUnionAllLine(BedFiles&); /* requires BedType::UseRest to be true */

template <typename BedFiles>
typename std::enable_if<!GetType<BedFiles>::BedType::UseRest, typename GetType<BedFiles>::BedType>::type*
nextUnionAllLine(BedFiles&); /* unimplemented on purpose */

template <typename BedFiles>
void doUnionAll(BedFiles& bedFiles) {
  /* If inputs have duplicate entries, output will too */
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);

  BedType* r = zero;
  if ( 1 == bedFiles.size() ) {
    /* a simple cat command, possibly post-padded as
         dealt with in BedPadReader. */
    while ( bedFiles[0]->HasNext() ) {
      r = bedFiles[0]->ReadLine();
      record(r);
      bedFiles[0]->Remove(r);
    } // while
    return;
  }

  const bool done = false;
  while ( !done ) {
    r = nextUnionAllLine(bedFiles);
    if ( !r )
      break;
    record(r);
    Remove(r);
  } // while
}

template <typename BedFiles>
void doUnionAllPQ(BedFiles& bedFiles) {
  /* meant for cases with large numbers of input files (50+ maybe) */
  /* If inputs have duplicate entries, output will too */
  typedef typename GetType<BedFiles>::BedType BedType;
  typename GetType<BedFiles>::IPQ_Rest pq; // union uses full_rest()
  static BedType* const zero = static_cast<BedType*>(0);

  BedType* r = zero;
  if ( 1 == bedFiles.size() ) {
    /* a simple cat command, possibly post-padded as
         dealt with in BedPadReader. */
    while ( bedFiles[0]->HasNext() ) {
      r = bedFiles[0]->ReadLine();
      record(r);
      bedFiles[0]->Remove(r);
    } // while
    return;
  }

  for ( std::size_t i = 0; i < bedFiles.size(); ++i ) {
    if ( bedFiles[i]->HasNext() )
      pq.push(std::make_pair(bedFiles[i]->ReadLine(), i));
  } // for

  while ( !pq.empty() ) {
    const std::pair<BedType*, int> val = pq.top();
    pq.pop();
    record(val.first);
    bedFiles[val.second]->Remove(val.first);
    if ( bedFiles[val.second]->HasNext() )
      pq.push(std::make_pair(bedFiles[val.second]->ReadLine(), val.second));
  } // while
}

//===========================
// getNextFileMergedCoords()
//===========================
template <typename BedFile>
typename BedFile::BedType* getNextFileMergedCoords(BedFile& bedFile) {
  // Merge coordinates within a file
  typedef typename BedFile::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  if ( !bedFile.HasNext() )
    return(zero);

  BedType* toRtn = bedFile.ReadLine();
  while ( bedFile.HasNext() ) {
    BedType* next = bedFile.ReadLine();
    BedType* merged = mergeOverlap(next, toRtn);
    if ( merged ) {
      bedFile.Remove(toRtn);
      bedFile.Remove(next);
      toRtn = merged;
      next = zero;
    } else {
      bedFile.PushBack(next);
      break;
    }
  } // while
  return(toRtn);
}

//===================
// getNextFileLine()
//===================
template <typename BedFile>
typename BedFile::BedType* getNextFileLine(BedFile& bedFile) {
  // Read next line from index i without merging
  typedef typename BedFile::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  if ( !bedFile.HasNext() )
    return(zero);
  return(bedFile.ReadLine());
}

//================
// getNextMerge()
//================
template <typename BedFiles>
typename GetType<BedFiles>::BedType*
    getNextMerge(std::deque<typename GetType<BedFiles>::BedType*>& mergeList,
                 int start, int end, BedFiles& bedFiles) {
  typedef typename GetType<BedFiles>::BedType BedType;
  if ( mergeList.empty() )
    return(nextMergeAllLines(start, end, bedFiles));
  BedType* toRtn = mergeList.front();
  mergeList.pop_front();
  return(toRtn);
}

//===============
// make_coords()
//===============
template <typename BedType>
inline PType make_coords(const BedType* b) {
  return(std::make_pair(b->start(), b->end()));
}

//===================
// intersectOverlap()
//===================
PType intersectOverlap(const PType& p1, const PType& p2) {
  Bed::CoordType min = std::max(p1.first, p2.first);
  Bed::CoordType max = std::min(p1.second, p2.second);
  return((max >= min) ? std::make_pair(min, max) : NADA_NOTHING);
}

//================
// mergeOverlap()
//================
template <typename BedType>
BedType* mergeOverlap(const BedType* p1, const BedType* p2) {
  static BedType* const zero = static_cast<BedType*>(0);
  BedType* toRtn = zero;

  if ( 0 != std::strcmp(p1->chrom(), p2->chrom()) ) {
    return(toRtn);
  } else if ( p1->start() < p2->start() ) {
    if ( p1->end() >= p2->start() ) {
      toRtn = CopyCreate(p1);
      toRtn->end(std::max(p1->end(), p2->end()));
    }
  } else if ( p1->start() > p2->start() ) {
    if ( p2->end() >= p1->start() ) {
      toRtn = CopyCreate(p2);
      toRtn->end(std::max(p1->end(), p2->end()));
    }
  } else { // p1->start() == p2->start()
    toRtn = CopyCreate(p1);
    toRtn->end(std::max(p1->end(), p2->end()));
  }
  return(toRtn);
}

//======================
// nextComplementLine()
//======================
template <typename BedFiles>
std::pair<bool, typename GetType<BedFiles>::BedType*> nextComplementLine(BedFiles& bedFiles, bool fullLeft) {
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* zero = static_cast<BedType*>(0);
  static BedType* last = static_cast<BedType*>(0);

  if ( last == zero ) {
    last = nextMergeAllLines(0, bedFiles.size(), bedFiles);
    if ( last == zero ) // stop condition
      return(std::make_pair(false, zero));
    else if ( fullLeft ) {
      // new chromosome -> first complement starts at base 0
      BedType* tmp = CopyCreate(last);
      tmp->start(0);
      tmp->end(last->start());
      if ( tmp->start() != tmp->end() )
        return(std::make_pair(false, tmp));
      else { // input coordinates include base 0 -> not part of complement
        Remove(tmp);
        return(std::make_pair(true, last));
      }
    }
  }
  BedType* nextline = nextMergeAllLines(0, bedFiles.size(), bedFiles);
  if ( nextline == zero ) { // stop condition
    Remove(last);
    last = zero;
    return(std::make_pair(false, zero));
  }

  if ( 0 != std::strcmp(nextline->chrom(), last->chrom()) ) {
    Remove(last);
    last = nextline;
    if ( fullLeft ) { // chrom change -> first complement starts at base 0
      BedType* tmp = CopyCreate(last);
      tmp->start(0);
      tmp->end(last->start());
      if ( tmp->start() != tmp->end() )
        return(std::make_pair(false, tmp));
      else { // input coordinates include base 0 -> not part of complement
        Remove(tmp);
        return(std::make_pair(true, last));
      }
    }
    return(std::make_pair(true, last));
    // direct recursion removed to prevent any possible stack overflow
    // return(nextComplementLine(bedFiles, fullLeft); // curse some more
  }

  BedType* toRtn = last;
  toRtn->start(toRtn->end());
  toRtn->end(nextline->start());
  last = nextline;
  return(std::make_pair(false, toRtn));
}

//======================
// nextDifferenceLine()
//======================
template <typename BedFiles>
std::pair<bool, typename GetType<BedFiles>::BedType*>
    nextDifferenceLine(BedFiles& bedFiles,
                       typename GetType<BedFiles>::BedType*& nextRefMerge,
                       typename GetType<BedFiles>::BedType*& nextNonRefMerge) {

  // Index 0 is the reference file
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  static const int ref = 0;
  static const int noRef = 1;
  static const bool callAgain = true;
  static const bool noRecurse = false;

  if ( !nextRefMerge && !bedFiles[ref]->HasNext() )
    return(std::make_pair(noRecurse, zero));
  else if ( !nextNonRefMerge ) { // everything else is a difference
    if ( !nextRefMerge )
      nextRefMerge = getNextFileMergedCoords(*bedFiles[ref]);
    return(std::make_pair(noRecurse, nextRefMerge));
  }

  if ( !nextRefMerge )
    nextRefMerge = getNextFileMergedCoords(*bedFiles[ref]);
  if ( !nextRefMerge )
    return(std::make_pair(noRecurse, zero));

  // Increment nextNonRefMerge until its back within range of nextRefMerge
  int cmp = std::strcmp(nextNonRefMerge->chrom(), nextRefMerge->chrom());
  while ( cmp < 0 || (0 == cmp && nextNonRefMerge->end() <= nextRefMerge->start()) ) {
    Remove(nextNonRefMerge);
    nextNonRefMerge = nextMergeAllLines(noRef, bedFiles.size(), bedFiles);
    if ( !nextNonRefMerge ) // always true after first true
      return(std::make_pair(noRecurse, nextRefMerge));
    cmp = std::strcmp(nextNonRefMerge->chrom(), nextRefMerge->chrom());
  } // while

  // Compare orientation of nextNonRefMerge and nextRefMerge
  if ( cmp > 0 || nextNonRefMerge->start() >= nextRefMerge->end() ) {
    /* no reference overlap */
    return(std::make_pair(noRecurse, nextRefMerge));
  } else if ( nextNonRefMerge->start() <= nextRefMerge->start() && nextNonRefMerge->end() >= nextRefMerge->end() ) {
    /* complete reference overlap */
    bedFiles[ref]->Remove(nextRefMerge);
    nextRefMerge = getNextFileMergedCoords(*bedFiles[ref]);
    return(std::make_pair(callAgain, nextRefMerge)); // possible for nextRefMerge to be zero
    // direct recursion removed to prevent any possible stack overflow
    // return(nextDifferenceLine(bedFiles, nextRefMerge, nextNonRefMerge)); // curse some more
  } else if ( nextNonRefMerge->start() > nextRefMerge->start() ) {
    /* difference found up to nextNonRefMerge->start() */
    BedType* toRtn = CopyCreate(nextRefMerge);
    toRtn->end(nextNonRefMerge->start());
    cmp = 0;
    nextRefMerge->start(nextNonRefMerge->end()); // safe if > nextRefMerge->end() momentarily
    while ( 0 == cmp && nextNonRefMerge->end() >= nextRefMerge->end() ) {
      bedFiles[ref]->Remove(nextRefMerge);
      nextRefMerge = getNextFileMergedCoords(*bedFiles[ref]);
      if ( !nextRefMerge )
        break;
      cmp = std::strcmp(nextNonRefMerge->chrom(), nextRefMerge->chrom());
    } // while
    return(std::make_pair(noRecurse, toRtn));
  } else { // nextNonRefMerge->start() <= nextRefMerge->start()
    /* need to shorten nextRefMerge's coordinates */
    nextRefMerge->start(nextNonRefMerge->end());
    return(std::make_pair(callAgain, nextRefMerge));
    // direct recursion removed to prevent any possible stack overflow
    // return(nextDifferenceLine(bedFiles, nextRefMerge, nextNonRefMerge)); // curse some more
  }
}

//=====================
// nextElementOfLine()
//=====================
template <typename RefFile, typename NonRefFiles>
std::pair<bool, typename RefFile::BedType*>
    nextElementOfLine(typename RefFile::BedType*& nextRef,
                      RefFile& refFile,
                      NonRefFiles& nonRefBedFiles,
                      std::deque<typename GetType<NonRefFiles>::BedType*>& mergeList,
                      double threshold, bool usePercent, bool invert) {

  // Index 0 is the reference file
  typedef typename RefFile::BedType RefBedType;
  typedef typename GetType<NonRefFiles>::BedType NonRefBedType;
  static RefBedType* const zero = static_cast<RefBedType*>(0);
  static const bool noRecurse = false;
  static const bool callAgain = true;

  if ( !nextRef && !refFile.HasNext() )
    return(std::make_pair(noRecurse, zero));
  if ( !nextRef )
    nextRef = getNextFileLine(refFile);

  NonRefBedType* nextMerge = getNextMerge(mergeList, 0, nonRefBedFiles.size(), nonRefBedFiles);
  if ( !nextMerge ) {
    if ( !invert ) // ref cannot be an element of nothing
      return(std::make_pair(noRecurse, zero));
    return(std::make_pair(noRecurse, nextRef));
  }

  // Increment nextMerge until its back within range of nextRef
  int cmp = std::strcmp(nextMerge->chrom(), nextRef->chrom());
  while ( cmp < 0 || (0 == cmp && nextMerge->end() <= nextRef->start()) ) {
    Remove(nextMerge);
    nextMerge = getNextMerge(mergeList, 0, nonRefBedFiles.size(), nonRefBedFiles);
    if ( !nextMerge ) {
      if ( !invert ) // ref cannot be an element of nothing
        return(std::make_pair(noRecurse, zero));
      return(std::make_pair(noRecurse, nextRef));
    }
    cmp = std::strcmp(nextMerge->chrom(), nextRef->chrom());
  } // while

  bool done = false;
  double rangeOverlap = 0;
  double range = nextRef->end() - nextRef->start();
  std::vector<NonRefBedType*> toPush;
  toPush.push_back(nextMerge);
  while ( !done ) {
    // Compare orientation of nextMerge and currentCoords[ref]
    if ( cmp > 0 || nextMerge->start() >= nextRef->end() ) {
      /* no reference overlap */
      break;
    }
    else { // partial or complete reference overlap
      PType lap = intersectOverlap(make_coords(nextMerge), make_coords(nextRef));
      rangeOverlap += lap.second - lap.first;
      // don't delete nextMerge -> in queue
      nextMerge = getNextMerge(mergeList, 0, nonRefBedFiles.size(), nonRefBedFiles);
      if ( !nextMerge )
        break;
      toPush.push_back(nextMerge);
      cmp = std::strcmp(nextMerge->chrom(), nextRef->chrom());
    }
  } // while

  if ( mergeList.empty() ) {
    for ( std::size_t i = 0; i < toPush.size(); ++i )
      mergeList.push_back(toPush[i]);
  }
  else { /* must push previously popped items to front of queue again */
    for ( typename std::vector<NonRefBedType*>::reverse_iterator i = toPush.rbegin(); i != toPush.rend(); ++i )
      mergeList.push_front(*i);
  }
  bool isElement = (rangeOverlap / range >= threshold);
  if ( !usePercent ) // measure in bps of overlap
    isElement = (rangeOverlap >= threshold);
  if ( invert ) // invert logic --> not element of
    return(isElement ? std::make_pair(callAgain, nextRef) : std::make_pair(noRecurse, nextRef));
  return(isElement ? std::make_pair(noRecurse, nextRef) : std::make_pair(callAgain, nextRef));
}

//=====================
// nextIntersectLine()
//=====================
template <typename BedFiles>
typename GetType<BedFiles>::BedType* nextIntersectLine(BedFiles& bedFiles) {

  // Intersect coordinates between all files.
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  BedType* toRtn = zero;
  BedType* next = zero;

  for ( std::size_t i = 0; i < bedFiles.size(); ++i ) { // find maximum minimum
    if ( !bedFiles[i]->HasNext() ) { // no more intersections
      // toRtn points to someone's cache at this point -> don't delete
      return(zero);
    }

    next = getNextFileMergedCoords(*bedFiles[i]);
    bedFiles[i]->PushBack(next); // only safe due to intersection assumptions
    if ( !toRtn ) {
      toRtn = next;
      next = zero;
      continue;
    }

    int val = std::strcmp(next->chrom(), toRtn->chrom());
    if ( 0 == val ) {
      if ( next->start() > toRtn->start() )
        toRtn = next;
    } else if ( val > 0 )
      toRtn = next;
  } // for


  toRtn = CopyCreate(toRtn);
  int marker = -1;
  Bed::CoordType maxVal = std::numeric_limits<Bed::CoordType>::max();
  for ( int i = 0; i < static_cast<int>(bedFiles.size()); ++i ) { // find next intersection
    if ( !bedFiles[i]->HasNext() ) { // no more intersections
      if ( toRtn )
        Remove(toRtn);
      return(zero);
    }

    next = getNextFileMergedCoords(*bedFiles[i]);

    int val = std::strcmp(next->chrom(), toRtn->chrom());
    while ( val < 0 || (val == 0 && next->end() <= toRtn->start()) ) {
      Remove(next);
      next = getNextFileMergedCoords(*bedFiles[i]);
      if ( !next ) { // no more intersections
        Remove(toRtn);
        return(zero);
      }
      val = std::strcmp(next->chrom(), toRtn->chrom());
    } // while

    bedFiles[i]->PushBack(next);
    if ( val > 0 || next->start() >= toRtn->end() ) {
      Remove(toRtn);
      toRtn = CopyCreate(next);
      i = -1;
      marker = -1;
      maxVal = std::numeric_limits<Bed::CoordType>::max();
      continue;
    }

    PType p = intersectOverlap(make_coords(toRtn), make_coords(next));
    toRtn->start(p.first);
    toRtn->end(p.second);
    if ( next->end() < maxVal ) {
      maxVal = next->end();
      marker = i;
    }
  } // for
  next = getNextFileMergedCoords(*bedFiles[marker]); // at least one file increment
  Remove(next);
  return(toRtn);
}

//====================
// nextMergeAllLines()
//====================
template <typename BedFiles>
typename GetType<BedFiles>::BedType*
nextMergeAllLines(int start, int end, BedFiles& bedFiles) {

  // Merge coordinates between files
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  BedType* toRtn = zero;
  BedType* bt = zero;
  bool anyNew = false;
  int minimum = start;

  // First find a minimum bed item (unmerged within a single file)
  int val = 0;
  for ( int i = start; i < end; ++i ) { // find minimum
    if ( bedFiles[i]->HasNext() ) {
      anyNew = true;
      bt = bedFiles[i]->ReadLine();
      bedFiles[i]->PushBack(bt);
      if ( !toRtn || (val = std::strcmp(bt->chrom(), toRtn->chrom())) < 0 ) {
        minimum = i;
        toRtn = bt;
      } else if ( 0 == val && bt->start() < toRtn->start() ) {
        minimum = i;
        toRtn = bt;
      }
    }
  } // for
  if ( !anyNew )
    return(zero);
  toRtn = bedFiles[minimum]->ReadLine(); // undo PushBack above for this file only
  bt = zero;

  // Increment bedfiles in search of contiguous pieces
  for ( int i = start; i < end; ++i ) {
    if ( !bedFiles[i]->HasNext() )
      continue;
    bt = bedFiles[i]->ReadLine();
    while ( 0 == (val = std::strcmp(bt->chrom(), toRtn->chrom())) && bt->end() <= toRtn->end() ) {
      bedFiles[i]->Remove(bt);
      bt = bedFiles[i]->ReadLine();
      if ( !bt )
        break;
    } // while

    // New max end coordinate?
    if ( bt &&
         0 == val &&
         bt->start() <= toRtn->end() &&
         bt->end() > toRtn->end() ) {
      toRtn->end(bt->end());
      bedFiles[i]->Remove(bt);
      i = (start - 1); // start over on next iteration
    } else if ( bt ) {
      bedFiles[i]->PushBack(bt);
    }
  } // for
  return(toRtn);
}

//======================
// nextPartitionGroup()
//======================
template <typename BedFiles, typename PQueue>
void nextPartitionGroup(BedFiles& bedFiles, PQueue& pq) {

  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  BedType* minelem = zero, * bt;
  std::size_t mn = bedFiles.size();

  // find minimum
  int val = 1;
  for ( std::size_t i = 0; i < bedFiles.size(); ++i ) {
    if ( bedFiles[i]->HasNext() ) {
      bt = bedFiles[i]->ReadLine();
      bedFiles[i]->PushBack(bt);
      if ( !minelem || (val = std::strcmp(bt->chrom(), minelem->chrom())) < 0 ) {
        if ( mn < bedFiles.size() )
          bedFiles[mn]->PushBack(minelem);
        mn = i;
        minelem = bedFiles[mn]->ReadLine();
      } else if ((0 == val) &&
                 ((bt->start() < minelem->start()) ||
                 ((bt->start() == minelem->start()) && (bt->end() < minelem->end())))) {
        bedFiles[mn]->PushBack(minelem);
        mn = i;
        minelem = bedFiles[mn]->ReadLine();
      }
    }
  } // for

  if ( !minelem )
    return;

  // find all elements that overlap the min element
  // in return queue, any overlapping element ends that go beyond min element's end,
  //   will be clipped.
  int sz = static_cast<int>(bedFiles.size());
  pq.push(minelem);
  for ( int i = 0; i < sz; ++i ) {
    typename GetType<BedFiles>::PQ lclQ;
    while ( bedFiles[i]->HasNext() ) {
      bt = bedFiles[i]->ReadLine();
      if ( strcmp(bt->chrom(), minelem->chrom()) != 0 ) { // no overlap
        bedFiles[i]->PushBack(bt);
        break;
      } else if ( bt->start() > minelem->end() ) { // no overlap
        bedFiles[i]->PushBack(bt);
        break;
      } else if ( bt->start() == minelem->end() ) { // no overlap, but be careful
        /* could be that lclQ holds elements that were previously truncated to
              start() == minelem->end().  Need to maintain sorted order.
        */
        lclQ.push(bt);
        continue;
      }

      if ( bt->start() == minelem->start() ) {
        if ( bt->end() == minelem->end() ) // duplicate
          bedFiles[i]->Remove(bt);
        else { // bt->end() > minelem->end(), no new info for pq
          bt->start(minelem->end());
          lclQ.push(bt);
          while ( bedFiles[i]->HasNext() ) {
            bt = bedFiles[i]->ReadLine();
            if ( bt->start() == minelem->end() )
              lclQ.push(bt);
            else {
              bedFiles[i]->PushBack(bt);
              break;
            }
          } // while
        }
      } else { // bt overlaps with different starting coord
        if ( bt->end() <= minelem->end() ) // fully-nested or shared-end coord
          pq.push(bt);
        else { // bt->end() > minelem->end()
          BedType* cpy = CopyCreate(bt);
          cpy->start(minelem->end());
          lclQ.push(cpy);
          bt->end(minelem->end());
          pq.push(bt);
        }
      }
    } // while

    while ( !lclQ.empty() ) {
      bedFiles[i]->PushBack(lclQ.top());
      lclQ.pop();
    } // while
  } // for
}

//=========================
// nextSymmetricDiffLine()
//=========================
template <typename BedFiles>
std::pair<bool, typename GetType<BedFiles>::BedType*>
     nextSymmetricDiffLine(BedFiles& bedFiles) {

  /* Idea: Find minimum non-overlapped region between all files */
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  static const Bed::CoordType max = std::numeric_limits<Bed::CoordType>::max();
  static const bool callAgain = true;
  static const bool noRecurse = false;

  BedType* min = zero, *next = zero, *lookahead = zero;
  std::vector<int> allMins, allNext;
  for ( std::size_t i = 0; i < bedFiles.size(); ++i ) {
    if ( !bedFiles[i]->HasNext() )
      continue;

    if ( !min ) {
      min = getNextFileMergedCoords(*bedFiles[i]);
      bedFiles[i]->PushBack(min); // only safe due to symmdiff assumptions
      next = zero;
      allNext.clear();
      allMins.clear();
      allMins.push_back(i);
      continue;
    }

    lookahead = getNextFileMergedCoords(*bedFiles[i]);
    bedFiles[i]->PushBack(lookahead); // only safe due to symmdiff assumptions

    int val = std::strcmp(lookahead->chrom(), min->chrom());
    if ( val > 0 )
      continue;
    else if ( val < 0 ) {
      min = lookahead;
      next = zero;
      allNext.clear();
      allMins.clear();
      allMins.push_back(i);
      continue;
    } else { // val == 0
      if ( lookahead->start() < min->start() ) {
        next = min;
        allNext.clear();
        allNext = allMins;
        allMins.clear();
        min = lookahead;
        allMins.push_back(i);
      }
      else if ( lookahead->start() == min->start() )
        allMins.push_back(i);
      else if ( !next || lookahead->start() < next->start() ) {
        allNext.clear();
        allNext.push_back(i);
        next = lookahead;
      }
      else if ( lookahead->start() == next->start() )
        allNext.push_back(i);
    }
  } // for

  if ( allMins.empty() )
    return(std::make_pair(noRecurse, zero));

  // Find minimum coordinate set across allMins
  Bed::CoordType minSecond = max;
  for ( std::size_t x = 0; x < allMins.size(); ++x ) {
    BedType* b = bedFiles[allMins[x]]->ReadLine();
    bedFiles[allMins[x]]->PushBack(b);
    minSecond = std::min(b->end(), minSecond);
  } // for

  Bed::CoordType nextFirst = max;
  if ( !allNext.empty() ) {
    BedType* b = bedFiles[allNext[0]]->ReadLine();
    bedFiles[allNext[0]]->PushBack(b);
    nextFirst = std::min(nextFirst, b->start());
  }

  if ( allMins.size() == 1 && allNext.empty() ) // case 1
    return(std::make_pair(noRecurse, bedFiles[allMins[0]]->ReadLine()));
  else if ( allNext.empty() ) { // case 2
    for ( std::size_t x = 0; x < allMins.size(); ++x ) {
      BedType* b = bedFiles[allMins[x]]->ReadLine();
      if ( minSecond != b->end() ) {
        b->start(minSecond);
        bedFiles[allMins[x]]->PushBack(b);
      } else {
        bedFiles[allMins[x]]->Remove(b);
      }
    } // for
    return(std::make_pair(callAgain, min)); // min just has to be nonzero; not used
    // Direct recursion removed to prevent any possible stack overflow
    // return(nextSymmetricDiffLine(bedFiles)); // curse some more
  } else if ( allMins.size() == 1 ) { // case 3
    BedType* b = bedFiles[allMins[0]]->ReadLine();
    if ( minSecond > nextFirst ) {
      BedType* c = CopyCreate(b);
      c->end(nextFirst);
      b->start(nextFirst); // multiple new mins for next time
      bedFiles[allMins[0]]->PushBack(b);
      return(std::make_pair(noRecurse, c));
    }
    else
      return(std::make_pair(noRecurse, b));
  } else { // multiple minimums, allNext nonempty: case 4
    if ( minSecond > nextFirst ) { // all minimum overlap nextFirst -> must recurse
      for ( std::size_t x = 0; x < allMins.size(); ++x ) {
        BedType* b = bedFiles[allMins[x]]->ReadLine();
        b->start(nextFirst); // new multiple mins for next time
        bedFiles[allMins[x]]->PushBack(b);
      } // for
    } else { // minSecond <= nextFirst
      for ( std::size_t x = 0; x < allMins.size(); ++x ) {
        BedType* b = bedFiles[allMins[x]]->ReadLine();
        if ( b->end() != minSecond ) {
          b->start(minSecond);
          bedFiles[allMins[x]]->PushBack(b);
        } else {
          bedFiles[allMins[x]]->Remove(b);
        }
      } // for
    }
    return(std::make_pair(callAgain, min)); // min just has to be nonzero; not used
  }
}

//====================
// nextUnionAllLine()
//====================
template <typename BedFiles>
typename std::enable_if<GetType<BedFiles>::BedType::UseRest, typename GetType<BedFiles>::BedType>::type*
nextUnionAllLine(BedFiles& bedFiles) {
  // Find next minimum entry between all files
  typedef typename GetType<BedFiles>::BedType BedType;
  static BedType* const zero = static_cast<BedType*>(0);
  BedType* first = zero;
  BedType* next = zero;
  int marker = -1;
  for ( std::size_t i = 0; i < bedFiles.size(); ++i ) { // find minimum
    if ( bedFiles[i]->HasNext() ) {
      if ( !first ) {
        first = bedFiles[i]->ReadLine();
        bedFiles[i]->PushBack(first);
        marker = i;
        continue;
      }

      next = bedFiles[i]->ReadLine();
      bedFiles[i]->PushBack(next);
      int val = std::strcmp(next->chrom(), first->chrom());
      if ( val < 0 ) {
        first = next;
        marker = i;
      } else if ( 0 == val ) {
        if ( next->start() < first->start() ) {
          first = next;
          marker = i;
        } else if ( next->start() == first->start() ) {
          if ( next->end() < first->end() ) {
            first = next;
            marker = i;
          } else if ( next->end() == first->end() ) {
            if ( strcmp(next->full_rest(), first->full_rest()) < 0 ) {
              first = next;
              marker = i;
            }
          }
        }
      }
    }
  } // for

  if ( marker < 0 )
    return(zero);
  return(bedFiles[marker]->ReadLine()); // re-purge cache in bedFiles[marker]
}

//==============
// selectWork()
//==============
template <typename BedFiles>
void selectWork(const Input& input, BedFiles& bedFiles) {

  // Iterate through all input files and output results
  ModeType modeType = input.GetModeType();
  switch ( modeType ) {
    case CHOP:
      doChop(bedFiles, input.ChopChunkSize(), input.ChopStaggerSize(), input.ChopExcludeShort());
      break;
    case COMPLEMENT:
      doComplement(bedFiles, input.ComplementFullLeft());
      break;
    case DIFFERENCE:
      doDifference(bedFiles);
      break;
    case INTERSECTION:
      doIntersection(bedFiles);
      break;
    case MERGE:
      doMerge(bedFiles);
      break;
    case PARTITION:
      doPartitions(bedFiles);
      break;
    case SYMMETRIC_DIFFERENCE:
      doSymmetricDifference(bedFiles);
      break;
    case UNIONALL:
      if ( GetType<BedFiles>::BedType::UseRest ) {
        if ( bedFiles.size() < 10 )
          doUnionAll(bedFiles);
        else
          doUnionAllPQ(bedFiles);
      } // else linker will fail
      break;
    default:
      throw(Ext::ProgramError("Unsupported mode"));
  };
}

//==============
// selectWork()
//==============
template <typename RefFile, typename NonRefFiles>
void selectWork(const Input& input, RefFile& refFile, NonRefFiles& nonRefFiles) {
  // Iterate through all input files and output results
  ModeType modeType = input.GetModeType();
  const bool doInvert = true, noInvert = false;
  switch ( modeType ) {
    case ELEMENTOF:
      doElementOf(refFile, nonRefFiles, input.Threshold(), input.UsePercentage(), noInvert);
      break;
    case NOTELEMENTOF:
      doElementOf(refFile, nonRefFiles, input.Threshold(), input.UsePercentage(), doInvert);
      break;
    default:
      throw(Ext::ProgramError("Unsupported mode"));
  };
}

} // namespace BedOperations
