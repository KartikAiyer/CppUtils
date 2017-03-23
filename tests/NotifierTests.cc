/** * Copyright (C) 2016 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 */

#include <gtest/gtest.h>
#include <ANotifier.h>
#include <thread>

using namespace NeoLibUsb;
using namespace std;

TEST( ASingleNotifierShould, RegisterANotificationCallback )
{
  ASingleNotifier<string, uint32_t> m_testObj;
  bool testVariable = false;
  auto token = m_testObj.RegisterNotification( 1, [ & ](string& val, shared_ptr<ACancelableToken> spToken ){
    testVariable = true;
  } );
  ASSERT_TRUE( token.use_count() > 0 );
}

TEST( ASingleNotifierShould, NotifyARegisteredNotification )
{
  ASingleNotifier<string, uint32_t> m_testObj;
  bool testVariable = false;
  auto token = m_testObj.RegisterNotification( 1, [ & ](string& val, shared_ptr<ACancelableToken> spToken ){
    testVariable = true;
  } );
  ASSERT_FALSE( testVariable );
  string str( "Random Test String" );
  m_testObj.Notify( 1, str );
  ASSERT_TRUE( testVariable );
}

TEST( ASingleNotifierShould, SubmitTheValueToTheNotificationCorrectly )
{
  ASingleNotifier<string,uint32_t> m_testObj;
  bool testVariable = false;
  string testString( "testString" );
  auto token = m_testObj.RegisterNotification( 1, [ & ]( string& val, shared_ptr<ACancelableToken> spToken  ) {
    if ( val == testString )
      testVariable = true;
    else
      testVariable = false;
  });
  m_testObj.Notify( 1, testString );
  ASSERT_TRUE( testVariable );
  string anotherString( "AnotherString" );
  m_testObj.Notify( 1, anotherString );
  ASSERT_FALSE( testVariable );
}

TEST( ASingleNotifierShould, CancelAsExpected )
{
  ASingleNotifier<string, uint32_t> m_testObj;
  bool testVariable = false;
  string someString( "SomeString" );
  auto token = m_testObj.RegisterNotification( 1, [ & ]( string& val, shared_ptr<ACancelableToken> spToken  ) {
    testVariable = true;
  });
  token.lock()->Cancel();
  m_testObj.Notify( 1, someString );
  ASSERT_FALSE( testVariable );
}

//Disabled since single notifier does not allow silent replacement
TEST( ASingleNotifierShould, DISABLED_RegisterOnlyOneNotificationReplacingOldOnes )
{
  ASingleNotifier<uint8_t, uint32_t> m_testObj;
  uint8_t testVariable = 0;
  auto token = m_testObj.RegisterNotification( 1, []( uint8_t& theVar, shared_ptr<ACancelableToken> spToken ){
  theVar = 2;
  });

  auto token2 = m_testObj.RegisterNotification( 1, []( uint8_t& theVar, shared_ptr<ACancelableToken> spToken ){
    theVar = 3;
  });

  m_testObj.Notify( 1, testVariable );
  ASSERT_EQ( 3, testVariable );
}

TEST( ASingleNotifierShould, RegisterOneNOtificationAndCancelWithAppropriateToken )
{
  //Token1 should not cancel the notification
  ASingleNotifier<uint8_t, uint32_t> m_testObj;
  uint8_t testVariable = 0;
  auto token = m_testObj.RegisterNotification( 1, []( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
    val = 2;
  });

  auto token2 = m_testObj.RegisterNotification( 1, []( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
    val = 3;
  });
  ASSERT_EQ( nullptr, token2.lock() );
  token.lock()->Cancel();
  token2 = m_testObj.RegisterNotification( 1, []( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
    val = 4;
  });
  m_testObj.Notify(1, testVariable );
  ASSERT_EQ( 4, testVariable );
}

