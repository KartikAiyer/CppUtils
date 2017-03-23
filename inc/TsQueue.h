/**
 * Copyright (C) 2015 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 */
#ifndef __TS_QUEUE_H__
#define __TS_QUEUE_H__

#include <condition_variable>
#include <queue>

using namespace std;

namespace NeoLibUsb {
  
template<typename T>
class TsQueue
{
private:
  condition_variable m_cond;
  mutex m_mtx;
  queue<T> m_q;
  
public:
  void EnQueue( T t )
  {
    unique_lock<mutex> lk(m_mtx);
    m_q.push( t );
    lk.unlock();
    m_cond.notify_one();
  }
  
  T DeQueue()
  {
    unique_lock<mutex> lk( m_mtx );
    if( m_q.empty() ){
      m_cond.wait( lk, [=](){ return !m_q.empty(); } );
    }
    T t = m_q.front();
    m_q.pop();
    return t;
  }
};
  
}

#endif //__TS_QUEUE_H__