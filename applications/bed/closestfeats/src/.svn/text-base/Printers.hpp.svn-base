//=========
// Author:  Shane Neph & Scott Kuehn
// Date:    Fri Aug 13 15:00:25 PDT 2010
// Project: featdist
// ID:      $Id$
//=========

// Macro Guard
#ifndef _FEATDIST_PRINTERTYPES_H
#define _FEATDIST_PRINTERTYPES_H

// Files included
#include <limits>

// Files included
#include "algorithm/visitors/helpers/ProcessBedVisitorRow.hpp"
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
  struct PrintAll : private Visitors::BedHelpers::PrintDelim {
    typedef Visitors::BedHelpers::PrintDelim Base;

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
  struct PrintShortest : private Visitors::BedHelpers::PrintDelim {
    typedef Visitors::BedHelpers::PrintDelim Base;

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
