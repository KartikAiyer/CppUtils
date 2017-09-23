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
#ifndef __ACANCELLABLE_H__
#define __ACANCELLABLE_H__

#include <memory>

namespace CppUtils {

class ACancelableToken;

class ACancelable
{
public:
  virtual ~ACancelable()
  { }

  virtual void CancelWith( std::shared_ptr<ACancelableToken> spToken ) = 0;
};

class ACancelableToken
{
public:
  virtual ~ACancelableToken() {}
  virtual void Cancel() = 0;
};

class ACancelableTokenImpl : public ACancelableToken, public std::enable_shared_from_this<ACancelableTokenImpl>
{
public:
  ACancelableTokenImpl( ACancelable& cancelable ) : m_cancelable{ cancelable }
  { }

  virtual ~ACancelableTokenImpl()
  { }

  virtual void Cancel()
  {
    m_cancelable.CancelWith( shared_from_this() );
  }

protected:
  ACancelable& m_cancelable;
};


template<typename U>
class ATypedCancelableToken : public ACancelableTokenImpl
{
public:
  ATypedCancelableToken( ACancelable& cancellable,
                          U value ) :
      ACancelableTokenImpl{ cancellable }, m_msgType{ value }
  { }

  virtual ~ATypedCancelableToken()
  { }

public:
  U m_msgType;
};

#define LOCK_AND_CANCEL( tok )\
do{\
auto sp = (tok).lock();\
if( sp ) sp->Cancel();\
}while( 0 )

#if 0
class WeakPtrWrappedCancelableToken : public ACancelableToken
{
public:
  WeakPtrWrappedCancelableToken() { }
  WeakPtrWrappedCancelableToken( std::shared_ptr<ACancelableToken> spToken ) :
      m_wpToken{ spToken }
  {}
  ~WeakPtrWrappedCancelableToken(){}
  virtual void Cancel() {
    auto sp = m_wpToken.lock();
    if( sp ) sp->Cancel();
  }
private:
  std::weak_ptr<ACancelableToken> m_wpToken;
};
#endif

}
#endif // __ACANCELLABLE_H__