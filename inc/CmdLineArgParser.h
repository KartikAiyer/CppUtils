/**
 * The MIT License (MIT)
 *
 * Copyright (c) <2014> <Kartik Aiyer>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef __CMD_LINE_ARG_PARSER_H__
#define __CMD_LINE_ARG_PARSER_H__

#include <EnumFactory.h>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <memory>

using namespace std;
namespace CppUtils
{
/**
 * C++ Command Line argument parser for getopt() flavored command line parsing.
 * 1. User creates a parser
 * 2. User registers options with short and long forms and callbacks when the option is encountered
 * 3. User calls Parse() on the parser object suppling the command line arguments as parameters to the method
 */

#define CLAP_OPTION_TYPE(XX)\
  XX(eArgNone,)\
  XX(eArgRequired,)\
  XX(eArgOptional,)

#define CLAP_CB_RESULTS( XX )\
  XX(eCmdParseSuccess,)\
  XX(eCmdParseOptionalArgumentAccepted,)\
  XX(eCmdParseArgumentNotAccepted, )

/**
 * CmdLineOption - The structure encapsulates the properties of the user supplied
 * option. Options can have a short name, an optional long name and an optional callback
 * when encountered
 *
 */
struct CmdLineOption
{
public:

  DECLARE_ENUM( ArgumentType, CLAP_OPTION_TYPE )
  DECLARE_ENUM( CbParseResults, CLAP_CB_RESULTS )
  using CmdLineOptionMatcher = function<CbParseResults( CmdLineOption&, string )>;
  CmdLineOption() {}
  /**
   *
   * @param sName - The short name (do not include the option identifier, ie don't add the '-'
   * @param lName - The long name, ditto...
   * @param type  - The argument type.
   * @param fn  - An Optional callback of type CmdLineOptionMatcher
   * @return
   */
  CmdLineOption( string sName, string lName, ArgumentType type, CmdLineOptionMatcher fn = nullptr ) :
      m_shortName{ sName },
      m_longName{ lName },
      m_fnMatch{ fn },
      m_type{ type }
  {
  }


  string m_shortName = "";
  string m_longName = "";
  CmdLineOptionMatcher m_fnMatch;
  ArgumentType m_type = eArgNone;
};

#define CLAP_RESULT_TYPES(XX)\
  XX(eCmdLineArgParseSuccess,)\
  XX(eCmdLineArgInvalidOptionFound,)\
  XX(eCmdLineArgRequiredNotSupplied, )\
  XX(eCmdLineArgNotAccepted, )

/**
 * ACmdLineParserResults - The resulting object returned by the Parser.
 * All options are stored in a vector<pair<CmdLineOption, string>>&, ie a
 * vector of pairs of CmdLineOption to the value of the option supplied.
 * All non options are returned as a vector of strings.
 */
class ACmdLineParseResults
{
public:

  DECLARE_ENUM( CmdLineParseResult, CLAP_RESULT_TYPES )

  virtual ~ACmdLineParseResults()
  {}

  /**
   * Returns the Option, value pair parsed out of the command line arguments supplied.
   * The options are in the order encountered in the commnad line arguments parsing
   * from left to right.
   *
   * @return vector<pair<CmdLineOption, string>>& A reference to the stored Option pairs.
   */
  virtual const std::vector<pair<CmdLineOption, string>>& Options() const = 0;
  /**
   * Returns all the non-option strings in the command line arguments.
   * @return vector<string>&
   */
  virtual const std::vector<string>& NonOptions() const = 0;
  /**
   * The result of the Parse Operation.
   * @return
   */
  virtual CmdLineParseResult Status() const = 0;

};

/**
 * ACmdLineParser - You can Register CmdLineOptions with the parser. When Parse() is invoked
 * the supplied command line options. The current default parser accepts short options if
 * prepended by '-' and long options if prepended by '--'
 */
class ACmdLineParser
{
public:
  virtual ~ACmdLineParser()
  {
  };

  virtual shared_ptr<ACmdLineParseResults> Parse( int argc, const char *const *pArgv ) = 0;

  virtual bool RegisterOption( CmdLineOption opt ) = 0;
  static unique_ptr<ACmdLineParser> CreateCmdLineParser();
};
}
#endif //__CMD_LINE_ARG_PARSER_H__ 