TEST( ASingleNotifierShould, NotNotifyCallbacksOfOtherMsgTypes )
{
  ASingleNotifier<uint8_t,uint32_t> m_testObj;
  bool testVariable1 = false;
  bool testVariable2 = false;

  auto token = m_testObj.RegisterNotification( 1, [ &testVariable1 ]( uint8_t& ignoredVal, shared_ptr<ACancelableToken> spToken ) {
    testVariable1 = true;
  });

  auto token2 = m_testObj.RegisterNotification( 2, [ &testVariable2 ]( uint8_t& ignoredVal, shared_ptr<ACancelableToken> spToken ) {
    testVariable2 = true;
  });

  uint8_t someVal = 42;
  m_testObj.Notify( 1, someVal );
  ASSERT_FALSE( testVariable2 );
  ASSERT_TRUE( testVariable1 );
  testVariable1 = false;
  m_testObj.Notify( 2, someVal );
  ASSERT_TRUE( testVariable2 );
  ASSERT_FALSE( testVariable1 );
}

TEST( ASingleNotifierShould, BeCancellableFromWithinTheCallback )
{
  ASingleNotifier<uint8_t, uint32_t> m_testObj;

  weak_ptr<ACancelableToken> token1 = m_testObj.RegisterNotification( 1, [ &token1 ]( uint8_t& ignoredVal, shared_ptr<ACancelableToken> spToken ) {
    token1.lock()->Cancel();
    ignoredVal++;
  } );
  weak_ptr<ACancelableToken> token3 = m_testObj.RegisterNotification( 2, [ &token3 ]( uint8_t& ignoredVal, shared_ptr<ACancelableToken> spToken ) {
    ignoredVal++;
  });

  uint8_t someVal = 42;
  uint8_t copyOfVal = someVal;
  ASSERT_NO_THROW( m_testObj.Notify( 1, someVal ) );
  ASSERT_EQ( 43, someVal );
  ASSERT_NO_THROW( m_testObj.Notify( 1, someVal ) );
  ASSERT_EQ( 43, someVal );
  ASSERT_NO_THROW( m_testObj.Notify( 2, someVal ) );
  ASSERT_EQ( 44, someVal );
}

TEST( AMultiNotifierShould, RegisterMoreThanOneNotficationCallback )
{
  AMultiNotifier<string, uint32_t> m_testObj;
  bool testVariable = false;
  auto token = m_testObj.RegisterNotification( 1, [ &testVariable ](string& val, shared_ptr<ACancelableToken> spToken ){
    testVariable = true;
  } );
  ASSERT_TRUE( token.use_count() > 0 );
  auto token2 = m_testObj.RegisterNotification( 1, []( string& val, shared_ptr<ACancelableToken> spToken ){

  });
  ASSERT_TRUE( token2.use_count() > 0 );
}

TEST( AMultiNotifierShould, CallAllRegisteredNotifications )
{
  AMultiNotifier<uint8_t, uint32_t> m_testObj;
  uint8_t testVariable1 = 0;
  uint8_t testVariable2 = 0;

  auto token1 = m_testObj.RegisterNotification( 1, [ &testVariable1 ]( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
    testVariable1 = val;
  });
  auto token2 = m_testObj.RegisterNotification( 1, [ &testVariable2 ]( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
    testVariable2 = val;
  });

  uint8_t someValue = 42;
  m_testObj.Notify( 1, someValue );
  ASSERT_EQ( someValue, testVariable1 );
  ASSERT_EQ( someValue, testVariable2 );
}

TEST( AMultiNotifierShould, CancelOnlyTheRequestedToken )
{
  AMultiNotifier<uint8_t, uint32_t> m_testObj;
  uint8_t testVariable1 = 0;
  uint8_t testVariable2 = 0;

  auto token = m_testObj.RegisterNotification( 1, [ &testVariable1 ]( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
    testVariable1 = val;
  });
  auto token2 = m_testObj.RegisterNotification( 1, [ &testVariable2 ]( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
    testVariable2 = val;
  });

  uint8_t someValue = 42;
  token.lock()->Cancel();
  m_testObj.Notify( 1, someValue);

  ASSERT_EQ( 0, testVariable1 );
  ASSERT_EQ( someValue, testVariable2 );
}

