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
#include <CmdLineArgParser.h>
#include <vector>
#include <map>

using namespace std;
using namespace CppUtils;

class CmdLineParser;

class CmdLineParser : public ACmdLineParser
{
public:
  class CmdLineParseResults : public ACmdLineParseResults
  {
  public:
    ~CmdLineParseResults()
    {
    }

    const vector<pair<CmdLineOption, string>> &Options() const
    { return m_options; }

    const vector<string> &NonOptions() const
    { return m_nonOptions; }

    void AddOptionValuePair( CmdLineOption option, string value )
    {
      m_options.push_back( make_pair( option, value ));
    }

    void AddNonOption( string option )
    {
      m_nonOptions.push_back( option );
    }

    const CmdLineParseResult Status() const
    { return status; }

    friend class CmdLineParser;

  private:
    vector<pair<CmdLineOption, string>> m_options;
    vector<string> m_nonOptions;
    CmdLineParseResult status;
    string errString;

  };

public:
  CmdLineParser( string shortOptPrefix = "-", string longOptPrefix = "--" ) :
      m_shortOptPrefix{ shortOptPrefix }, m_longOptPrefix{ longOptPrefix }
  {
  }

  virtual ~CmdLineParser()
  {
  }

  bool RegisterOption( CmdLineOption opt )
  {
    bool retval = false;
    if( opt.m_shortName.size() > 0 || opt.m_longName.size() > 0 ) {
      m_options.push_back( opt );
      retval = true;
    }
    return retval;
  }

  shared_ptr<ACmdLineParseResults> Parse( int argc, const char *const *pArgv )
  {
    shared_ptr<CmdLineParseResults> retval;
    retval.reset( new CmdLineParseResults());
    bool continueParsing = true;
    for( auto i = 1; i < argc && continueParsing; i++ ) {
      bool isLong = IsLongOption( pArgv[ i ] );
      if( isLong || IsShortOption( pArgv[ i ] )) {
        CmdLineOption option;
        if( GetOptionFor( StripOption( pArgv[ i ] ), isLong, option )) {
          string nextArg;
          CmdLineOption::CbParseResults status;
          switch( option.m_type ) {
            case CmdLineOption::eArgRequired:
              if(( i + 1 ) < argc ) {
                nextArg = pArgv[ i + 1 ];
                if( !IsLongOption( nextArg.c_str()) && !IsShortOption( nextArg.c_str())) {
                  if( option.m_fnMatch ) {
                    status = option.m_fnMatch( option, nextArg );
                    if( status == CmdLineOption::eCmdParseArgumentNotAccepted ) {
                      retval->errString = "Unsupplied or bad argument for " + option.m_longName;
                      retval->status = ACmdLineParseResults::eCmdLineArgNotAccepted;
                      continueParsing = false;
                    } else {
                      retval->status = ACmdLineParseResults::eCmdLineArgParseSuccess;
                      retval->AddOptionValuePair( option, nextArg );
                      i++;
                    }
                  } else {
                    retval->status = ACmdLineParseResults::eCmdLineArgParseSuccess;
                    retval->AddOptionValuePair( option, nextArg );
                    i++;
                  }
                } else {
                  //Got an Option instead of an argument
                  retval->errString = "Unsupplied Argument";
                  retval->status = ACmdLineParseResults::eCmdLineArgRequiredNotSupplied;
                  continueParsing = false;
                }
              } else {
                retval->errString = "Unsupplied Argument";
                retval->status = ACmdLineParseResults::eCmdLineArgRequiredNotSupplied;
                continueParsing = false;
              }
              break;
            case CmdLineOption::eArgOptional:
              if(( i + 1 ) < argc ) {
                nextArg = pArgv[ i + 1 ];
              } else {
                nextArg.clear();
              }
              if( !IsLongOption( nextArg.c_str()) && !IsShortOption( nextArg.c_str())) {
                i++;
              } else {
                nextArg.clear();
              }
              if( option.m_fnMatch ) {
                status = option.m_fnMatch( option, nextArg );
                if( status == CmdLineOption::eCmdParseArgumentNotAccepted ) {
                  retval->errString = "Invalid Argument " + nextArg;
                  retval->status = ACmdLineParseResults::eCmdLineArgNotAccepted;
                  continueParsing = false;
                } else {
                  //No Argument taken
                  retval->status = ACmdLineParseResults::eCmdLineArgParseSuccess;
                  retval->AddOptionValuePair( option, nextArg );
                  retval->errString.clear();
                }
              } else {
                retval->AddOptionValuePair( option, nextArg );
                retval->status = ACmdLineParseResults::eCmdLineArgParseSuccess;
                retval->errString.clear();
              }
              break;
            case CmdLineOption::eArgNone:
              nextArg.clear();
              if( option.m_fnMatch ) {
                if( option.m_fnMatch( option, nextArg ) == CmdLineOption::eCmdParseArgumentNotAccepted ) {
                  retval->errString = "Bad Option Parse";
                  retval->status = ACmdLineParseResults::eCmdLineArgInvalidOptionFound;
                  continueParsing = false;
                }
              }
              if( continueParsing ) {
                retval->status = ACmdLineParseResults::eCmdLineArgParseSuccess;
                retval->AddOptionValuePair( option, string());
              }
              break;
          }
        } else {
          //Invalid Option
          retval->status = ACmdLineParseResults::eCmdLineArgInvalidOptionFound;
          retval->errString = string( pArgv[ i ] ) + " is not a valid option";
          continueParsing = false;
        }
      } else {
        // non option
        retval->AddNonOption( string( pArgv[ i ] ));
        retval->status = ACmdLineParseResults::eCmdLineArgParseSuccess;
        retval->errString.clear();
      }
    }
    return dynamic_pointer_cast<ACmdLineParseResults>( retval );
  }

private:
  bool GetOptionFor( string optString, bool isLong, CmdLineOption &option )
  {
    bool retval = false;
    for( auto &opt : m_options ) {
      string comparisonName = ( isLong ) ? opt.m_longName : opt.m_shortName;
      if( optString == comparisonName ) {
        retval = true;
        option = opt;
        break;
      }
    }
    return retval;
  }

  bool IsOption( const char *pPrefix, const char *pStr ) const
  {
    auto retval = false;
    auto optLength = strlen( pPrefix );
    if( strlen( pStr ) > optLength && !strncmp( pPrefix, pStr, optLength )) {
      retval = true;
    }
    return retval;
  }

  bool IsShortOption( const char *pStr ) const
  {
    return IsOption( m_shortOptPrefix.c_str(), pStr );
  }

  bool IsLongOption( const char *pStr ) const
  {
    return IsOption( m_longOptPrefix.c_str(), pStr );
  }

  string StripOption( const char *pStr ) const
  {
    string retval;
    size_t pos = string::npos;
    size_t sizeOfPrefix = 0;
    if( IsLongOption( pStr )) {
      pos = string( pStr ).find( m_longOptPrefix );
      sizeOfPrefix = m_longOptPrefix.length();
    } else if( IsShortOption( pStr )) {
      pos = string( pStr ).find( m_shortOptPrefix );
      sizeOfPrefix = m_shortOptPrefix.length();
    }

    if( pos != string::npos ) {
      retval = string( pStr ).substr( pos + sizeOfPrefix );
    }
    return retval;
  }

private:
  vector<CmdLineOption> m_options;
  string m_shortOptPrefix;
  string m_longOptPrefix;
};

unique_ptr<ACmdLineParser> ACmdLineParser::CreateCmdLineParser()
{
  return unique_ptr<ACmdLineParser>( new CmdLineParser());
}
