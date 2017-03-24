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

#include <gtest/gtest.h>
#include <CmdLineArgParser.h>

using namespace CppUtils;
using namespace std;


TEST( CmdLineParserShould, CreateACmdLineParser )
{
  auto upCmdLineParser = ACmdLineParser::CreateCmdLineParser();
  ASSERT_TRUE( upCmdLineParser != nullptr );
}

TEST( CmdLineParserShould, AcceptOptionsWithNoArgs )
{
  auto upCmdLineParser = ACmdLineParser::CreateCmdLineParser();
  if( upCmdLineParser ) {
    ASSERT_TRUE( upCmdLineParser->RegisterOption( CmdLineOption( "s", "short", CmdLineOption::eArgNone ) ) );
    ASSERT_TRUE( upCmdLineParser->RegisterOption( CmdLineOption( "l", "long", CmdLineOption::eArgNone ) ) );

    const char* pArgv[] = { "cmd", "-s", "--short", "-l", "--long", "nonoption" };
    int argc = sizeof( pArgv ) / sizeof( pArgv[ 0 ] );
    auto result = upCmdLineParser->Parse( argc, pArgv );

    ASSERT_EQ( ACmdLineParseResults::eCmdLineArgParseSuccess, result->Status() );
    ASSERT_EQ( string( "s" ), result->Options()[0].first.m_shortName );
    ASSERT_EQ( string( "short" ), result->Options()[1].first.m_longName );
    ASSERT_EQ( string( "l" ), result->Options()[2].first.m_shortName );
    ASSERT_EQ( string( "long" ), result->Options()[3].first.m_longName );
    ASSERT_EQ( string( "nonoption" ), result->NonOptions()[0] );
  }
}

TEST( CmdLineParserShould, AcceptOptionsWithArgs )
{
  auto upCmdLineParser = ACmdLineParser::CreateCmdLineParser();
  if( upCmdLineParser ){
    ASSERT_TRUE( upCmdLineParser->RegisterOption( CmdLineOption( "s", "short", CmdLineOption::eArgRequired ) ) );
    ASSERT_TRUE( upCmdLineParser->RegisterOption( CmdLineOption( "l", "long", CmdLineOption::eArgOptional ) ) );

    const char* pArgv[] = { "cmd", "-s", "requiredArg", "-l", "optionalArg", "--long" };
    int argc = sizeof( pArgv ) / sizeof( pArgv[ 0 ] );

    auto result = upCmdLineParser->Parse( argc, pArgv );

    ASSERT_EQ( ACmdLineParseResults::eCmdLineArgParseSuccess, result->Status() );

    ASSERT_EQ( string( "s" ), result->Options()[0].first.m_shortName );
    ASSERT_EQ( string( "requiredArg" ), result->Options()[0].second );
    ASSERT_EQ( string( "l" ), result->Options()[1].first.m_shortName );
    ASSERT_EQ( string( "optionalArg" ), result->Options()[1].second );
    ASSERT_EQ( string( "long" ), result->Options()[2].first.m_longName );
    ASSERT_EQ( string(), result->Options()[2].second );

    auto nonOpt = result->NonOptions();
    for( auto it : nonOpt ) {
      cout << "Non Option: " << it << endl;
    }
    ASSERT_EQ( 0, result->NonOptions().size() );
  }
}

TEST( CmdLineParserShould, RejectIfRequiredArgNotProvided )
{
  auto upCmdLineParser = ACmdLineParser::CreateCmdLineParser();
  if( upCmdLineParser ) {
    ASSERT_TRUE( upCmdLineParser->RegisterOption( CmdLineOption( "s", "short", CmdLineOption::eArgRequired ) ) );
    ASSERT_TRUE( upCmdLineParser->RegisterOption( CmdLineOption( "l", "long", CmdLineOption::eArgRequired ) ) );

    const char* pArgv1[] = { "cmd", "-s", "requiredArg", "-l" };
    int argc1 = sizeof( pArgv1 ) / sizeof( pArgv1[ 0 ] );

    const char* pArgv2[] = { "cmd", "-s", "-l" };
    int argc2 = sizeof( pArgv2 ) / sizeof( pArgv2[ 0 ] );

    auto result = upCmdLineParser->Parse( argc1, pArgv1 );
    ASSERT_EQ( ACmdLineParseResults::eCmdLineArgRequiredNotSupplied, result->Status() );

    result = upCmdLineParser->Parse( argc2, pArgv2 );
    ASSERT_EQ( ACmdLineParseResults::eCmdLineArgRequiredNotSupplied, result->Status() );
  }
}

TEST( CmdLineParserShould, AcceptWithCallbacks )
{
  auto upCmdLineParser = ACmdLineParser::CreateCmdLineParser();
  if( upCmdLineParser ) {
    auto opt1 = CmdLineOption( "s", "short", CmdLineOption::eArgRequired,
                               [ ] ( CmdLineOption& opt, string val ) {
                                 return CmdLineOption::eCmdParseSuccess;
                               } );
    auto opt2 = CmdLineOption( "l", "long", CmdLineOption::eArgNone );

    ASSERT_TRUE( upCmdLineParser->RegisterOption( opt1 ) );
    ASSERT_TRUE( upCmdLineParser->RegisterOption( opt2 ) );

    const char* pArgv[] = { "cmd", "-s", "someShit", "-l", "nonoption" };
    int argc = sizeof( pArgv ) / sizeof( pArgv[ 0 ] );

    auto result = upCmdLineParser->Parse( argc, pArgv );
    ASSERT_EQ( ACmdLineParseResults::eCmdLineArgParseSuccess, result->Status() );
    ASSERT_EQ( string( "someShit" ), result->Options()[0].second );
    ASSERT_EQ( string(), result->Options()[1].second );
    ASSERT_EQ( 1, result->NonOptions().size() );
    ASSERT_EQ( string("nonoption"), result->NonOptions()[0] );
  }
}

TEST( CmdLineParserShould, AllowNonOptionsMixedWithOptions ) {
  auto upCmdLineParser = ACmdLineParser::CreateCmdLineParser();
  if( upCmdLineParser ) {
    auto opt1 = CmdLineOption( "s", "short", CmdLineOption::eArgRequired );
    auto opt2 = CmdLineOption( "l", "long", CmdLineOption::eArgRequired );

    ASSERT_TRUE( upCmdLineParser->RegisterOption( opt1 ) );
    ASSERT_TRUE( upCmdLineParser->RegisterOption( opt2 ) );

    const char* pArgv[] = { "cmd", "-s", "Duh", "NonOption", "-l", "crap" };
    int argc = sizeof( pArgv ) / sizeof( pArgv[ 0 ] );

    auto result = upCmdLineParser->Parse( argc, pArgv );
    ASSERT_EQ( ACmdLineParseResults::eCmdLineArgParseSuccess, result->Status() );
    ASSERT_EQ( 1, result->NonOptions().size() );
    ASSERT_EQ( string( "Duh" ), result->Options()[0].second );
    ASSERT_EQ( string( "NonOption" ), result->NonOptions()[0] );
    ASSERT_EQ( string( "crap" ), result->Options()[1].second );
  }
}

