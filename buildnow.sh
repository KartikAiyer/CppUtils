#!/bin/bash
# Build script for CppUtils
# This build script is useful if you only intend to build the unit tests
# If you plan on including the code library in your project just use
# add_subdirectory in your CMakeLists.txt file
# 
# To build command line version
# $>./buildnow.sh <clean - for a fresh build>
#
# To build Xcode project
# $> ./buildnow.sh xcode
#
# All build files are created in either the "build" (for command line) or buildX (Xcode build)
# folders. 

BUILD_DIR="build"
declare -i XCODE_BUILD=0
declare -i CLEAN_BUILD=0
while [[ $# > 0 ]]
do
  key="$1"
  case $key in
    clean)
      CLEAN_BUILD=1 
    ;;
    xcode)
      BUILD_DIR="buildX"
      XCODE_BUILD=1
    ;;
  esac
  shift
done

if [[ $CLEAN_BUILD == 1 && -d "$BUILD_DIR" ]]; then
  rm -rf "$BUILD_DIR"
fi

if [[ ! -d "$BUILD_DIR" ]]; then
  mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"
if [[ $XCODE_BUILD == 1 ]]; then
  cmake -DCPPUTIL_BUILD_TESTS:BOOL=On -G Xcode ..
  if [[ $? == 0 ]]; then
    open $(find . -iname "*xcodeproj" -maxdepth 1 | head)
  fi
else
  cmake -DCPPUTIL_BUILD_TESTS:BOOL=On ..
  if [[ $? == 0 ]]; then
    make
  fi
fi

