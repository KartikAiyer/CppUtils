/** * Copyright (C) 2016 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 */

#include <gtest/gtest.h>
#include <DispatchThread.h>
#include <future>

using namespace std;
using namespace NeoLibUsb;

TEST( DispatchThreadShould, RunATask )
{
  DispatchThread thr;
  promise<bool> success;
  auto fut = success.get_future();
  thr.PostToDispatch( [&success]() { success.set_value(true); } );
  if( fut.wait_for(chrono::milliseconds(500)) != future_status::ready ) {
    FAIL();
  } else {
    ASSERT_TRUE( fut.get() );
  }
}

TEST( DispatchThreadShould, QueueUpTasks )
{
  DispatchThread thr;
  promise<bool> success;
  auto fut = success.get_future();
  uint32_t termination = 100;
  for( uint32_t i = 0; i <= termination; i++ ){
    thr.PostToDispatch( [&success,termination, i]() {
      if( i == termination ){
        success.set_value(true);
      }
    });
  }
  if( fut.wait_for( chrono::milliseconds(500)) != future_status::ready ) {
    FAIL();
  } else {
    ASSERT_TRUE( fut.get() );
  }
}

//Doesn't work properly
TEST( DispatchThreadShould, DISABLED_ExitProperly )
{
  DispatchThread* pThr;
  promise<bool> success;
  auto fut = success.get_future();
  function<void(void)> fn( [pThr,&fn]() {
    this_thread::sleep_for(chrono::milliseconds(1));
    pThr->PostToDispatch( fn ); } );
  delete pThr;
}
