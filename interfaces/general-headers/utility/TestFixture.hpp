/*
  FILE: TestFixture.hpp
  AUTHOR: Scott Kuehn
  CREATE DATE: Tue Aug  7 09:15:14 PDT 2007
  PROJECT: utility
  ID: $Id: TestFixture.hpp 412 2007-09-11 00:08:28Z skuehn $
*/

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
