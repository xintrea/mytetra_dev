add_library(qtsingleapplication SHARED "qtlocalpeer.cpp" "qtsingleapplication.cpp")
target_link_libraries(qtsingleapplication PRIVATE ${QT_LIBS})
target_compile_definitions(qtsingleapplication PRIVATE QT_QTSINGLEAPPLICATION_LIBRARY)


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS qtsingleapplication DESTINATION "${CMAKE_INSTALL_LIBDIR}/mytetra/")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(TARGETS qtsingleapplication DESTINATION "${WIN_INSTALL_DIR}")
endif()
