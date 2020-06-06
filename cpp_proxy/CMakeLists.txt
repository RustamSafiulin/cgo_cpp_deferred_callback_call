cmake_minimum_required (VERSION 3.8)
 
project(cpp_proxy)
 
set(SOURCE_FILES "proxy.cpp")
set(HEADER_FILES "proxy.h")
 
# We declare the project as a static library and add all the source code files to it.
add_library(proxy SHARED ${HEADER_FILES} ${SOURCE_FILES})
