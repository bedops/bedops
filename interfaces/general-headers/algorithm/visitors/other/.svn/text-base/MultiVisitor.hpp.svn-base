/*
  FILE: MultiVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Thu Sep 20 15:00:33 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef MULTIVISITOR_HPP
#define MULTIVISITOR_HPP

#include <algorithm>
#include <functional>
#include <vector>


namespace Visitors {

  template
  <
    typename ProcessFields, // belongs to MultiVisitor only; not contained visitors
    typename ProcessRows, // belongs to MultiVisitor only; not contained visitors
    typename BaseVisitor
  >
  struct MultiVisitor : BaseVisitor {
    typedef BaseVisitor BaseClass;
    typedef ProcessFields ProcessFieldType;
    typedef ProcessRows ProcessRowType;
    typedef typename BaseClass::reference_type RefType;
    typedef typename BaseClass::mapping_type MapType;
    typedef std::vector<BaseClass*> GroupType;

    explicit MultiVisitor(GroupType& visitors,
                          const ProcessFieldType& pFields = ProcessFieldType(),
                          const ProcessRowType& pRows = ProcessRowType())
      : BaseVisitor(), t_(visitors), pFields_(pFields), pRows_(pRows)
      { /* */ }

    template <typename BaseDistType>
    explicit MultiVisitor(GroupType& visitors,
                          const BaseDistType& dist,
                          const ProcessFieldType& pFields = ProcessFieldType(),
                          const ProcessRowType& pRows = ProcessRowType())
      : BaseVisitor(dist), t_(visitors), pFields_(pFields), pRows_(pRows)
      { /* */ }

    void Add(MapType* u) {
      void (BaseClass::*memberFuncPtr)(MapType*) = &BaseClass::Add;
      std::for_each(t_.begin(), t_.end(), std::bind2nd(std::mem_fun(memberFuncPtr), u));
    }
    
    void Delete(MapType* u) {
      void (BaseClass::*memberFuncPtr)(MapType*) = &BaseClass::Delete;
      std::for_each(t_.begin(), t_.end(), std::bind2nd(std::mem_fun(memberFuncPtr), u));
    }
    
    void DoneReference() {
      typename GroupType::const_iterator i = t_.begin();
      if ( i != t_.end() )
        (*i)->DoneReference();
      else
        return;

      for ( ++i; i != t_.end(); ++i ) {
        pFields_.operator()();
        (*i)->DoneReference();
      } // for
      pRows_.operator()();
    }

    void SetReference(RefType* t) {
      void (BaseClass::*memberFuncPtr)(RefType*) = &BaseClass::SetReference;
      std::for_each(t_.begin(), t_.end(), std::bind2nd(std::mem_fun(memberFuncPtr), t));
    }

    void Purge() {
      std::for_each(t_.begin(), t_.end(), std::mem_fun(&BaseClass::Purge));
    }

    void End() {
      std::for_each(t_.begin(), t_.end(), std::mem_fun(&BaseClass::End));
    }

    virtual ~MultiVisitor() {
      for ( cGtI gi = t_.begin(); gi != t_.end(); ++gi )
        delete *gi;
    }

  protected:
    typedef typename std::vector<std::string>::const_iterator inputI;
    typedef typename GroupType::iterator gtI;
    typedef typename GroupType::const_iterator cGtI;
    GroupType& t_;
    ProcessFields pFields_;
    ProcessRows pRows_;
  };

} // namespace Visitors

#endif // MULTIVISITOR_HPP
