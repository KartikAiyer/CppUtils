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
#ifndef __DISPATH_THREAD_H__
#define __DISPATH_THREAD_H__

#include <thread>
#include <TsQueue.h>

namespace CppUtils
{

using namespace std;

class DispatchThread
{
public:
  DispatchThread()
  {
    // thread started in constructor member initialization
    // list appears to reference uninitailized variables
    // so wait until constructor body to start it up
    m_spThread = make_shared<thread>( [this]() {
      while( m_keepRunning ) {
        auto fn = m_tsQueue.DeQueue();
        fn();
      }
    });
  }
  virtual ~DispatchThread()
  {
    Kill();
  }
  void Kill()
  {
    if ( m_spThread ) {
      PostToDispatch( [this]() { m_keepRunning = false; } );
      if( this_thread::get_id() != m_spThread->get_id() ) {
        m_spThread->join();
        m_spThread.reset();
      }
    }
  }

  void PostToDispatch( std::function<void(void)> fn )
  {
    if( fn ) {
      m_tsQueue.EnQueue( fn );
    }
  }
private:
  shared_ptr<thread> m_spThread;
  TsQueue<std::function<void(void)>> m_tsQueue;
  bool m_keepRunning = true;
};

}

#endif // __DISPATH_THREAD_H__