TEST( AMultiNotifierShould, DISABLED_CancelRequestedTokensInMultiThreadSystem )
{
  AMultiNotifier<uint8_t, uint32_t> m_testObj;
  uint32_t registerIterations = 1<<20;
  std::thread  testThread1 =
    std::thread([ &m_testObj, registerIterations ](){
                      uint32_t registerCount = 0;
                      uint8_t testVariable1 = 0;
                      while ( ++registerCount < registerIterations ) {
                        uint8_t someVal = (uint8_t)registerCount;
                        auto token = m_testObj.RegisterNotification( 1, [ &testVariable1 ]( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
                          testVariable1 = val;
                        });
                        // notify on the other thread's handler
                        m_testObj.Notify( 2, someVal);
                        token.lock()->Cancel();
                        if ( registerCount % (1<<17) == 0 ) {
                          cout<<"Thread1 register count "<<registerCount<<endl;
                        }
                      }
                    } );
  std::thread  testThread2 =
    std::thread([ &m_testObj, registerIterations ](){
                      uint32_t registerCount = 0;
                      uint8_t testVariable2 = 0;
                      while ( ++registerCount < registerIterations ) {
                        uint8_t someVal = (uint8_t)registerCount;
                        auto token2 = m_testObj.RegisterNotification( 2, [ &testVariable2 ]( uint8_t& val, shared_ptr<ACancelableToken> spToken ) {
                          testVariable2 = val;
                        });
                        // notify on the other thread's handler
                        m_testObj.Notify( 1, someVal);
                        token2.lock()->Cancel();
                        if ( registerCount % (1<<17) == 0 ) {
                          cout<<"Thread2 register count "<<registerCount<<endl;
                        }
                      }
                    } );

  testThread1.join();
  testThread2.join();

  //ASSERT_EQ( 0, testVariable1 );
  //ASSERT_EQ( someValue, testVariable2 );
}

TEST( AMultiNotifierShould, BeCancellableFromWithinTheCallback )
{
  AMultiNotifier<uint8_t, uint32_t> m_testObj;
  uint32_t count = 250;
  uint8_t testVarA = 0, testVarB = 0;
  std::list<weak_ptr<ACancelableToken>> listA;
  std::list<weak_ptr<ACancelableToken>> listB;

  for ( uint32_t i = 0; i < count / 2 ; i++ ) {
    listA.push_back( m_testObj.RegisterNotification( 1, [ i, &listA ]( uint8_t& valueA, shared_ptr<ACancelableToken> spTok ) {
      valueA++;
      auto token = listA.back();
      ASSERT_NO_FATAL_FAILURE( listA.pop_back() );
      auto spToken = token.lock();
      ASSERT_NO_FATAL_FAILURE( spToken->Cancel() );
    } ) );
    listB.push_back( m_testObj.RegisterNotification( 2, [ i, &listB ]( uint8_t& valueB, shared_ptr<ACancelableToken> spTok ) {
      valueB++;
      auto token = listB.back();
      ASSERT_NO_FATAL_FAILURE( listB.pop_back() );
      auto spToken = token.lock();
      ASSERT_NO_FATAL_FAILURE( spToken->Cancel() );
    } ) );
  }
  std::thread testThreadA =
    std::thread([&m_testObj, count, &testVarA]() {
      uint32_t c = count;
      cout<<c<<endl;
      while( c-- ) {
        ASSERT_NO_FATAL_FAILURE( m_testObj.Notify( 1, testVarA ) );
      }
    });
  std::thread testThreadB =
    std::thread( [&m_testObj, count, &testVarB]() {
      uint32_t c = count;
      cout<<c<<endl;
      while( c-- ) {
        ASSERT_NO_FATAL_FAILURE( m_testObj.Notify( 2, testVarB ) );
      }
    } );

  testThreadA.join();
  testThreadB.join();

  ASSERT_EQ( count/2, testVarA );
  ASSERT_EQ( count/2, testVarB );
}
