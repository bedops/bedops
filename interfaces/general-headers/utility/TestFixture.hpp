/*
  FILE: TestFixture.hpp
  AUTHOR: Scott Kuehn
  CREATE DATE: Tue Aug  7 09:15:14 PDT 2007
  PROJECT: utility
  ID: $Id: TestFixture.hpp 412 2007-09-11 00:08:28Z skuehn $
*/

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013, 2014 Shane Neph, Scott Kuehn and Alex Reynolds
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

#include <exception>
#include <string>
#include <sstream>
#include <utility>

#ifndef TESTFIXTURE_HPP
#define TESTFIXTURE_HPP

namespace Test
{
  class TestFixture
  {
  public:
    virtual void testConstruction() const { }
    virtual void testProperties() const { }
    virtual void testComparison() const  { }
    virtual void testOperators() const { }
    virtual void testIO() const { }
    virtual ~TestFixture() { }

    std::string errorStr(const std::string& details, const std::string& fileName, int lineNum, 
			 int testNum) const
    {
      std::stringstream  errorString;
      errorString << details << " (" 
		  << fileName << ":" 
		  << lineNum << ") test " 
		  << testNum;   
      return errorString.str();
    }

  };
  
  class FixtureTestException
    : public std::exception 
  { 
  public:
    FixtureTestException(const std::string& inS) 
      : errStr_(inS) { }
    ~FixtureTestException() throw() {}
    const char* what() const throw() { return errStr_.c_str(); }
    
  private:
    std::string errStr_;
  };

  template < typename T > 
  class RunTest 
    : public std::unary_function< T, void > 
  {
  public:
    void operator() (const T testFixture) 
    {
      testFixture->testComparison();
      testFixture->testConstruction();
      testFixture->testOperators();
      testFixture->testProperties();
      testFixture->testIO();
    }
    
  };


} // namespace Test

#endif // TESTFIXTURE_HPP
