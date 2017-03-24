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
#ifndef __ASHUTDOWNABLE_H__
#define __ASHUTDOWNABLE_H__

#include <future>

namespace CppUtils {

enum EShutdownStatus {
  eSHUTDOWN_STATUS_NULL = 0,
  eSHUTDOWN_STATUS_SUCCESS = 1,
  eSHUTDOWN_STATUS_ERROR = 2,
};

using ShutdownFuture = std::future<EShutdownStatus>;

class AShutdownable
{
public:
  virtual ~AShutdownable()
  { }

  virtual ShutdownFuture Shutdown() = 0;
};


class AShutdownableImpl : public AShutdownable
{
public:
  virtual ~AShutdownableImpl() { }

protected:
  ShutdownFuture GetShutdownFuture() { return m_shutdownPromise.get_future(); }
  void SetShutdownStatus( EShutdownStatus status ) { m_shutdownPromise.set_value( status ); }

private:
  std::promise<EShutdownStatus> m_shutdownPromise;
};

}


#endif // __ASHUTDOWNABLE_H__
