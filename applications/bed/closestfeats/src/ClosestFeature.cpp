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
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "data/bed/AllocateIterator_BED_starch.hpp"
#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedTypes.hpp"
#include "suite/BEDOPS.Constants.hpp"
#include "suite/BEDOPS.Version.hpp"
#include "utility/Exception.hpp"
#include "utility/FPWrap.hpp"
#include "utility/Typify.hpp"

#include "BedReader.hpp"
#include "Input.hpp"
#include "Printers.hpp"

namespace FeatDist {
  const std::string prognm = "closest-features";
  const std::string version = BEDOPS::version();
  const std::string authors = "Shane Neph & Scott Kuehn";
  const std::string citation = BEDOPS::citation();
  const char* none = "NA";
  const Bed::SignedCoordType plus_infinite = std::numeric_limits<Bed::SignedCoordType>::max();
  const Bed::SignedCoordType minus_infinite = std::numeric_limits<Bed::SignedCoordType>::min();
  const std::size_t PoolSz = 8*8*8; // only 2 file inputs

  void doWork(const Input&);
} // namespace FeatDist


//========
// main()
//========
int main(int argc, char** argv) {
  try {
    // Check inputs; initialize variables
    FeatDist::Input input(argc, argv);
    FeatDist::doWork(input);
    return(EXIT_SUCCESS);

  } catch(FeatDist::HelpException& h) {
    std::cout << FeatDist::prognm << std::endl;
    std::cout << "  citation: " + FeatDist::citation << std::endl;
    std::cout << "  version:  " + FeatDist::version << std::endl;
    std::cout << "  authors:  " + FeatDist::authors << std::endl;
    std::cout << FeatDist::Usage() << std::endl;
    return(EXIT_SUCCESS);
  } catch(FeatDist::VersionException& v) {
    std::cout << FeatDist::prognm << std::endl;
    std::cout << "  citation: " + FeatDist::citation << std::endl;
    std::cout << "  version:  " + FeatDist::version << std::endl;
    std::cout << "  authors:  " + FeatDist::authors << std::endl;
    return(EXIT_SUCCESS);
  } catch(FeatDist::NoInput& ni) {
    std::cerr << FeatDist::prognm << std::endl;
    std::cerr << "  citation: " + FeatDist::citation << std::endl;
    std::cerr << "  version:  " + FeatDist::version << std::endl;
    std::cerr << "  authors:  " + FeatDist::authors << std::endl;
    std::cerr << FeatDist::Usage() << std::endl;
  } catch(const std::exception& stx) {
    std::cerr << "May use " + FeatDist::prognm + " --help for more help.\n" << std::endl;
    std::cerr << "Error: " << stx.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown Error.  Aborting" << std::endl;
  }
  return(EXIT_FAILURE);
}



