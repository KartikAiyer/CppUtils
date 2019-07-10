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
 *
 * Credits go to the Stack Overflow answer found here
 * http://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c#202511
 */

#include <string>
#include <cstring>
//expansion macro for enum value definition
#define ENUM_VALUE( name, assign ) name assign,

//expansion macro for enum to string conversion
#define ENUM_CASE( name, assign ) case name: return #name;
#define ENUM_VAL_CASE( name, assign ) case name: return name;
#define ENUM_STRCMP( name, assign ) if( !strcmp( str, #name ) ) return name;

#define DECLARE_ENUM_NONMEMBER( EnumType, ENUM_DEF ) \
  enum EnumType { \
    ENUM_DEF( ENUM_VALUE ) \
  };\
  static inline const char* GetString( const EnumType blah )\
  {\
     switch( blah ) {\
       ENUM_DEF(ENUM_CASE)\
       default: return "";\
     }\
  }\
  static inline EnumType Get##EnumType##Value( const char* str )\
  {\
     ENUM_DEF(ENUM_STRCMP)\
     return (EnumType)0;\
  }\
  static inline EnumType Get##EnumType##Value( const uint32_t value )\
  {\
    switch( value ) {\
    ENUM_DEF(ENUM_VAL_CASE)\
    default: return (EnumType)0;\
    }\
  }

//declare the access funtion and enum values
#define DECLARE_ENUM( EnumType, ENUM_DEF ) \
  enum EnumType { \
    ENUM_DEF( ENUM_VALUE ) \
  };\
  inline const char* GetString( const EnumType blah ) const\
  {\
     switch( blah ) {\
       ENUM_DEF(ENUM_CASE)\
       default: return "";\
     }\
  }\
  inline EnumType Get##EnumType##Value( const char* str ) const\
  {\
     ENUM_DEF(ENUM_STRCMP)\
     return (EnumType)0;\
  }

