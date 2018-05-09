#include <Property.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace CppUtils;
using namespace std;

TEST( PopertyShould, WorkOnIntAssign ) {
  Property<int> i;
  i.SubscribeChanged( [] ( ETrackedOps ops, int newVal, unique_ptr<int> pOldVal )->void {
    if( ops == eOpAssign ) {
      ASSERT_TRUE( pOldVal );
      ASSERT_EQ( *pOldVal, 0 );
      ASSERT_EQ( newVal, 1 );
  }
  });

  i = 1;

  //Add a new Cb
  i.SubscribeChanged( []( ETrackedOps ops, int newVal, unique_ptr<int> pOldVal ) -> void {
    ASSERT_TRUE( ( ops == eFunCopyConstructor || ops == eFunDestructor ) );
  } );

  Property<int> j = i;

  j.SubscribeChanged( [] ( ETrackedOps ops, int newVal, unique_ptr<int> pOldVal ) -> void {
    if( ops == eOpAssign ) {
      ASSERT_TRUE( pOldVal );
      ASSERT_EQ( *pOldVal, 1 );
      ASSERT_EQ( newVal, 3 );
    }
  } );
  j = 3;
}

TEST( PopertyShould, WorkOnClassForAssign )
{
  struct TestClass {
      TestClass( int i = 0, int j = 0 ) : x{ i }, y{ j } { }
      TestClass( const TestClass &other ) : x{ other.x }, y { other.y } { };
      int x, y;
      bool operator==( const TestClass& other ) const { return ( x == other.x && y == other.y ); }
  };

  Property<TestClass> p1;

  p1.SubscribeChanged( [] ( ETrackedOps ops, TestClass newVal, std::unique_ptr<TestClass> pOldVal ) {
    if( ops != eFunDestructor ) {
      ASSERT_EQ( ops, eOpAssign );
      ASSERT_TRUE( pOldVal && (( *pOldVal ) == TestClass()));
      ASSERT_TRUE( newVal == TestClass( 1, 2 ));
    }
  } );

  p1 = TestClass{ 1, 2 };
  p1.SubscribeChanged( []( ETrackedOps ops, TestClass newVal, std::unique_ptr<TestClass> pOldVal ) {
    ASSERT_TRUE( ops == eFunCopyConstructor || ops == eFunDestructor );
    ASSERT_FALSE( pOldVal );
    ASSERT_EQ( newVal, TestClass( 1, 2 ) );
  } );
  Property<TestClass>  p(p1);
  p.SubscribeChanged( []( ETrackedOps ops, TestClass newVal, std::unique_ptr<TestClass> pOldVal ) {
    ASSERT_TRUE( ( ops == eOpAssign ) || ( ops == eFunDestructor ) );
    if( ops == eOpAssign ) {
      ASSERT_TRUE( pOldVal && *pOldVal == TestClass( 1, 2 ));
      ASSERT_TRUE( newVal == TestClass( 2, 4 ));
    }
  });
  p = TestClass( 2, 4 );

  auto tc = TestClass( 5, 8 );
  Property<TestClass> p2( tc );

  p2.SubscribeChanged( [] ( ETrackedOps ops, TestClass newVal, std::unique_ptr<TestClass> pOldVal ) {
    ASSERT_TRUE( ( ops == eOpAssign ) || ( ops == eFunDestructor ) );
    if( ops == eOpAssign ) {
      ASSERT_TRUE( pOldVal && *pOldVal == TestClass( 5, 8 ) );
      ASSERT_TRUE( newVal == TestClass( 10, 456 ) );
    }
  } );
}

struct TestClass
{
  TestClass( int i = 0, int j = 0 ) : x{ i }, y{ j }
  {}

  TestClass( const TestClass& other ) : x{ other.x }, y{ other.y }
  {};
  int x, y;

  bool operator==( const TestClass& other ) const
  { return ( x == other.x && y == other.y ); }

  inline TestClass& operator+=( const TestClass& other )
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  inline TestClass& operator-=( const TestClass& other )
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  inline TestClass& operator*=( const TestClass& other )
  {
    x *= other.x;
    y *= other.y;
    return *this;
  }

  inline TestClass& operator/=( const TestClass& other )
  {
    x /= other.x;
    y /= other.y;
    return *this;
  }

  inline TestClass operator++(int)
  {
    TestClass retval = *this;
    x++;
    y++;
    return retval;
  }
  inline TestClass& operator++()
  {
    x++;
    y++;
    return *this;
  }
  inline TestClass operator--(int)
  {
    TestClass retval = *this;
    x--;
    y--;
    return retval;
  }
  inline TestClass& operator--()
  {
    x--;
    y--;
    return *this;
  }

};

TEST( PropertyShould, IncDecClass )
{
  Property<TestClass> p1{ TestClass{ 1, 2 } };
  p1.SubscribeChanged( [ ] ( ETrackedOps ops, const TestClass& newVal, std::unique_ptr<TestClass> pOldVal ) {
    ASSERT_TRUE( ops == eFunDestructor || ops == eOpPostIncrement );
    if( ops == eOpPostIncrement ) {
      ASSERT_EQ( newVal, TestClass( 2, 3 ) );
      ASSERT_TRUE( pOldVal );
      ASSERT_EQ( *pOldVal, TestClass( 1, 2 ) );
    }
  });
  p1++;
  p1.SubscribeChanged( [] ( ETrackedOps ops, const TestClass &newVal, std::unique_ptr<TestClass> pOldVal ) {
    ASSERT_TRUE( ops == eFunDestructor || ops == eOpPostDecrement );
    if( ops == eOpPostDecrement ) {
      ASSERT_EQ( newVal, TestClass( 1, 2 ) );
      ASSERT_TRUE( pOldVal );
      ASSERT_EQ( *pOldVal, TestClass( 2, 3 ) );
    }
  });
  auto p2 = p1--;
  ASSERT_EQ( p2, TestClass( 2, 3 ) );
  auto p3 = p2;
  ASSERT_EQ( p2, p3 );
}

