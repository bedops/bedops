/*
  FILE: BedTypes.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Tue Aug 14 14:44:45 PDT 2007
  PROJECT: wg-pro
  ID: $Id$
*/


#ifndef BEDTYPES_HPP
#define BEDTYPES_HPP

// Files included
#include "data/bed/Bed.hpp"

namespace Bed {

  /***********************************************/
  /* Typedef helper for user applications        */
  /***********************************************/
  template <bool UseNonStaticChrom, bool UseRest, typename MType = double>
  struct BedTypes {
    typedef MType                                   MeasureType;
    typedef BasicCoords<UseNonStaticChrom, UseRest> Bed3Type;
    typedef Bed4< Bed3Type, UseRest >               Bed4Type;
    typedef Bed5< Bed4Type, MeasureType, UseRest >  Bed5Type;
    typedef Bed6< Bed5Type, UseRest >               Bed6Type;
  };

  enum { Rest = true, NoRest = false, OneChrom = false, AllChrom = true };


  /***********************************************/
  /* Common typedefs                             */
  /***********************************************/
  typedef BedTypes<AllChrom, Rest, double>    BTAllRest;
  typedef BedTypes<AllChrom, NoRest, double>  BTAllNoRest;
  typedef BedTypes<OneChrom, Rest, double>    BTOneRest;
  typedef BedTypes<OneChrom, NoRest, double>  BTOneNoRest;

  typedef BTAllRest::Bed3Type   B3Rest;
  typedef BTAllNoRest::Bed3Type B3NoRest;
  typedef BTOneRest::Bed3Type   B3OneChromRest;
  typedef BTOneNoRest::Bed3Type B3OneChromNoRest;

  typedef BTAllRest::Bed4Type   B4Rest;
  typedef BTAllNoRest::Bed4Type B4NoRest;
  typedef BTOneRest::Bed4Type   B4OneChromRest;
  typedef BTOneNoRest::Bed4Type B4OneChromNoRest;

  typedef BTAllRest::Bed5Type   B5Rest;
  typedef BTAllNoRest::Bed5Type B5NoRest;
  typedef BTOneRest::Bed5Type   B5OneChromRest;
  typedef BTOneNoRest::Bed5Type B5OneChromNoRest;

  typedef BTAllRest::Bed6Type   B6Rest;
  typedef BTAllNoRest::Bed6Type B6NoRest;
  typedef BTOneRest::Bed6Type   B6OneChromRest;
  typedef BTOneNoRest::Bed6Type B6OneChromNoRest;

} // namespace Bed

#endif // BEDTYPES_HPP
