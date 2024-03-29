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
#ifndef __ANOTIFIER_H__
#define __ANOTIFIER_H__

#include <ACancelable.h>
#include <memory>
#include <map>
#include <list>
#include <mutex>
#include <functional>

namespace CppUtils {

template<typename T, typename U>
class ANotifier : public ACancelable
{
public:
  using NotificationFn = std::function<void( T&, std::shared_ptr<ACancelableToken> )>;
  virtual std::weak_ptr<ACancelableToken> RegisterNotification( U msgType, NotificationFn fn ) = 0;
  virtual void Notify( U msgType, T& value ) = 0;
};

template<typename T, typename U>
class ASingleNotifier : public ANotifier<T, U>
{
public:
  using NotificationFn = typename ANotifier<T,U>::NotificationFn;
  virtual ~ASingleNotifier()
  { }

  virtual std::weak_ptr<ACancelableToken> RegisterNotification( U msgType, NotificationFn fn )
  {
    std::weak_ptr<ACancelableToken> retval;
    std::unique_lock <std::mutex> lk( m_mtx );
    std::shared_ptr <ATypedCancelableToken<U>> tmp( std::make_shared<ATypedCancelableToken<U>>( *this, msgType ));
    if( m_notifierMap.find( msgType ) == m_notifierMap.end() ) {
      m_notifierMap[ msgType ] = std::make_pair( fn, tmp );
      retval = tmp;
    }
    return retval;
  }

  virtual void CancelWith( std::shared_ptr<ACancelableToken> spBaseToken )
  {
    try {
      auto spToken = std::dynamic_pointer_cast<ATypedCancelableToken<U>>( spBaseToken );
      std::unique_lock<std::mutex> lk( this->m_mtx );
      m_notifierMap.erase( spToken->m_msgType );
    }
    catch ( std::bad_cast exp ) { }
  }

  virtual void Notify( U msgType, T& value )
  {
    m_mtx.lock();
    auto it = m_notifierMap.find( msgType );
    if ( it != m_notifierMap.end() && it->second.first ) {
      auto fn = it->second.first;
      m_mtx.unlock();
      fn( value, it->second.second );
    }else {
      m_mtx.unlock();
    }
  }

protected:
  mutable std::map <U, std::pair<NotificationFn, std::shared_ptr<ATypedCancelableToken<U>>>> m_notifierMap;
  mutable std::mutex m_mtx;
};

template<typename T, typename U>
class AMultiNotifier : public ANotifier<T,U>
{
protected:
  using NotificationFn = typename ANotifier<T,U>::NotificationFn;
  class AmnCancellableToken : public ATypedCancelableToken<U>
  {
  public:
    AmnCancellableToken( ACancelable& cancellable,
                         U msgType,
                         typename std::list<std::pair<NotificationFn,std::shared_ptr<AmnCancellableToken>>>::iterator it ) :
        ATypedCancelableToken<U>{ cancellable, msgType }, m_it{ it } { }
    ~AmnCancellableToken() {}
    const typename std::list<std::pair<NotificationFn,std::shared_ptr<AmnCancellableToken>>>::iterator m_it;
  };

public:
  ~AMultiNotifier() { }
  virtual std::weak_ptr<ACancelableToken> RegisterNotification( U msgType, NotificationFn fn )
  {
    std::weak_ptr<ACancelableToken> retval;
    std::unique_lock <std::mutex> lk( m_mtx );
    std::shared_ptr<AmnCancellableToken> token;

    m_notifierMap[ msgType ].push_back( std::make_pair( fn, token) );
    auto it = m_notifierMap[msgType].end();
    token = std::make_shared<AmnCancellableToken>( *this, msgType, --it  );

    m_notifierMap[ msgType ].back().second = token;
    retval = token;
    return retval;
  }

  virtual void CancelWith( std::shared_ptr<ACancelableToken> spBaseToken )
  {
    try {
      auto spToken = std::dynamic_pointer_cast<AmnCancellableToken>( spBaseToken );
      std::unique_lock<std::mutex> lk( this->m_mtx );
      if ( m_notifierMap.find( spToken->m_msgType ) != m_notifierMap.end() &&
          !m_notifierMap[ spToken->m_msgType ].empty()) { //If the list of handler is not empty
        m_notifierMap[ spToken->m_msgType ].erase( spToken->m_it ); //Delete the handler in list
        if ( m_notifierMap[ spToken->m_msgType ].empty()) //If the list is now empty
          m_notifierMap.erase( spToken->m_msgType ); // Delete the msgType Key element
      }
    } catch ( std::bad_cast exp ) { }
  }

  virtual void Notify( U msgType, T& value )
  {
    m_mtx.lock();
    auto anotherCopy = m_notifierMap;
    m_mtx.unlock();
    typename std::map<U, std::list<std::pair<NotificationFn, std::shared_ptr<AmnCancellableToken>>>>::iterator ait =
        anotherCopy.find( msgType );

    if( ait != anotherCopy.end() &&
        !ait->second.empty() ) {
      for( auto ait2 = ait->second.begin(); ait2 != ait->second.end(); ait2++ )
        if( ait2->first )
          ait2->first( value, ait2->second );
    }
  }

protected:
  mutable std::map <U, std::list<std::pair<NotificationFn,std::shared_ptr<AmnCancellableToken>>>> m_notifierMap;
  mutable std::mutex m_mtx;
};
}

#endif // __ANOTIFIER_H__
