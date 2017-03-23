#include <iostream>
#include <gtest/gtest.h>

using namespace std;

int main( int argc, char **argv ) 
{
  cout<<"============================================="<<endl;
  cout<<"Running NeoLib tests..."<<endl;
  cout<<"Run individual tests with option '--gtest_filter'"<<endl;
  cout<<"example: $ ./tests --gtest_filter=NeoLibEntry.GetStatusUpdates"<<endl;
  cout<<"============================================="<<endl;
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
