cmake_minimum_required(VERSION 3.22)
project("mytetra" LANGUAGES "C" "CXX")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

message("compiler: ${CMAKE_CXX_COMPILER_ID}\nbuild type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "/W3 /WX /EHsc /O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(GCC_WARNS "-Wall -Wextra -Werror")
    set(CMAKE_C_FLAGS_DEBUG     "${GCC_WARNS} -O0 -ggdb3")
    set(CMAKE_C_FLAGS_RELEASE   "${GCC_WARNS} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(FATAL_ERROR "building under MacOS not supported")
endif()

find_package(Qt6 COMPONENTS Core Linguist Network PrintSupport Widgets Xml REQUIRED)
set(QT_LIBS Qt6::Core Qt6::Network Qt6::PrintSupport Qt6::Widgets Qt6::Xml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(USE_SYSTEM_MIMETEX "If off, build own version of mimetex. Major linux distros have mimetex package, so no need one more" ON)

add_subdirectory("app/src")
if (NOT USE_SYSTEM_MIMETEX)
    add_subdirectory("thirdParty/mimetex")
endif()

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if (DOXYGEN_FOUND)
    # output inside "${CMAKE_CURRENT_BINARY_DIR}/html/"
    doxygen_add_docs(doc ".." WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/app/doc")
endif()


# to run static analysis:
# scan-build cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ <SRC_DIR>
# scan-build make

# TODO:
# more warnings check
# static analyzer
# option for build crypto
# packages. rpm/deb is supported, some windows packages also. Don't know about MacOs/android.
