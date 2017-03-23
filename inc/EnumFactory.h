/** * Copyright (C) 2016 Jaunt Inc. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or
 * otherwise, without the prior consent of Jaunt Inc.
 *
 * Credits go to the Stack Overflow answer found here
 * http://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c#202511
 */

//expansion macro for enum value definition
#define ENUM_VALUE( name, assign ) name assign,

//expansion macro for enum to string conversion
#define ENUM_CASE( name, assign ) case name: return #name;

#define ENUM_STRCMP( name, assign ) if( !strcmp( str, #name ) ) return name;

//declare the access funtion and enum values
#define DECLARE_ENUM( EnumType, ENUM_DEF ) \
  enum EnumType { \
    ENUM_DEF( ENUM_VALUE ) \
  };\
  const char* GetString( EnumType blah )\
  {\
     switch( blah ) {\
       ENUM_DEF(ENUM_CASE)\
       default: return "";\
     }\
  }\
  EnumType Get##EnumType##Value( const char* str )\
  {\
     ENUM_DEF(ENUM_STRCMP)\
     return (EnumType)0;\
  }

