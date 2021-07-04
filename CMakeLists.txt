cmake_minimum_required(VERSION 2.6)

project(downloader)               

SET(GLOG_INCLUDE_PATH "/opt/workspace/glog/include")
SET(GLOG_LIB_PATH "/opt/workspace/glog/lib")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++0x -fno-omit-frame-pointer -W -Wall -fPIC -Wno-unused-parameter -g -Wno-deprecated -fpermissive -pthread -no-pie")
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++0x -DNDEBUG -fno-omit-frame-pointer -D__const__= -pipe -W -Wall -fPIC -Wno-unused-parameter -O2 -Wall -Wno-deprecated -fpermissive -pthread -no-pie -g ")
set(SRCROOT "${PROJECT_SOURCE_DIR}")

INCLUDE_DIRECTORIES(${GLOG_INCLUDE_PATH})
LINK_DIRECTORIES(${GLOG_LIB_PATH})
set(SRC
	"${SRCROOT}/main.cc"
	"${SRCROOT}/downloader.h"
	"${SRCROOT}/downloader.cc"
	) 

add_executable(downloader ${SRC}) 

target_link_libraries(downloader -lcurl -lglog)
if(CMAKE_COMPILER_IS_GNUCXX)   
	set_target_properties(downloader PROPERTIES COMPILE_FLAGS "-std=c++11 -Wno-multichar")
endif()
