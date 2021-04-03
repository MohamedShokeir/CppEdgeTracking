cmake_minimum_required(VERSION 3.7)

project(EdgeTracking)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
set(CMAKE_OSX_ARCHITECTURES "arm64")

#Find all headers
include_directories(include)

# Required packages
find_package(OpenCV 4.1 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# Find all executables
file(GLOB project_SRCS src/*.cpp) 

# Add project executable
add_executable(ET ${project_SRCS})
target_link_libraries(ET ${OpenCV_LIBRARIES})