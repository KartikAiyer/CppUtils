/** * Copyright (C) 2015 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 */
#ifndef __DISPATH_THREAD_H__
#define __DISPATH_THREAD_H__

#include <thread>
#include <TsQueue.h>
#include "NeoLibDebug.h"

namespace NeoLibUsb
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
    DEBUG_PRINTLN( "Exited" );
  }
  void Kill()
  {
    if ( m_spThread ) {
      PostToDispatch( [this]() { m_keepRunning = false; } );
      m_spThread->join();
      m_spThread.reset();
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
