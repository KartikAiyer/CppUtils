/** * Copyright (C) 2016 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 */
#ifndef __ASHUTDOWNABLE_H__
#define __ASHUTDOWNABLE_H__

#include <future>

namespace NeoLibUsb {

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
