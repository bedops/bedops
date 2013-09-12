/*
  FILE: Exception.hpp
  AUTHOR: Shane Neph, Scott Kuehn
  CREATE DATE: Fri Aug 10 15:01:39 PDT 2007
  PROJECT: utility
  ID: $Id: Exception.hpp 1589 2010-08-12 13:19:34Z sjn $
*/

#ifndef EXCEPTIONTEMPLATE_H
#define EXCEPTIONTEMPLATE_H

#include <exception>
#include <string>

namespace Ext
{

  template <typename ExceptionType, int ErrorID>
  struct Exception 
    : public ExceptionType 
  {
    enum { Value = ErrorID };
    explicit Exception(const std::string& msg) : msg_(msg) { /* */ }
    Exception(const std::string& msg1, const std::string& msg2) : msg_(msg1 + "\n" + msg2) { /* */ }
    virtual ~Exception() throw() { /* */ }
    virtual const char* what() const throw() { return msg_.c_str(); }
  protected:
    Exception() { } // Hack.  look at traits in boost
    std::string msg_;
  };

  // Set of common application errors
  enum MajorErrors
    {
      FileErrorNum,
      UserErrorNum,
      DataErrorNum,
      ProgramErrorNum
    };
  enum FileErrorNums
    {
      InvalidFileErrorNum,
      MissingFileErrorNum
    };
  enum UserErrorNums 
    { 
      InvalidOperationErrorNum
    };
  enum DataErrorNums { };
  enum ProgramErrorNums
    {
      ParameterErrorNum,
      LogicErrorNum,
      ArgumentErrorNum
    };
  
  typedef Exception<std::exception, FileErrorNum> FileError;
  typedef Exception<FileError, InvalidFileErrorNum> InvalidFile;
  typedef Exception<FileError, MissingFileErrorNum> MissingFile;

  typedef Exception<std::exception, UserErrorNum> UserError;
  typedef Exception<UserError, InvalidOperationErrorNum> InvalidOperationError;

  typedef Exception<std::exception, DataErrorNum> DataError;

  typedef Exception<std::exception, ProgramErrorNum> ProgramError;
  typedef Exception<ProgramError, ParameterErrorNum> ParameterError;
  typedef Exception<ProgramError, LogicErrorNum> LogicError;
  typedef Exception<ProgramError, ArgumentErrorNum> ArgumentError;

}

#endif // EXCEPTIONTEMPLATE_H

