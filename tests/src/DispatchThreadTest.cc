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
#include <DispatchThread.h>
#include <future>

using namespace std;
using namespace CppUtils;

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
