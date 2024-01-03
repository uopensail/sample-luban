cmake_minimum_required(VERSION 3.16)
project(sample_luban)

message(STATUS CMAKE_SYSTEM_VERSION)
message(STATUS CMAKE_HOST_SYSTEM_VERSION)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g -ggdb -DDEBUG -Wall -std=c++17 -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall -std=c++17 -fPIC")

if(CMAKE_COMPILER_IS_GNUCXX)
add_compile_definitions(__GCC__)
endif(CMAKE_COMPILER_IS_GNUCXX)

if(APPLE)
#add_compile_definitions(_LIBCPP_DISABLE_AVAILABILITY)
endif(APPLE)


include_directories(include)
include_directories(plugins)
include_directories(/usr/local/include)
include_directories(/usr/include)
 
link_directories(/usr/local/lib)

include_directories(sol2/include)
include_directories(luban/include)
add_subdirectory(luban)

SET(SAMPLE_SOURCE src/pool.cc src/lua_plugin.cc src/sample.cc)


add_library(sample_luban_static STATIC ${SAMPLE_SOURCE})
target_link_libraries(sample_luban_static PUBLIC liblua.a  luban_static pthread)
add_dependencies(sample_luban_static luban_static)

option(PYBIND "Description of PYBIND" ON)
if (PYBIND)
    # Find and link to Python
    find_package(Python3 COMPONENTS Interpreter Development)
    include_directories(${Python3_INCLUDE_DIRS})
    message(STATUS "PYTHON3_LIBRARIES = ${Python3_LIBRARIES}")
    message(STATUS "PYTHON3_EXECUTABLE = ${PYTHON_EXECUTABLE}")
    message(STATUS "PYTHON3_INCLUDE_DIRS = ${Python3_INCLUDE_DIRS}")


    # Include pybind11
    find_package(pybind11 REQUIRED)
    # Define the Python module
    pybind11_add_module(pysampletoolkit  src/pysample.cc )
    target_link_libraries(pysampletoolkit PRIVATE liblua.a sample_luban_static luban_static pthread)
endif()
