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
#ifndef __TS_QUEUE_H__
#define __TS_QUEUE_H__

#include <condition_variable>
#include <queue>

using namespace std;

namespace CppUtils {
  
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