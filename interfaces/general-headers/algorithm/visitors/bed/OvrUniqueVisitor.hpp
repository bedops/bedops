/*
  FILE: OvrUniqueVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Wed Sep  5 09:33:15 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef OVR_UNIQUE_VISITOR_HPP
#define OVR_UNIQUE_VISITOR_HPP

#include <set>
#include <string>

#include "data/bed/BedCompare.hpp"

namespace Visitors {

  namespace BedSpecific {
  
    template <
              typename ProcessType,
              typename BaseVisitor
             >
    struct OvrUnique : BaseVisitor {
      typedef BaseVisitor BaseClass;
      typedef typename BaseClass::reference_type RefType;
      typedef typename BaseClass::mapping_type MapType;
  
      OvrUnique(const ProcessType& pt = ProcessType()) : pt_(pt), refItem_(0)
        { /* */ }
  
      inline void SetReference(RefType* t) {
        refItem_ = t;
      }
      
      // Append an element comprised of the coords overlapping target and reference
      inline void Add(MapType* u) {
        cache_.insert(u);
      }
  
      inline void Delete(MapType* u) {
        cache_.erase(u);
      }
      
      // Calculate the sum of overlapping ranges
      inline void DoneReference() {
        unsigned int ovr = 0;
        if ( !cache_.empty() ) {
          MapType tmpOvrRange = **cache_.begin();
          cacheI i = cache_.begin();
          for ( ++i; i != cache_.end(); ++i ) {
            if ( tmpOvrRange.overlap(**i) )
              tmpOvrRange.eunion(**i);
            else {
              ovr += tmpOvrRange.intersection(*refItem_).length();
              tmpOvrRange = **i;
            }
          } // for
          ovr += tmpOvrRange.intersection(*refItem_).length();
        }
        pt_.operator()(ovr);
      }
      
      virtual ~OvrUnique() { /* */ }
  
    protected:
      typedef Bed::GenomicCompare<MapType, MapType> Comp;
      typedef std::set<MapType*, Comp> SType; // OK not to use multiset
  
      typedef typename SType::const_iterator cacheI;
  
      ProcessType pt_;
      RefType* refItem_;
      SType cache_;
    };

  } // namespace BedSpecific

} // namespace Visitors


#endif // OVR_UNIQUE_VISITOR_HPP
