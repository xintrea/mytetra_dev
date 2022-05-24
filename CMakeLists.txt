cmake_minimum_required(VERSION 3.22)
project("mytetra" LANGUAGES "C" "CXX")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_FLAGS_WARN "-Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu17 ${CMAKE_FLAGS_WARN}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ${CMAKE_FLAGS_WARN}")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

find_package(Qt6 COMPONENTS Core Network PrintSupport Widgets Xml REQUIRED)
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

# TODO:
# more warnings check
# static analyzer
# packages. rpm/deb is supported, some windows packages also. Don't know about MacOs/android.
