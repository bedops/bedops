/*
  FILE: BedSeqGenerator.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Wed Oct 31 09:05:17 PDT 2007
  PROJECT: utility
  ID: $Id$
*/


#ifndef BED_SEQ_GENERATOR_HPP
#define BED_SEQ_GENERATOR_HPP

#include "utility/AllocateIterator.hpp"

#include <iostream>
#include <limits>


namespace Bed {

  template< class BedType >
  class BedSeqGenerator;

  template< typename BedType >
  class BedSeqGenerator< BedType* >
  {

  public:

    typedef std::forward_iterator_tag iterator_category; //input?
    typedef BedType value_type;
    // typedef std::ptrdiff_t difference_type;
    typedef BedType* pointer;
    typedef BedType& reference;
    
    BedSeqGenerator() : validSeq_(false){ /* */}
    BedSeqGenerator(Ext::allocate_iterator< BedType* > seqData, 
                    unsigned int size = 20, unsigned int gap = 0, bool cap = true)
      : seqData_(seqData), size_(size), gap_(gap), cap_(cap),
        generatedVal_(0), currSeq_(0), validSeq_(*seqData_ ? true : false)
    { /* */ }

    reference operator*() { return *generatedVal_; }
    pointer operator->() { return &(operator*()); }

    BedSeqGenerator& operator++() {
      fetchBedSeq();
      return *this;
    }

    BedSeqGenerator operator++(int) {
      fetchBedSeq();
      return *this;
    }

    bool equal(const BedSeqGenerator& __x) const { return (validSeq_ == __x.validSeq_); }

  private:
    Ext::allocate_iterator< BedType* > seqData_, seqDataEnd_;
    unsigned int size_;
    unsigned int gap_;
    bool cap_;
    BedType* generatedVal_;
    BedType* currSeq_;
    bool validSeq_;

    // Increment file ptr and update BedSeq cache
    void fetchBedSeq() {
      if( !generatedVal_ ) {
        generatedVal_ = new BedType(**seqData_);
        currSeq_ = new BedType(**seqData_);
        seqData_++;
      } else if( (generatedVal_->end() >= currSeq_->end()) || 
          (std::string(generatedVal_->chrom()).compare(currSeq_->chrom()) > 0) ) {
        if(seqData_ == seqDataEnd_) 
          validSeq_ = false;      
        else {
          *generatedVal_ = **seqData_;          
          *currSeq_  = **seqData_;
          seqData_++;
        }
      }
      else
        generatedVal_->start(generatedVal_->end() + gap_);

      generatedVal_->end(generatedVal_->start() + size_);
      if(generatedVal_->end() >= currSeq_->end()) {
        if(seqData_ == seqDataEnd_)
          validSeq_ = false;
        else
          if(cap_)
            generatedVal_->end(currSeq_->end());
      }
    }
  };


  template < class BedType >
  inline bool 
  operator==(const BedSeqGenerator< BedType >& x,
             const BedSeqGenerator< BedType >& y) {
    return x.equal(y);
  }

  template < class BedType >
  inline bool 
  operator!=(const BedSeqGenerator< BedType >& x,
             const BedSeqGenerator< BedType >& y) {
    return !x.equal(y);
  }


} // namespace Bed


#endif // BED_SEQ_GENERATOR_HPP
