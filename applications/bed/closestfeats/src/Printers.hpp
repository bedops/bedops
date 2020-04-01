/*
// Author:  Shane Neph & Scott Kuehn
// Date:    Fri Aug 13 15:00:25 PDT 2010
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

#ifndef _FEATDIST_PRINTERTYPES_H
#define _FEATDIST_PRINTERTYPES_H

#include <limits>

#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "suite/BEDOPS.Constants.hpp"
#include "utility/PrintTypes.hpp"
#include "utility/Typify.hpp"

namespace FeatDist {
  extern const char* none;
  extern const Bed::SignedCoordType plus_infinite;
  extern const Bed::SignedCoordType minus_infinite;

  template <typename BedType1, typename BedType2>
  inline Bed::SignedCoordType getDistance(BedType1 const*, BedType2 const*);


  //==========
  // PrintAll
  //==========
  struct PrintAll : private Visitors::Helpers::PrintDelim {
    typedef Visitors::Helpers::PrintDelim Base;

    explicit PrintAll(const std::string& delim = "|", bool printDistances = false,
                      bool suppressRefField = false)
       : Base(delim), printDist_(printDistances), suppressRefField_(suppressRefField)
      { /* */ }

    template <typename BedType1, typename BedType2>
    void operator()(BedType1* const ref, BedType2* const left, BedType2* const right) const {
      if ( !suppressRefField_ ) {
        PrintTypes::Print(*ref);
        Base::operator()();
      }

      if ( left ) {
        PrintTypes::Print(*left);
        if ( printDist_ ) {
          Base::operator()();
          PrintTypes::Print(getDistance(left, ref));
        }
      } else {
        PrintTypes::Print(none);
        if ( printDist_ ) {
          Base::operator()();
          PrintTypes::Print(none);
        }
      }

      Base::operator()();
      if ( right ) {
        if ( printDist_ ) {
          PrintTypes::Print(*right);
          Base::operator()();
          PrintTypes::Println(getDistance(right, ref));
        }
        else
          PrintTypes::Println(*right);
      } else {
        if ( printDist_ ) {
          PrintTypes::Print(none);
          Base::operator()();
          PrintTypes::Println(none);
        }
        else
          PrintTypes::Println(none);
      }
    }

  protected:
    bool printDist_;
    bool suppressRefField_;
  };


  //===============
  // PrintShortest
  //===============
  struct PrintShortest : private Visitors::Helpers::PrintDelim {
    typedef Visitors::Helpers::PrintDelim Base;

    explicit PrintShortest(const std::string& delim = "|", bool printDistances = false,
                           bool suppressRefField = false)
       : Base(delim), printDist_(printDistances), suppressRefField_(suppressRefField)
         { /* */ }

    template <typename BedType1, typename BedType2>
    void operator()(BedType1* const ref, BedType2* const left, BedType2* const right) const {
      Bed::SignedCoordType dist1 = std::numeric_limits<Bed::SignedCoordType>::max();
      Bed::SignedCoordType dist2 = dist1;
      if ( !suppressRefField_ ) {
        PrintTypes::Print(*ref);
        Base::operator()();
      }
      if ( !left && !right ) {
        if ( printDist_ ) {
          PrintTypes::Print(none);
          Base::operator()();
          PrintTypes::Println(none);
        } else {
          PrintTypes::Println(none);
        }
        return;
      }

      if ( left ) {
        if ( left->end() <= ref->start() ) { // <= not < : matches getDistance()
          dist1 = static_cast<Bed::SignedCoordType>(ref->start() - left->end() + 1);
          if ( !right ) {
            if ( printDist_ ) {
              PrintTypes::Print(*left);
              Base::operator()();
              PrintTypes::Println(getDistance(left, ref));
            }
            else {
              PrintTypes::Println(*left);
            }
            return;
          }
        } else { // must overlap or be adjacent by def'n of "left"
          if ( printDist_ ) {
            PrintTypes::Print(*left);
            Base::operator()();
            PrintTypes::Println(0);
          }
          else
            PrintTypes::Println(*left);
          return;
        }
      }

      if ( right ) {
        if ( !left ) {
          if ( printDist_ ) {
            PrintTypes::Print(*right);
            Base::operator()();
            PrintTypes::Println(getDistance(right, ref));
          }
          else
            PrintTypes::Println(*right);
          return;
        }

        if ( ref->end() <= right->start() ) // <= not < : matches getDistance()
          dist2 = static_cast<Bed::SignedCoordType>(right->start() - ref->end() + 1);
        else { // must overlap or be adjacent by def'n of "right"
          if ( printDist_ ) {
            PrintTypes::Print(*right);
            Base::operator()();
            PrintTypes::Println(0);
          }
          else
            PrintTypes::Println(*right);
          return;
        }
      }

      if ( dist1 <= dist2 ) {
        if ( printDist_ ) {
          PrintTypes::Print(*left);
          Base::operator()();
          PrintTypes::Println(getDistance(left, ref));
        }
        else
          PrintTypes::Println(*left);
      } else {
        if ( printDist_ ) {
          PrintTypes::Print(*right);
          Base::operator()();
          PrintTypes::Println(getDistance(right, ref));
        }
        else
          PrintTypes::Println(*right);
      }
    }

  protected:
    bool printDist_;
    bool suppressRefField_;
  };

} // namespace FeatDist

#endif // _FEATDIST_PRINTERTYPES_H
