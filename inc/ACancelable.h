/** * Copyright (C) 2016 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 */
#ifndef __ACANCELLABLE_H__
#define __ACANCELLABLE_H__

#include <memory>

namespace NeoLibUsb {

class ACancelableToken;

class ACancelable
{
public:
  virtual ~ACancelable()
  { }

  virtual void CancelWith( std::shared_ptr<ACancelableToken> spToken ) const = 0;
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
  ACancelableTokenImpl( const ACancelable& cancelable ) : m_cancelable{ cancelable }
  { }

  virtual ~ACancelableTokenImpl()
  { }

  virtual void Cancel()
  {
    m_cancelable.CancelWith( shared_from_this() );
  }

protected:
  const ACancelable& m_cancelable;
};


template<typename U>
class ATypedCancelableToken : public ACancelableTokenImpl
{
public:
  ATypedCancelableToken( const ACancelable& cancellable,
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