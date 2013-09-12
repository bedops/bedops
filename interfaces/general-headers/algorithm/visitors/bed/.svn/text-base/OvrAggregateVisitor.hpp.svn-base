/*
  FILE: OvrAggregateVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Wed Sep  5 09:40:33 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef OVR_AGGREGATE_VISITOR_HPP
#define OVR_AGGREGATE_VISITOR_HPP

#include <set>

#include "data/bed/BedCompare.hpp"


namespace Visitors {

    namespace BedSpecific {
  
    // Collect the total number of overlapping positions
  
    template <
              typename ProcessType,
              typename BaseVisitor
             >
    struct OvrAggregate : BaseVisitor {
      typedef BaseVisitor BaseClass;
      typedef typename BaseClass::reference_type T;
      typedef typename BaseClass::mapping_type V;
  
      OvrAggregate(const ProcessType& pt = ProcessType()) : ovr_(0), refItem_(0), pt_(pt)
        { /* */ }
  
      inline void SetReference(T* t) { 
        refItem_ = t; 
        ovr_ = 0;
        if ( !cache_.empty() ) {
          for ( cacheI i = cache_.begin(); i != cache_.end(); ++i )
            ovr_ += coordCompare(refItem_, *i);
        }
      }
  
      inline void Delete(V* v) {
        cacheI iter = cache_.find(v);
        if ( iter != cache_.end() )
          ovr_ -= coordCompare(refItem_, v);
        cache_.erase(v);
      }
  
      inline void Add(V* v) {
        cache_.insert(v);
        ovr_ += coordCompare(refItem_, v);
      }
  
      inline void DoneReference() {
        pt_.operator()(ovr_);
      }
  
      virtual ~OvrAggregate() { }
  
     protected:
      inline unsigned long coordCompare(T* t, V* v) {
        if ( t->start() >= v->start() ) {
          if ( v->end() > t->start() ) {
            if( v->end() > t->end() )
              return (t->end() - t->start());
            else
              return (v->end() - t->start());
          }
          else
            return 0;
        } else { // t->start() < v->start()
          if ( t->end() > v->start() ) {
            if ( v->end() < t->end() )
              return (v->end() - v->start());
            else
              return (t->end() - v->start());
          }
          else
            return 0;
        }         
      }
  
    protected:
      typedef Bed::GenomicAddressCompare<V, V> Comp;
      typedef std::set<V*, Comp> SType;
      typedef typename SType::const_iterator cacheI;
  
      unsigned long ovr_;
      T* refItem_;
      ProcessType pt_;
      SType cache_;
    };
  
  } // namespace BedSpecific

} // namespace Visitors

#endif // OVR_AGGREGATE_VISITOR_HPP
