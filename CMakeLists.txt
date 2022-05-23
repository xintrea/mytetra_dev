cmake_minimum_required(VERSION 3.22)
project("mytetra" LANGUAGES "C" "CXX")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
include(GetGitRevisionDescription)
git_describe_working_tree(APP_VERSION)


message("compiler: ${CMAKE_CXX_COMPILER_ID}\nbuild type: ${CMAKE_BUILD_TYPE}\napp version:${APP_VERSION}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "/W3 /WX /EHsc /O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(GCC_WARNS "-Wall -Wextra -Werror")
    set(CMAKE_C_FLAGS_DEBUG     "${GCC_WARNS} -O0 -ggdb3")
    set(CMAKE_C_FLAGS_RELEASE   "${GCC_WARNS} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

    include(CheckPIESupported)
    check_pie_supported() # rpmlint wants PIE
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(FATAL_ERROR "building under MacOS not supported")
endif()

find_package(Qt6 COMPONENTS Core Linguist Network PrintSupport Widgets Xml REQUIRED)
set(QT_LIBS Qt6::Core Qt6::Network Qt6::PrintSupport Qt6::Widgets Qt6::Xml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(USE_SYSTEM_MIMETEX "If off, build own version of mimetex. Major linux distros have mimetex package, so no need one more" ON)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(GNUInstallDirs)
    set(CMAKE_INSTALL_PREFIX "/usr")
    install(FILES "app/desktop/any/mytetra.desktop" DESTINATION "${CMAKE_INSTALL_DATADIR}/applications/")
    install(FILES "app/desktop/mytetra.png" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps/")
    install(FILES "app/desktop/mytetra.svg" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/scalable/apps/")
endif()

add_subdirectory("app/src")
if (NOT USE_SYSTEM_MIMETEX)
    add_subdirectory("thirdParty/mimetex")
endif()

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if (DOXYGEN_FOUND)
    # output inside "${CMAKE_CURRENT_BINARY_DIR}/html/"
    doxygen_add_docs(doc ".." WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/app/doc")
endif()


set(CPACK_PACKAGE_NAME "mytetra")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Personal manager for information accumulation")
set(CPACK_PACKAGE_VERSION "${APP_VERSION}")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://webhamster.ru/site/page/index/articles/projectcode/105")
set(CPACK_STRIP_FILES TRUE)
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_DESCRIPTION "This is effective personal manager for information accumulation.")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/share/icons")

string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+" DEBIAN_APP_VERSION "${APP_VERSION}")
set(CPACK_DEBIAN_PACKAGE_VERSION "${DEBIAN_APP_VERSION}")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_RPM_PACKAGE_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sergey M. Stepanov <xintrea@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6,libqt6core6,libqt6gui6,libqt6network6,libqt6printsupport6,libqt6widgets6,libqt6xml6")
set(CPACK_GENERATOR "DEB" "RPM")
include(CPack)


# Use only Release build type when building packages.
# Generate packages with 'cpack -G DEB/RPM'.
# To build RPM need to install cmake gcc-c++ git rpmdevtools qt6-qtbase qt6-tools.
# Recommend to check result with rpmlint/lintian.
# To build DEB need to install cmake g++ linguist-qt6 qt6-base-dev qt6-l10n-tools qt6-tools-dev qt6-tools-dev-tools.
# If using virtual machine need to install libgl1-mesa-dev also (https://forum.qt.io/post/633525).


# to run static analysis:
# scan-build cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ <SRC_DIR>
# scan-build make

# TODO:
# more warnings check
# static analyzer
# option for build crypto
# packages. rpm/deb is supported, some windows packages also. Don't know about MacOs/android.