// Function implementations
namespace FeatDist {

template <typename BedFile1, typename BedFile2, typename Printer>
void findDistances(BedFile1&, BedFile2&, bool, const Printer&); // forward decl.


//================
// runDistances()
//================
template <typename BedFile1, typename BedFile2>
void runDistances(BedFile1& refFile, BedFile2& nonRefFile, const Input& input) {
  bool printDistances = input.PrintDistances();
  bool suppressRefField = input.SuppressReference();
  if ( input.ShortestOnly() )
    findDistances(refFile, nonRefFile, input.AllowOverlaps(), PrintShortest(input.Delimiter(), printDistances, suppressRefField));
  else
    findDistances(refFile, nonRefFile, input.AllowOverlaps(), PrintAll(input.Delimiter(), printDistances, suppressRefField));
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
// remove()
//==========
template <typename BedType>
inline
void remove(BedType* b) {
  static auto& p = get_pool<BedType*>();
  p.release(b);
}

//=================
// createWork<T,U>
//=================
template <typename BedType1, typename BedType2>
struct createWork {
  static void run(const Input& input) {
    auto& mem1 = get_pool<BedType1*>();
    auto& mem2 = get_pool<BedType2*>();
    if ( input.ErrorCheck() ) {
      typedef Ext::UserError UE;
      typedef std::ifstream* StreamPtr;
      typedef Bed::bed_check_iterator<BedType1*, PoolSz> IterType1;
      typedef Bed::bed_check_iterator<BedType2*, PoolSz> IterType2;
      typedef BedReader<IterType1> BedReaderType1;
      typedef BedReader<IterType2> BedReaderType2;

      StreamPtr refFilePtr = static_cast<StreamPtr>(0);
      StreamPtr nonRefFilePtr = static_cast<StreamPtr>(0);
      BedReaderType1* refFile = static_cast<BedReaderType1*>(0);
      BedReaderType2* nonRefFile = static_cast<BedReaderType2*>(0);

      bool isStdin = (input.GetReferenceFileName() == "-");
      refFilePtr = new std::ifstream(input.GetReferenceFileName().c_str());
      if ( isStdin ) {
        IterType1 fileI(std::cin, "stdin", mem1, input.Chrome());
        refFile = new BedReaderType1(fileI);
      } else {
        if ( !refFilePtr || !(*refFilePtr) )
          throw(UE("Unable to find file: " + input.GetReferenceFileName()));
        IterType1 fileI(*refFilePtr, input.GetReferenceFileName(), mem1, input.Chrome());
        refFile = new BedReaderType1(fileI);
      }

      isStdin = (input.GetNonReferenceFileName() == "-");
      nonRefFilePtr = new std::ifstream(input.GetNonReferenceFileName().c_str());
      if ( isStdin ) {
        IterType2 fileJ(std::cin, input.GetNonReferenceFileName(), mem2, input.Chrome());
        nonRefFile = new BedReaderType2(fileJ);
      } else {
        if ( !nonRefFilePtr || !(*nonRefFilePtr) )
          throw(UE("Unable to find file: " + input.GetNonReferenceFileName()));
        IterType2 fileJ(*nonRefFilePtr, input.GetNonReferenceFileName(), mem2, input.Chrome());
        nonRefFile = new BedReaderType2(fileJ);
      }

      runDistances(*refFile, *nonRefFile, input);

      if ( refFile ) {
        refFile->CleanAll(); // read file all the way through for error-checking
        delete refFile;
      }
      if ( nonRefFile ) {
        nonRefFile->CleanAll(); // read file all the way through for error-checking
        delete nonRefFile;
      }
      if ( refFilePtr )
        delete refFilePtr;
      if ( nonRefFilePtr )
        delete nonRefFilePtr;

    } else { // fast-mode
      typedef Ext::FPWrap<Ext::InvalidFile> FPType;
      typedef Bed::allocate_iterator_starch_bed<BedType1*, PoolSz> IterType1;
      typedef Bed::allocate_iterator_starch_bed<BedType2*, PoolSz> IterType2;
      typedef BedReader<IterType1> BedReaderType1;
      typedef BedReader<IterType2> BedReaderType2;

      FPType* refFilePtr = static_cast<FPType*>(0);
      FPType* nonRefFilePtr = static_cast<FPType*>(0);
      BedReaderType1* refFile = static_cast<BedReaderType1*>(0);
      BedReaderType2* nonRefFile = static_cast<BedReaderType2*>(0);

      refFilePtr = new FPType(input.GetReferenceFileName());
      refFile = new BedReaderType1(IterType1(*refFilePtr, mem1, input.Chrome()));

      nonRefFilePtr = new FPType(input.GetNonReferenceFileName());
      nonRefFile = new BedReaderType2(IterType2(*nonRefFilePtr, mem2, input.Chrome()));

      runDistances(*refFile, *nonRefFile, input);

      if ( refFile )
        delete refFile;
      if ( nonRefFile )
        delete nonRefFile;
      if ( refFilePtr )
        delete refFilePtr;
      if ( nonRefFilePtr )
        delete nonRefFilePtr;
    }
  }
};

//==========
// doWork()
//==========
void doWork(const Input& input) {
  typedef Bed::B3Rest BedType1;
  typedef Bed::B3Rest BedType2;
  createWork<BedType1, BedType2>::run(input);
}

//=========================
// proportionOverlapLeft()
//=========================
template <typename BedType>
double proportionOverlapLeft(BedType* const p, double marker) {
  if ( marker < p->start() )
    return(0.);
  return((marker + 1 - p->start()) / (p->end() - p->start()));
}

//===============
// getCentroid()
//===============
template <typename BedType>
inline double getCentroid(BedType* const b) {
  return((b->end() - 1.0 + b->start())/2.0);
}

//===============
// getDistance()
//===============
template <typename BedType1, typename BedType2>
inline Bed::SignedCoordType getDistance(BedType1 const* b1, BedType2 const* b2) {
  int val = 0;
  if ( 0 != (val = std::strcmp(b1->chrom(), b2->chrom())) )
    return(val < 0 ? minus_infinite : plus_infinite);
  else if ( b1->end() <= b2->start() )
    return(-1 * static_cast<Bed::SignedCoordType>(b2->start() - b1->end() + 1));
  else if ( b2->end() <= b1->start() )
    return(static_cast<Bed::SignedCoordType>(b1->start() - b2->end() + 1));
  else
    return(0); // overlap
}

//=================
// findDistances()
//=================
template <typename BedFile1, typename BedFile2, typename Printer>
void findDistances(BedFile1& ref, BedFile2& nonRef, bool allowOverlaps, const Printer& printer) {

  typedef typename BedFile1::BedType BedType1;
  typedef typename BedFile2::BedType BedType2;
  BedType2* const zero = static_cast<BedType2*>(0);
  BedType1* b = static_cast<BedType1*>(0);
  BedType2* c = zero;
  BedType2* left = zero;
  BedType2* right = zero;

  double proportion = 0;
  Bed::SignedCoordType leftDist = minus_infinite, rightDist = plus_infinite;
  Bed::SignedCoordType dist = plus_infinite;
  bool leftCached = false;
  std::list<BedType2*> read;

  while ( (b = ref.ReadLine()) ) {
    leftDist = minus_infinite;
    rightDist = plus_infinite;
    left = zero;
    right = zero;
    leftCached = false;

    while ( (c = nonRef.ReadLine()) ) {
      dist = getDistance(c, b);
      if ( dist == minus_infinite ) { // different chromosomes; catch nonref up
        remove(c);
        continue;
      } else if ( dist == plus_infinite ) { // different chromosomes; catch ref up
        if ( left && !leftCached )
          read.push_back(left);
        leftCached = static_cast<bool>(left);
        if ( right )
          read.push_back(right);
        read.push_back(c);
        break; // read next line from ref
      }

      // Deal with left and right closest elements
      if ( dist < 0 && dist >= leftDist ) {
        while ( !read.empty() ) { // new best left makes all others obsolete
          remove(read.front());
          read.pop_front();
        } // while
        if ( left && !leftCached )
          remove(left); // never closest left element again
        leftDist = dist;
        left = c;
        leftCached = false;
      } else if ( dist < 0 ) { // happens if previous element starts sooner and stops later
        if ( !leftCached ) // must keep things in sort order; left is not zero if here
          read.push_back(left);
        remove(c);
        leftCached = true;
      } else if ( dist > 0 && dist < rightDist ) {
        // right must be zero (overlap sets rightDist to 0); done processing left
        if ( left && !leftCached )
          read.push_back(left);
        leftCached = static_cast<bool>(left);
        rightDist = dist;
        right = c;
        read.push_back(c);
        break;
      } else if ( dist > 0 ) { // read one too many
        if ( left && !leftCached )
          read.push_back(left);
        leftCached = static_cast<bool>(left);
        if ( right )
          read.push_back(right);
        read.push_back(c);
        break;
      } else if ( 0 == dist && allowOverlaps ) { // overlap
        // Is new element "more closely left" or "more closely right"?
        //  Remember start coordinates only increase or stay the same.
        if ( c->start() <= b->start() ) { // hanging over left edge of ref
          if ( left && left->end() <= c->end() && !leftCached )
            remove(left); // never closest left element again
          else if ( left && !leftCached ) // current left has end coordinate greater than c
            read.push_back(left);
          left = c;
          leftDist = 0;
          leftCached = false;
        } else if ( b->end() <= c->end() ) { // anything hanging over right edge of ref gets assigned to right
          if ( left && !leftCached ) // done with left
            read.push_back(left);
          leftCached = static_cast<bool>(left);
          if ( right )
            read.push_back(right);
          right = c;
          rightDist = 0;
        } else { // completely contained within reference element
          proportion = proportionOverlapLeft(c, getCentroid(b));
          if ( 0 == leftDist ) { // left already assigned to an overlapping (more left) element
            if ( proportion < 0.5 ) { // more close to right side of reference element
              if ( !leftCached ) // leftDist == 0 -> left cannot be zero
                read.push_back(left);
              leftCached = true;
              if ( right )
                read.push_back(right);
              right = c;
              rightDist = 0;
            } else { // more close to left side of reference element
              // current left element remains our left element
              if ( !leftCached ) // must keep proper order
                read.push_back(left);
              leftCached = true;
              read.push_back(c); // may be needed in the future
            }
          } else if ( proportion >= 0.5 ) { // new left element
            while ( !read.empty() ) { // new best left makes all others obsolete
              remove(read.front());
              read.pop_front();
            } // while
            if ( left && !leftCached ) // non-overlapping, current left element
              remove(left); // never closest left element again
            leftCached = false;
            left = c;
            leftDist = 0;
          } else { // new more-right element
            if ( left && !leftCached )
              read.push_back(left);
            leftCached = static_cast<bool>(left);
            if ( right )
              read.push_back(right);
            right = c;
            rightDist = 0;
          }
        }
      } else if ( 0 == dist ) { // overlap but allowOverlaps == false
        // elements overlap but we won't use those as left or right
        // need to cache them for future reference elements, however,
        // left element is what it is now.  right is still to come.
        if ( left && !leftCached ) {
          read.push_back(left);
          leftCached = true;
        }
        read.push_back(c);
      } else {
        throw(Ext::ProgramError("Logical program error"));
      }
    } // while more in nonRef

    if ( !c && left && !leftCached )
      read.push_back(left);
    if ( !c && right )
      read.push_back(right);
    nonRef.PushBack(read);
    read.clear();

    printer(b, left, right);
    remove(b);
  } // while more in ref
}

} // namespace FeatDist
