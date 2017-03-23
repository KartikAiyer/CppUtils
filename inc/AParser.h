/** * Copyright (C) 2016 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 */
#ifndef __APARSER_H__
#define __APARSER_H__

#include <stdint.h>
#include <PancamMessages.pb.h>

namespace NeoLibUsb {

template <typename T>
class AParser
{
public:
  virtual ~AParser() {}

  virtual bool ParseFromBytes( T& out, const void* pData, int size ) = 0;
};

}

#endif // __AMESSAGE_PARSER_H__
